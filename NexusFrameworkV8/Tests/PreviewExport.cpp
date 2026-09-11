#include "Domain/NexusDomain.h"
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
namespace fs=std::filesystem;
namespace
{
fs::path fromUtf8(const std::string& value) {
#if __cplusplus >= 202002L
    std::u8string text; text.reserve(value.size());
    for(unsigned char c:value) text.push_back(static_cast<char8_t>(c));
    return fs::path(text);
#else
    return fs::u8path(value);
#endif
}

std::string pathString(const fs::path& P){auto S=P.u8string();return {reinterpret_cast<const char*>(S.data()),S.size()};}
std::string json(const std::string& S){std::string R;for(unsigned char C:S){if(C=='"'||C=='\\'){R+='\\';R+=char(C);}else if(C=='\n')R+="\\n";else if(C<32)R+=' ';else R+=char(C);}return R;}
void require(bool B,const char* Message){if(!B)throw std::runtime_error(Message);}
void bitmap(const fs::path& Path,const nexus::RoadOutput& Road)
{
    const unsigned W=Road.textureWidth,H=Road.textureHeight,Stride=(W*3+3)&~3u;
    std::ofstream F(Path,std::ios::binary);require(bool(F),"Cannot create bitmap.");
    auto integer=[&](unsigned V,unsigned N){for(unsigned I=0;I<N;++I)F.put(char((V>>(I*8))&255));};
    F<<"BM";integer(54+Stride*H,4);integer(0,4);integer(54,4);integer(40,4);integer(W,4);integer(H,4);integer(1,2);integer(24,2);integer(0,4);integer(Stride*H,4);integer(2835,4);integer(2835,4);integer(0,4);integer(0,4);
    for(unsigned Y=H;Y>0;--Y){for(unsigned X=0;X<W;++X){const auto I=((Y-1)*W+X)*4;F.put(char(Road.markingRGBA[I+2]));F.put(char(Road.markingRGBA[I+1]));F.put(char(Road.markingRGBA[I]));}for(unsigned X=W*3;X<Stride;++X)F.put(0);}
    require(bool(F),"Bitmap write failed.");
}
}
int main(int argc,char** argv)
{
    try
    {
        const fs::path Out=argc>1?fromUtf8(argv[1]):fs::path("preview-output");
        fs::create_directories(Out);
        std::uint64_t Id=1;nexus::Document D=nexus::makeExample([&]{return nexus::fixtureId(Id++);});
        if(argc>2){const auto E=nexus::loadFile(argv[2],D);if(nexus::hasErrors(E)){for(const auto& I:E)std::cerr<<I.code<<": "<<I.message<<'\n';return 2;}}
        if(D.sourceWorldKey!="Demo://NexusV8"){std::cerr<<"SOURCE_REQUIRED: external Landscape geometry must be read inside Unreal; it is deliberately not cached in .nv8 documents.\n";return 2;}
        const auto Source=nexus::makeExampleScan();nexus::CompileOptions Options;Options.requireAssets=false;
        const auto R=nexus::compilePreview(D,Source,{},Options);
        if(!R.success){for(const auto& I:R.issues)std::cerr<<I.code<<": "<<I.message<<'\n';return 1;}
        const auto DocumentPath=pathString(Out/"Demo-6-4-2.nv8");
        require(nexus::saveAtomic(DocumentPath,D,nexus::fileFingerprint(DocumentPath)).saved,"Could not save example .nv8 document.");
        std::ofstream Obj(Out/"Road-network.obj"),Mtl(Out/"Road-network.mtl"),Lanes(Out/"Lane-corridors.json"),Manifest(Out/"Preview-manifest.json");
        require(bool(Obj)&&bool(Mtl)&&bool(Lanes)&&bool(Manifest),"Cannot create export files.");
        Obj<<std::setprecision(12)<<"# Actual shared C++ Phase 2 preview. Units: meters, Z up. Not a production runtime graph.\nmtllib Road-network.mtl\n";
        std::size_t Index=1,RoadIndex=0,Vertices=0,Triangles=0,Corridors=0;
        Lanes<<std::setprecision(12)<<"{\"productionPublishable\":false,\"snapshot\":\""<<R.snapshot.contentHash<<"\",\"corridors\":[";bool First=true;
        for(const auto& Road:R.snapshot.roads)
        {
            const std::string Texture="Road-"+std::to_string(RoadIndex)+"-markings.bmp";bitmap(Out/Texture,Road);
            for(const auto& S:Road.surfaces)
            {
                if(S.indices.empty())continue;
                const std::string Name="Road"+std::to_string(RoadIndex)+"_"+S.role;
                Obj<<"o "<<Name<<"\nusemtl "<<Name<<"\n";Mtl<<"newmtl "<<Name<<"\n";
                if(S.role=="Road")Mtl<<"Kd 1 1 1\nmap_Kd "<<Texture<<"\n";
                else if(S.role.find("Sidewalk")==0)Mtl<<"Kd 0.25 0.28 0.3\n";else Mtl<<"Kd 0.48 0.49 0.5\n";
                Mtl<<"Ks 0 0 0\nNs 4\n\n";
                double Length=1;for(const auto& V:S.vertices)if(V.v>Length)Length=V.v;
                for(const auto& V:S.vertices)Obj<<"v "<<V.position.x<<' '<<V.position.y<<' '<<V.position.z<<'\n';
                for(const auto& V:S.vertices)Obj<<"vt "<<V.u<<' '<<(1-V.v/Length)<<'\n';
                for(const auto& V:S.vertices)Obj<<"vn "<<V.normal.x<<' '<<V.normal.y<<' '<<V.normal.z<<'\n';
                for(std::size_t I=0;I<S.indices.size();I+=3){Obj<<"f";for(unsigned J=0;J<3;++J){auto N=Index+S.indices[I+J];Obj<<' '<<N<<'/'<<N<<'/'<<N;}Obj<<'\n';}
                Index+=S.vertices.size();Vertices+=S.vertices.size();Triangles+=S.indices.size()/3;
            }
            for(const auto& C:Road.corridors)
            {
                if(!First)Lanes<<',';First=false;++Corridors;
                Lanes<<"{\"id\":\""<<C.id<<"\",\"segment\":\""<<C.segment<<"\",\"fromProfileLane\":\""<<C.fromLane<<"\",\"toProfileLane\":\""<<C.toLane<<"\",\"direction\":\""<<(C.direction==nexus::Direction::Forward?"Forward":"Reverse")<<"\",\"accessMask\":"<<C.access<<",\"samples\":[";
                bool Start=true;for(const auto& S:C.samples){if(!Start)Lanes<<',';Start=false;Lanes<<"{\"stationM\":"<<S.stationM<<",\"widthM\":"<<S.widthM<<",\"centerM\":["<<S.center.x<<','<<S.center.y<<','<<S.center.z<<"],\"leftM\":["<<S.left.x<<','<<S.left.y<<','<<S.left.z<<"],\"rightM\":["<<S.right.x<<','<<S.right.y<<','<<S.right.z<<"]}";}Lanes<<"]}";
            }
            ++RoadIndex;
        }
        Lanes<<"]}\n";
        Manifest<<"{\n  \"generator\":\"Actual shared C++ Phase 1–2 core\",\n  \"productionPublishable\":false,\n  \"unrealVerification\":\"NOT_EXECUTED\",\n  \"documentId\":\""<<D.id<<"\",\n  \"documentFingerprint\":\""<<R.snapshot.documentFingerprint<<"\",\n  \"sourceFingerprint\":\""<<R.snapshot.sourceFingerprint<<"\",\n  \"snapshotFingerprint\":\""<<R.snapshot.contentHash<<"\",\n  \"roads\":"<<RoadIndex<<",\n  \"vertices\":"<<Vertices<<",\n  \"triangles\":"<<Triangles<<",\n  \"corridors\":"<<Corridors<<",\n  \"units\":\"meters\"\n}\n";
        require(bool(Obj)&&bool(Mtl)&&bool(Lanes)&&bool(Manifest),"Export write failed.");
        std::cout<<"EXPORT OK: "<<RoadIndex<<" roads, "<<Triangles<<" triangles, "<<Corridors<<" corridors.\nPreview only; no Unreal build or runtime AI execution is implied.\n";
        return 0;
    }
    catch(const std::exception& E){std::cerr<<"EXPORT FAILED: "<<E.what()<<'\n';return 1;}
}
