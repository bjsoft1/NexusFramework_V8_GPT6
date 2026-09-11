#include "Domain/NexusDomain.h"
#include <cstring>
#include <algorithm>
#include <limits>
#include <type_traits>
namespace nexus {
    namespace {
        constexpr std::size_t MaxBytes=64u*1024u*1024u,MaxCount=100000;
        struct Writer {
            Bytes b;
            void integer(std::uint64_t v,unsigned count){
                for(unsigned i=0;i<count;++i)b.push_back(static_cast<std::uint8_t>((v>>(8*i))&255));
            }
            void value(bool v){
                integer(v?1:0,1);
            }
            void value(std::uint32_t v){
                integer(v,4);
            }
            void value(std::uint64_t v){
                integer(v,8);
            }
            void value(double v){
                static_assert(sizeof(double)==8,"IEEE-754 double required");
                std::uint64_t n;
                std::memcpy(&n,&v,8);
                integer(n,8);
            }
            void value(const std::string&s){
                value(static_cast<std::uint32_t>(s.size()));
                b.insert(b.end(),s.begin(),s.end());
            }
            template<class E,std::enable_if_t<std::is_enum<E>::value,int> =0>void value(E e){
                integer(static_cast<unsigned>(e),1);
            }
        };
        struct Reader {
            const Bytes&b;
            std::size_t pos=0;
            bool ok=true;
            std::uint64_t integer(unsigned count){
                if(count>b.size()-std::min(pos,b.size())){
                    ok=false;
                    return 0;
                }
                std::uint64_t n=0;
                for(unsigned i=0;i<count;++i)n|=static_cast<std::uint64_t>(b[pos++])<<(8*i);
                return n;
            }
            void value(bool&v){
                auto n=integer(1);
                if(n>1)ok=false;
                v=n!=0;
            }
            void value(std::uint32_t&v){
                v=static_cast<std::uint32_t>(integer(4));
            }
            void value(std::uint64_t&v){
                v=integer(8);
            }
            void value(double&v){
                auto n=integer(8);
                std::memcpy(&v,&n,8);
            }
            void value(std::string&s){
                std::uint32_t n=0;
                value(n);
                if(n>4u*1024u*1024u||n>b.size()-std::min(pos,b.size())){
                    ok=false;
                    return;
                }
                if(!ok)return;
                if(n==0){
                    s.clear();
                    return;
                }
                s.assign(reinterpret_cast<const char*>(b.data()+pos),n);
                pos+=n;
            }
            template<class E,std::enable_if_t<std::is_enum<E>::value,int> =0>void value(E&e){
                e=static_cast<E>(integer(1));
            }
        };
        template<class A,class... T>void fields(A&a,T&...v){
            (a.value(v),...);
        }
        // Object field order is the v1 schema. Adding fields requires an explicit new version.
        template<class A>void record(A&a,Vec3&v){
            fields(a,v.x,v.y,v.z);
        }
        template<class A>void record(A&a,Quat&v){
            fields(a,v.x,v.y,v.z,v.w);
        }
        template<class A>void record(A&a,Pose&v){
            record(a,v.position);
            record(a,v.rotation);
        }
        template<class A>void record(A&a,Payload&v){
            fields(a,v.type,v.version,v.enabled,v.bytes);
        }
        template<class A>void record(A&a,Entity&v){
            fields(a,v.id,v.parent,v.kind,v.name,v.catalog,v.defaultProfile,v.drivingSide,v.sourceKey,v.availability,v.acceptedSourceFingerprint,v.anchorSegment,v.anchorAtEnd);
            record(a,v.local);
            record(a,v.payload);
        }
        template<class A>void record(A&a,Lane&v){
            fields(a,v.id,v.direction,v.ordinal,v.widthM,v.speedMps,v.access);
        }
        template<class A>void record(A&, Profile&);
        template<class A>void record(A&, Section&);
        template<class A>void record(A&, LaneLink&);
        template<class A>void record(A&, Adapter&);
        template<class A>void record(A&, Segment&);
        template<class A>void record(A&, Port&);
        template<class A>void record(A&, MeshInterface&);
        template<class A>void record(A&, MeshEntry&);
        template<class A>void record(A&, Catalog&);
        template<class A>void record(A&, SharedTopology&);
        template<class A>void record(A&, Document&);
        template<class T>void list(Writer&a,std::vector<T>&v){
            a.value(static_cast<std::uint32_t>(v.size()));
            for(auto&x:v)record(a,x);
        }
        template<class T>void list(Reader&a,std::vector<T>&v){
            std::uint32_t n=0;
            a.value(n);
            if(n>MaxCount){
                a.ok=false;
                return;
            }
            for(std::uint32_t i=0;i<n&&a.ok;++i){
                T x{
                };
                record(a,x);
                v.push_back(std::move(x));
            }
        }
        template<class A>void scalarList(A&a,std::vector<double>&v){
            if constexpr(std::is_same<A,Writer>::value){
                a.value(static_cast<std::uint32_t>(v.size()));
                for(auto x:v)a.value(x);
            }else{
                std::uint32_t n=0;
                a.value(n);
                if(n>1024){
                    a.ok=false;
                    return;
                }
                for(std::uint32_t i=0;i<n&&a.ok;++i){
                    double x=0;
                    a.value(x);
                    v.push_back(x);
                }
            }
        }
        template<class A>void ids(A&a,std::vector<Id>&v){
            if constexpr(std::is_same<A,Writer>::value){
                a.value(static_cast<std::uint32_t>(v.size()));
                for(auto&x:v)a.value(x);
            }else{
                std::uint32_t n=0;
                a.value(n);
                if(n>MaxCount){
                    a.ok=false;
                    return;
                }
                for(std::uint32_t i=0;i<n&&a.ok;++i){
                    Id x;
                    a.value(x);
                    v.push_back(std::move(x));
                }
            }
        }
        template<class A>void record(A&a,Profile&v){
            fields(a,v.id,v.name);
            list(a,v.lanes);
            fields(a,v.medianM,v.sidewalkLeftM,v.sidewalkRightM,v.borderLeftM,v.borderRightM,v.sidewalkHeightM);
        }
        template<class A>void record(A&a,Section&v){
            fields(a,v.startM,v.profile);
        }
        template<class A>void record(A&a,LaneLink&v){
            fields(a,v.id,v.fromLane,v.toLane,v.direction,v.kind,v.approved);
        }
        template<class A>void record(A&a,Adapter&v){
            fields(a,v.id,v.startM,v.endM,v.before,v.after);
            list(a,v.links);
            fields(a,v.minimumTaperRatio);
        }
        template<class A>void record(A&a,Segment&v){
            fields(a,v.id,v.highway,v.startPoint,v.endPoint,v.sourceKey,v.acceptedSourceFingerprint,v.availability);
            list(a,v.sections);
            list(a,v.adapters);
        }
        template<class A>void record(A&a,Port&v){
            fields(a,v.name);
            scalarList(a,v.forwardWidths);
            scalarList(a,v.reverseWidths);
            fields(a,v.sidewalkLeftM,v.sidewalkRightM,v.borderLeftM,v.borderRightM);
        }
        template<class A>void record(A&a,MeshInterface&v){
            fields(a,v.metersPerUnit,v.forwardAxis,v.upAxis,v.pivot,v.lengthM,v.openEnds,v.textureMarkings);
            list(a,v.ports);
        }
        template<class A>void record(A&a,MeshEntry&v){
            fields(a,v.assignment,v.assetPath);
            record(a,v.interface);
        }
        template<class T>void dictionary(Writer&a,std::map<std::string,T>&v){
            a.value(static_cast<std::uint32_t>(v.size()));
            for(auto&kv:v){
                a.value(kv.first);
                record(a,kv.second);
            }
        }
        template<class T>void dictionary(Reader&a,std::map<std::string,T>&v){
            std::uint32_t n=0;
            a.value(n);
            if(n>MaxCount){
                a.ok=false;
                return;
            }
            for(std::uint32_t i=0;i<n&&a.ok;++i){
                std::string k;
                T x;
                a.value(k);
                record(a,x);
                if(!v.emplace(std::move(k),std::move(x)).second)a.ok=false;
            }
        }
        template<class A>void record(A&a,Catalog&v){
            fields(a,v.id,v.name);
            dictionary(a,v.entries);
        }
        template<class A>void record(A&a,SharedTopology&v){
            fields(a,v.id,v.meshOwnerHighway,v.type);
            ids(a,v.approachPoints);
        }
        template<class A>void record(A&a,Document&v){
            fields(a,v.schema,v.id,v.revision,v.sourceWorldKey);
            dictionary(a,v.entities);
            dictionary(a,v.segments);
            dictionary(a,v.profiles);
            dictionary(a,v.catalogs);
            dictionary(a,v.topology);
        }
    }
    Bytes encode(const Document&d){
        Document copy=d;
        Writer body;
        record(body,copy);
        Writer out;
        out.value(std::string("NEXUSV8DOC"));
        out.value(SchemaVersion);
        out.value(static_cast<std::uint64_t>(body.b.size()));
        out.value(fingerprint(std::string(body.b.begin(),body.b.end())));
        out.b.insert(out.b.end(),body.b.begin(),body.b.end());
        return out.b;
    }
    Issues decode(const Bytes&b,Document&out){
        auto bad=[](const std::string&message){
            return Issues{
                {
                    "DOCUMENT_DECODE",{
                    },message,true
                }
            };
        };
        if(b.size()>MaxBytes)return bad("Document exceeds the 64 MiB safety limit.");
        Reader r{
            b
        };
        std::string magic,checksum;
        std::uint32_t version=0;
        std::uint64_t count=0;
        r.value(magic);
        r.value(version);
        r.value(count);
        r.value(checksum);
        if(!r.ok||magic!="NEXUSV8DOC")return bad("Not a Nexus V8 document. Legacy UAssets are not automatically converted.");
        if(version!=SchemaVersion)return {
            {
                "UNSUPPORTED_SCHEMA",{
                },"Future/legacy schema rejected; input and current document remain unchanged.",true
            }
        };
        if(count!=b.size()-r.pos)return bad("Truncated data or trailing bytes.");
        Bytes body(b.begin()+static_cast<std::ptrdiff_t>(r.pos),b.end());
        if(fingerprint(std::string(body.begin(),body.end()))!=checksum)return bad("Document checksum mismatch.");
        Reader contents{
            body
        };
        Document candidate;
        record(contents,candidate);
        if(!contents.ok||contents.pos!=body.size())return bad("Malformed field count, duplicate map key, or invalid field encoding.");
        auto issues=validate(candidate);
        if(hasErrors(issues))return issues;
        out=std::move(candidate);
        return issues;
    }
    std::string contentFingerprint(const Document&d){
        auto bytes=encode(d);
        return fingerprint(std::string(bytes.begin(),bytes.end()));
    }
}
// namespace nexus
