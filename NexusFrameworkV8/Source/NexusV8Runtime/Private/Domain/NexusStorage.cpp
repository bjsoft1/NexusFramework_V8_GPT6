#include "Domain/NexusDomain.h"
#include <filesystem>
#include <fstream>
#include <system_error>
#ifdef _WIN32
#ifndef NEXUS_STANDALONE
#include "Windows/AllowWindowsPlatformTypes.h"
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#ifndef NEXUS_STANDALONE
#include "Windows/HideWindowsPlatformTypes.h"
#endif
#else
#include <fcntl.h>
#include <unistd.h>
#endif
namespace nexus {
    namespace fs=std::filesystem;
    namespace {
fs::path fromUtf8(const std::string& value) {
#if __cplusplus >= 202002L
    std::u8string text; text.reserve(value.size());
    for(unsigned char c:value) text.push_back(static_cast<char8_t>(c));
    return fs::path(text);
#else
    return fs::u8path(value);
#endif
}

        Issue fail(std::string code,const std::string&msg){
            return {
                std::move(code),{
                },msg,true
            };
        }
        Bytes readBytes(const fs::path&p,bool&ok){
            ok=false;
            std::error_code ec;
            auto size=fs::file_size(p,ec);
            if(ec||size>64u*1024u*1024u)return {
            };
            std::ifstream f(p,std::ios::binary);
            if(!f)return {
            };
            Bytes b(static_cast<std::size_t>(size));
            if(size)f.read(reinterpret_cast<char*>(b.data()),static_cast<std::streamsize>(size));
            ok=static_cast<bool>(f);
            return b;
        }
        bool syncFile(const fs::path&p){
#ifdef _WIN32
            HANDLE h=CreateFileW(p.c_str(),GENERIC_WRITE,FILE_SHARE_READ,nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);
            if(h==INVALID_HANDLE_VALUE)return false;
            bool ok=FlushFileBuffers(h)!=0;
            CloseHandle(h);
            return ok;
#else
            int fd=::open(p.c_str(),O_RDONLY);
            if(fd<0)return false;
            bool ok=::fsync(fd)==0;
            ::close(fd);
            return ok;
#endif
        }
        bool promote(const fs::path&src,const fs::path&dst,std::error_code&ec){
#ifdef _WIN32
            if(MoveFileExW(src.c_str(),dst.c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH))return true;
            ec=std::error_code(static_cast<int>(GetLastError()),std::system_category());
            return false;
#else
            fs::rename(src,dst,ec);
            return !ec;
#endif
        }
        void syncDirectory(const fs::path&p){
#ifndef _WIN32
            int fd=::open(p.c_str(),O_RDONLY|O_DIRECTORY);
            if(fd>=0){
                ::fsync(fd);
                ::close(fd);
            }
#else
            (void)p;
#endif
        }
        struct Lock {
            fs::path p;
            bool owned=false;
            ~Lock(){
                if(owned){
                    std::error_code e;
                    fs::remove(p,e);
                }
            }
        };
    }
    std::optional<std::string> fileFingerprint(const std::string&path){
        bool ok=false;
        auto b=readBytes(fromUtf8(path),ok);
        if(!ok)return {
        };
        return fingerprint(std::string(b.begin(),b.end()));
    }
    Issues loadFile(const std::string&path,Document&out){
        bool ok=false;
        auto b=readBytes(fromUtf8(path),ok);
        if(!ok)return {
            fail("FILE_READ","Cannot read document, or file is larger than 64 MiB: "+path)
        };
        return decode(b,out);
    }
    static SaveResult writeAtomic(const std::string&name,const Bytes&bytes,const std::optional<std::string>&expected,SaveFault fault){
        SaveResult r;
        const fs::path path=fromUtf8(name);
        std::error_code ec;
        const auto directory=path.has_parent_path()?path.parent_path():fs::path(".");
        fs::create_directories(directory,ec);
        if(ec){
            r.issues.push_back(fail("SAVE_DIRECTORY",ec.message()));
            return r;
        }
        Lock lock{
            fs::path(path).concat(".lock"),false
        };
        lock.owned=fs::create_directory(lock.p,ec);
        if(!lock.owned){
            r.issues.push_back(fail("SAVE_LOCK","Another writer or retained crash lock exists. No source file was changed."));
            return r;
        }
        const auto current=fileFingerprint(name);
        const bool exists=fs::exists(path,ec);
        if(ec||(exists&&!current)||current!=expected){
            r.issues.push_back(fail("SAVE_CONFLICT","File revision/hash differs from the opened baseline; reload/reconcile rather than overwrite."));
            return r;
        }
        if(fault==SaveFault::BeforeWrite){
            r.issues.push_back(fail("INJECTED_FAILURE","Failure before writing staging file."));
            return r;
        }
        if(bytes.size()>64u*1024u*1024u){
            r.issues.push_back(fail("SAVE_SIZE","Document exceeds limit."));
            return r;
        }
        auto stage=fs::path(path).concat(".staging");
        {
            std::ofstream f(stage,std::ios::binary|std::ios::trunc);
            if(!f){
                r.issues.push_back(fail("SAVE_WRITE","Cannot open staging file."));
                return r;
            }
            f.write(reinterpret_cast<const char*>(bytes.data()),static_cast<std::streamsize>(bytes.size()));
            f.flush();
            if(!f){
                r.issues.push_back(fail("SAVE_WRITE","Staging write failed."));
                return r;
            }
        }
        if(!syncFile(stage)){
            r.issues.push_back(fail("SAVE_SYNC","Staging flush failed; previous file preserved."));
            return r;
        }
        bool verified=false;
        auto check=readBytes(stage,verified);
        if(!verified||check!=bytes){
            r.issues.push_back(fail("SAVE_VERIFY","Staging readback failed; previous file preserved."));
            return r;
        }
        if(fault==SaveFault::BeforePromote){
            r.issues.push_back(fail("INJECTED_FAILURE","Failure before promotion; previous document preserved, staging retained for diagnosis."));
            return r;
        }
        if(current){
            auto backupStage=fs::path(path).concat(".backup.staging");
            auto backup=fs::path(path).concat(".bak");
            fs::copy_file(path,backupStage,fs::copy_options::overwrite_existing,ec);
            if(ec||!syncFile(backupStage)||!promote(backupStage,backup,ec)){
                r.issues.push_back(fail("SAVE_BACKUP","Backup failed; active document preserved."));
                return r;
            }
        }
        if(!promote(stage,path,ec)){
            r.issues.push_back(fail("SAVE_PROMOTE",ec.message()));
            return r;
        }
        syncDirectory(directory);
        r.saved=true;
        r.hash=fingerprint(std::string(bytes.begin(),bytes.end()));
        return r;
    }
    SaveResult saveAtomic(const std::string&path,const Document&doc,const std::optional<std::string>&expected,SaveFault fault){
        auto issues=validate(doc);
        if(hasErrors(issues)){
            SaveResult r;
            r.issues=std::move(issues);
            return r;
        }
        return writeAtomic(path,encode(doc),expected,fault);
    }
    SaveResult saveDraftAtomic(const std::string&path,const Document&base,const Document&draft,const std::optional<std::string>&expected){
        auto issues=validate(draft);
        if(base.id!=draft.id)issues.push_back(fail("DRAFT_ID","Draft must belong to the same document."));
        if(hasErrors(issues)){
            SaveResult r;
            r.issues=std::move(issues);
            return r;
        }
        const std::string header="NEXUSV8DRAFT\n"+contentFingerprint(base)+"\n";
        Bytes b(header.begin(),header.end());
        auto data=encode(draft);
        b.insert(b.end(),data.begin(),data.end());
        return writeAtomic(path,b,expected,SaveFault::None);
    }
    Issues recoverDraft(const std::string&path,const Document&current,Document&out){
        bool ok=false;
        auto bytes=readBytes(fromUtf8(path),ok);
        const std::string prefix="NEXUSV8DRAFT\n";
        if(!ok||bytes.size()<prefix.size()+17||std::string(bytes.begin(),bytes.begin()+static_cast<std::ptrdiff_t>(prefix.size()))!=prefix)return {
            fail("DRAFT_READ","Invalid or unreadable recovery file.")
        };
        std::string base(bytes.begin()+static_cast<std::ptrdiff_t>(prefix.size()),bytes.begin()+static_cast<std::ptrdiff_t>(prefix.size()+16));
        if(base!=contentFingerprint(current))return {
            fail("DRAFT_CONFLICT","Recovery was based on a different committed document. Keep both files and reconcile explicitly.")
        };
        Bytes data(bytes.begin()+static_cast<std::ptrdiff_t>(prefix.size()+17),bytes.end());
        Document candidate;
        auto errors=decode(data,candidate);
        if(hasErrors(errors))return errors;
        if(candidate.id!=current.id)return {
            fail("DRAFT_ID","Recovery belongs to a different document.")
        };
        out=std::move(candidate);
        return errors;
    }
}
// namespace nexus
