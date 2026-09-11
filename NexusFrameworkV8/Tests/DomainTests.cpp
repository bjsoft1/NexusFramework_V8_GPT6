#include "Domain/NexusDomain.h"
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>
using namespace nexus;
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

    std::string pathUtf8(const fs::path& p) {
        auto value=p.u8string();
        return std::string(reinterpret_cast<const char*>(value.data()),value.size());
    }
    int assertions=0;
    void check(bool value,const char*expression,int line){
        ++assertions;
        if(!value)throw std::runtime_error(std::string("line ")+std::to_string(line)+": "+expression);
    }
#define CHECK(x) check(static_cast<bool>(x),#x,__LINE__)
    bool code(const Issues&i,const std::string&c){
        return std::any_of(i.begin(),i.end(),[&](const Issue&x){
            return x.code==c&&x.blocking;
        });
    }
    struct Fixture {
        std::uint64_t seq=1;
        Document d;
        SourceScan source;
        Fixture(){
            d=makeExample([&]{
                return fixtureId(seq++);
            });
            source=makeExampleScan();
        }
        Id next(){
            return fixtureId(seq++);
        }
        Id entity(Kind k)const{
            for(const auto&kv:d.entities)if(kv.second.kind==k)return kv.first;
            throw std::runtime_error("entity missing");
        }
        Segment& segment(int n=0){
            for(auto&kv:d.segments)if(kv.second.sourceKey=="demo#S"+std::to_string(n))return kv.second;
            throw std::runtime_error("segment missing");
        }
        CompileResult compile(){
            CompileOptions o;
            o.requireAssets=false;
            return compilePreview(d,source,{
            },o);
        }
        Id profile(int count)const{
            for(const auto&kv:d.profiles)if(kv.second.lanes.size()==static_cast<std::size_t>(count))return kv.first;
            throw std::runtime_error("profile missing");
        }
    };
    MeshInterface interfaceFor(const Profile&a,const Profile&b){
        MeshInterface m;
        for(const Profile*p:{
            &a,&b
        }){
            Port port;
            port.name=m.ports.empty()?"Start":"End";
            for(auto dir:{
                Direction::Forward,Direction::Reverse
            })for(std::uint32_t i=0;i<32;++i)for(const auto&l:p->lanes)if(l.direction==dir&&l.ordinal==i)(dir==Direction::Forward?port.forwardWidths:port.reverseWidths).push_back(l.widthM);
            port.sidewalkLeftM=p->sidewalkLeftM;
            port.sidewalkRightM=p->sidewalkRightM;
            port.borderLeftM=p->borderLeftM;
            port.borderRightM=p->borderRightM;
            m.ports.push_back(port);
        }
        return m;
    }
    void catalogs(Fixture&f){
        for(auto k:{
            Kind::World,Kind::State,Kind::City,Kind::Highway
        }){
            auto id=f.entity(k);
            Catalog c;
            c.id=f.next();
            c.name="Catalog";
            MeshEntry e;
            e.assignment=Assignment::Assigned;
            e.assetPath="/Game/Road"+std::to_string(static_cast<unsigned>(k));
            e.interface=interfaceFor(f.d.profiles.at(f.profile(6)),f.d.profiles.at(f.profile(6)));
            c.entries["Road.6"]=e;
            f.d.entities.at(id).catalog=c.id;
            f.d.catalogs.emplace(c.id,c);
        }
    }
    AssetProbe probe=[](const std::string&p){
        return p.rfind("/Game/",0)==0&&p.find("Broken")==std::string::npos;
    };
    std::string escape(const std::string&s){
        std::string r;
        for(char c:s){
            if(c=='"'||c=='\\')r+='\\';
            if(c=='\n')r+="\\n";
            else r+=c;
        }
        return r;
    }
}
int main(int argc,char**argv){
    const fs::path output=argc>1?fromUtf8(argv[1]):fs::path("test-output");
    fs::create_directories(output);
    struct Result{
        std::string name,error;
        int checks;
    };
    std::vector<Result>results;
    auto test=[&](const std::string&name,const std::function<void()>&fn){
        int before=assertions;
        std::string err;
        try{
            fn();
        }catch(const std::exception&e){
            err=e.what();
        }
        results.push_back({
            name,err,assertions-before
        });
        std::cout<<(err.empty()?"PASS ":"FAIL ")<<name;
        if(!err.empty())std::cout<<" :: "<<err;
        std::cout<<'\n';
    };
    test("P1.Document.ExampleValid",[]{
        Fixture f;
        CHECK(!hasErrors(validate(f.d)));
        CHECK(f.d.entities.size()==7);
        CHECK(f.d.segments.size()==2);
    });
    test("P1.Hierarchy.ExactLevelsAndFivePlaceTypes",[]{
        Fixture f;
        auto parent=f.entity(Kind::Point);
        for(auto type:{
            "Parking","BusStop","Hospital","CityPark","PetrolPump"
        }){
            Entity e;
            e.id=f.next();
            e.parent=parent;
            e.kind=Kind::Place;
            e.name=type;
            e.payload.type=type;
            CHECK(!hasErrors(addEntity(f.d,e)));
        }
        CHECK(!hasErrors(validate(f.d)));
    });
    test("P1.Hierarchy.RejectCycleAndOrphan",[]{
        Fixture f;
        auto w=f.entity(Kind::World);
        f.d.entities.at(w).parent=f.entity(Kind::Point);
        CHECK(code(validate(f.d),"WORLD_PARENT"));
        f.d.entities.at(w).parent.clear();
        f.d.entities.at(f.entity(Kind::City)).parent=fixtureId(9900);
        CHECK(code(validate(f.d),"OWNERSHIP"));
    });
    test("P1.Hierarchy.NoExtraRoot",[]{
        Fixture f;
        Entity e;
        e.id=f.next();
        e.kind=Kind::World;
        e.name="Root";
        CHECK(code(addEntity(f.d,e),"WORLD_COUNT"));
        CHECK(f.d.entities.size()==7);
    });
    test("P1.Hierarchy.InvalidPlaceParent",[]{
        Fixture f;
        Entity e;
        e.id=f.next();
        e.kind=Kind::Place;
        e.name="Parking";
        e.payload.type="Parking";
        e.parent=f.entity(Kind::Highway);
        CHECK(code(addEntity(f.d,e),"OWNERSHIP"));
    });
    test("P1.Identity.RenameUndoRedo",[]{
        Fixture f;
        Session session(f.d);
        auto id=f.entity(Kind::Highway);
        auto old=f.d.entities.at(id).name;
        CHECK(!hasErrors(renameEntity(session.draft(),id,"Renamed")));
        CHECK(!hasErrors(session.apply()));
        CHECK(session.committed().revision==1);
        CHECK(session.undo());
        CHECK(session.committed().entities.at(id).name==old);
        CHECK(session.redo());
        CHECK(session.committed().entities.at(id).name=="Renamed");
    });
    test("P1.Identity.ReparentPreservesChildren",[]{
        Fixture f;
        Entity e;
        e.id=f.next();
        e.kind=Kind::City;
        e.parent=f.entity(Kind::State);
        e.name="City 2";
        CHECK(!hasErrors(addEntity(f.d,e)));
        auto h=f.entity(Kind::Highway);
        auto count=descendants(f.d,h).size();
        CHECK(!hasErrors(reparentEntity(f.d,h,e.id)));
        CHECK(descendants(f.d,h).size()==count);
        CHECK(!hasErrors(validate(f.d)));
    });
    test("P1.Identity.DuplicateRejected",[]{
        Fixture f;
        auto e=f.d.entities.begin()->second;
        CHECK(code(addEntity(f.d,e),"DUPLICATE_ID"));
        CHECK(!validId("name-as-id"));
    });
    test("P1.Identity.SourceCoordinatesNotAuthorable",[]{
        Fixture f;
        f.d.entities.at(f.entity(Kind::Point)).local.position.x=1;
        CHECK(code(validate(f.d),"GEOMETRY_AUTHORITY"));
    });
    test("P1.Session.NoOpDoesNotAdvanceRevision",[]{
        Fixture f;
        Session s(f.d);
        CHECK(!hasErrors(s.apply()));
        CHECK(s.committed().revision==0);
        CHECK(!s.undo());
    });
    test("P1.Session.RevertDoesNotUndoCommittedAction",[]{
        Fixture f;
        Session s(f.d);
        auto id=f.entity(Kind::City);
        s.draft().entities.at(id).name="Applied";
        CHECK(!hasErrors(s.apply()));
        s.draft().entities.at(id).name="Draft";
        s.revert();
        CHECK(s.committed().entities.at(id).name=="Applied");
        CHECK(s.undo());
    });
    test("P1.Session.DirtyDraftBlocksUndo",[]{
        Fixture f;
        Session s(f.d);
        auto id=f.entity(Kind::City);
        s.draft().entities.at(id).name="A";
        s.apply();
        s.draft().entities.at(id).name="B";
        CHECK(!s.undo());
    });
    test("P1.Session.StaleDraftAndABA",[]{
        Fixture f;
        Session s(f.d);
        s.draft().entities.at(f.entity(Kind::City)).name="draft";
        auto other=f.d;
        other.entities.at(f.entity(Kind::City)).name="external";
        s.replaceExternal(other);
        CHECK(code(s.apply(),"STALE_DRAFT"));
        CHECK(s.committed().entities.at(f.entity(Kind::City)).name=="external");
    });
    test("P1.Delete.DependencySafety",[]{
        Fixture f;
        auto before=contentFingerprint(f.d);
        CHECK(code(eraseEntity(f.d,f.entity(Kind::Highway)),"HAS_CHILDREN"));
        CHECK(code(eraseEntity(f.d,f.entity(Kind::Point),true),"SEGMENT_ENDPOINT"));
        CHECK(contentFingerprint(f.d)==before);
    });
    test("P1.Source.IdempotentScan",[]{
        Fixture f;
        auto before=contentFingerprint(f.d);
        auto diff=diffScan(f.d,f.source,f.entity(Kind::Highway));
        CHECK(diff.changes.empty());
        CHECK(!hasErrors(applyScan(f.d,f.source,diff,[&]{
            return f.next();
        })));
        CHECK(before==contentFingerprint(f.d));
    });
    test("P1.Source.ThreeIndependentSignedEnds",[]{
        Fixture f;
        auto s=f.source.segments.at("demo#S0");
        s.key="demo#S2";
        s.end.pointKey="demo#P0";
        s.start.signedTangentM=-23;
        s.end.signedTangentM=47;
        f.source.segments.emplace(s.key,s);
        auto ends=connectedEnds(f.source,"demo#P0");
        CHECK(ends.size()==3);
        CHECK(ends[0].signedTangentM==160);
        CHECK(ends[1].signedTangentM==-23);
        CHECK(ends[2].signedTangentM==47);
    });
    test("P1.Source.UnloadedIsNotDeletion",[]{
        Fixture f;
        auto count=f.d.entities.size();
        f.source.points.clear();
        f.source.segments.clear();
        f.source.completeScopes.clear();
        f.source.fingerprint="unloaded";
        auto diff=diffScan(f.d,f.source,f.entity(Kind::Highway));
        CHECK(!hasErrors(applyScan(f.d,f.source,diff,[&]{
            return f.next();
        })));
        CHECK(f.d.entities.size()==count);
        CHECK(f.d.entities.at(f.entity(Kind::Point)).availability==Availability::Unloaded);
    });
    test("P1.Source.LoadedScopeMissingIsRetained",[]{
        Fixture f;
        f.source.points.clear();
        f.source.segments.clear();
        f.source.fingerprint="missing";
        auto diff=diffScan(f.d,f.source,f.entity(Kind::Highway));
        CHECK(!hasErrors(applyScan(f.d,f.source,diff,[&]{
            return f.next();
        })));
        CHECK(f.d.entities.at(f.entity(Kind::Point)).availability==Availability::Missing);
        CHECK(f.d.segments.size()==2);
    });
    test("P1.Source.ExplicitDeletionRetainsSemantics",[]{
        Fixture f;
        auto id=f.entity(Kind::Point);
        auto key=f.d.entities.at(id).sourceKey;
        f.source.deletedKeys.insert(key);
        f.source.points.clear();
        f.source.segments.clear();
        f.source.fingerprint="deleted";
        auto diff=diffScan(f.d,f.source,f.entity(Kind::Highway));
        CHECK(!hasErrors(applyScan(f.d,f.source,diff,[&]{
            return f.next();
        })));
        CHECK(f.d.entities.at(id).availability==Availability::Deleted);
    });
    test("P1.Source.RescanPreservesPayload",[]{
        Fixture f;
        auto id=f.entity(Kind::Point);
        auto key=f.d.entities.at(id).sourceKey;
        f.d.entities.at(id).payload={
            "FutureFeature",8,true,"opaque-data"
        };
        f.source.points.at(key).frame.position.z=2;
        f.source.points.at(key).fingerprint="changed";
        f.source.fingerprint="rescan";
        auto diff=diffScan(f.d,f.source,f.entity(Kind::Highway));
        CHECK(!hasErrors(applyScan(f.d,f.source,diff,[&]{
            return f.next();
        })));
        CHECK(f.d.entities.at(id).payload.bytes=="opaque-data");
        CHECK(f.d.entities.at(id).acceptedSourceFingerprint=="changed");
    });
    test("P1.Source.StaleReviewRejected",[]{
        Fixture f;
        auto diff=diffScan(f.d,f.source,f.entity(Kind::Highway));
        f.d.entities.at(f.entity(Kind::Highway)).name="edited";
        CHECK(code(applyScan(f.d,f.source,diff,[&]{
            return f.next();
        }),"STALE_SCAN"));
    });
    test("P1.Source.WrongWorldRejected",[]{
        Fixture f;
        f.source.worldKey="another-map";
        CHECK(code(diffScan(f.d,f.source,f.entity(Kind::Highway)).issues,"WRONG_WORLD"));
        CHECK(code(f.compile().issues,"WRONG_WORLD"));
    });
    test("P1.Source.SplitRequiresExplicitRemap",[]{
        Fixture f;
        f.source.segments.at("demo#S0").end.pointKey="demo#P2";
        CHECK(code(diffScan(f.d,f.source,f.entity(Kind::Highway)).issues,"REMAP_REQUIRED"));
    });
    test("P1.Source.RebindPreservesGuid",[]{
        Fixture f;
        auto id=f.entity(Kind::Point);
        auto old=f.d.entities.at(id).sourceKey;
        auto p=f.source.points.at(old);
        p.key="demo#Renamed";
        p.fingerprint="renamed";
        f.source.points.emplace(p.key,p);
        CHECK(!hasErrors(rebindSource(f.d,id,old,p.key,f.source)));
        CHECK(f.d.entities.at(id).sourceKey==p.key);
    });
    test("P1.Payload.UnknownRoundTripAndCompileBlock",[]{
        Fixture f;
        auto id=f.entity(Kind::Point);
        f.d.entities.at(id).payload={
            "Unknown",9,true,std::string("\0\xff\x01",3)
        };
        Document out;
        CHECK(!hasErrors(decode(encode(f.d),out)));
        CHECK(out.entities.at(id).payload.bytes==f.d.entities.at(id).payload.bytes);
        CHECK(code(validate(out,true),"UNSUPPORTED_TYPE"));
    });
    test("P1.Codec.DeterministicRoundTrip",[]{
        Fixture f;
        auto b=encode(f.d);
        Document out;
        CHECK(!hasErrors(decode(b,out)));
        CHECK(encode(out)==b);
        CHECK(contentFingerprint(out)==contentFingerprint(f.d));
    });
    test("P1.Codec.FutureSchemaPreservesOutput",[]{
        Fixture f;
        auto b=encode(f.d);
        b[14]=2;
        Document out=f.d;
        auto hash=contentFingerprint(out);
        CHECK(code(decode(b,out),"UNSUPPORTED_SCHEMA"));
        CHECK(contentFingerprint(out)==hash);
    });
    test("P1.Codec.CorruptionAndTruncation",[]{
        Fixture f;
        auto original=encode(f.d);
        auto b=original;
        b.back()^=128;
        Document out=f.d;
        CHECK(hasErrors(decode(b,out)));
        for(std::size_t i=0;i<std::min<std::size_t>(original.size(),128);++i){
            b.assign(original.begin(),original.begin()+static_cast<std::ptrdiff_t>(i));
            CHECK(hasErrors(decode(b,out)));
        }
        CHECK(encode(out)==original);
    });
    test("P1.Codec.NaNRejected",[]{
        Fixture f;
        f.d.profiles.begin()->second.lanes[0].widthM=std::numeric_limits<double>::quiet_NaN();
        Document out;
        CHECK(hasErrors(decode(encode(f.d),out)));
    });
    test("P1.Transform.Full3DComposition",[]{
        Pose a{
            {
                10,20,30
            },{
                0,0,std::sqrt(0.5),std::sqrt(0.5)
            }
        };
        Pose local{
            {
                3,4,5
            },{
                std::sqrt(0.5),0,0,std::sqrt(0.5)
            }
        };
        auto w=compose(a,local);
        CHECK(length(w.position-Vec3{
            6,23,35
        })<1e-8);
        auto back=relative(a,w);
        CHECK(length(back.position-local.position)<1e-8);
        CHECK(std::abs(back.rotation.x-local.rotation.x)<1e-8);
    });
    test("P1.Storage.AtomicSaveReopen",[&]{
        Fixture f;
        auto path=pathUtf8(output/"atomic.nv8");
        fs::remove(path);
        auto r=saveAtomic(path,f.d,{
        });
        CHECK(r.saved);
        Document out;
        CHECK(!hasErrors(loadFile(path,out)));
        CHECK(encode(out)==encode(f.d));
    });
    test("P1.Storage.OptimisticConflict",[&]{
        Fixture f;
        auto path=pathUtf8(output/"conflict.nv8");
        fs::remove(path);
        CHECK(saveAtomic(path,f.d,{
        }).saved);
        auto hash=fileFingerprint(path);
        f.d.revision++;
        CHECK(saveAtomic(path,f.d,hash).saved);
        auto wrong=saveAtomic(path,f.d,hash);
        CHECK(!wrong.saved);
        CHECK(code(wrong.issues,"SAVE_CONFLICT"));
    });
    test("P1.Storage.FailurePreservesLastValid",[&]{
        Fixture f;
        auto path=pathUtf8(output/"fault.nv8");
        fs::remove(path);
        CHECK(saveAtomic(path,f.d,{
        }).saved);
        auto hash=fileFingerprint(path);
        f.d.revision++;
        for(auto fault:{
            SaveFault::BeforeWrite,SaveFault::BeforePromote
        }){
            auto r=saveAtomic(path,f.d,hash,fault);
            CHECK(!r.saved);
            CHECK(fileFingerprint(path)==hash);
        }
        CHECK(saveAtomic(path,f.d,hash).saved);
        CHECK(fileFingerprint(path+".bak")==hash);
    });
    test("P1.Storage.DraftRecoveryAndConflict",[&]{
        Fixture f;
        auto path=pathUtf8(output/"recover.draft");
        fs::remove(path);
        auto draft=f.d;
        draft.entities.at(f.entity(Kind::City)).name="Recovered";
        CHECK(saveDraftAtomic(path,f.d,draft,{
        }).saved);
        Document recovered;
        CHECK(!hasErrors(recoverDraft(path,f.d,recovered)));
        CHECK(recovered.entities.at(f.entity(Kind::City)).name=="Recovered");
        f.d.revision++;
        auto before=encode(recovered);
        CHECK(code(recoverDraft(path,f.d,recovered),"DRAFT_CONFLICT"));
        CHECK(encode(recovered)==before);
    });
    test("P1.Storage.LockBlocksSecondWriter",[&]{
        Fixture f;
        auto path=pathUtf8(output/"locked.nv8");
        fs::create_directory(path+".lock");
        auto r=saveAtomic(path,f.d,{
        });
        CHECK(code(r.issues,"SAVE_LOCK"));
        fs::remove(path+".lock");
    });
    test("P2.Catalog.FourLevelFallback",[]{
        Fixture f;
        catalogs(f);
        auto h=f.entity(Kind::Highway);
        for(auto k:{
            Kind::Highway,Kind::City,Kind::State,Kind::World
        }){
            auto r=resolveMesh(f.d,h,"Road.6",true,probe);
            CHECK(r.resolved);
            CHECK(r.scope==f.entity(k));
            f.d.catalogs.at(f.d.entities.at(f.entity(k)).catalog).entries.erase("Road.6");
        }
        CHECK(code(resolveMesh(f.d,h,"Road.6",true,probe).issues,"MISSING_MESH"));
    });
    test("P2.Catalog.PartialAndNullInheritance",[]{
        Fixture f;
        catalogs(f);
        auto h=f.entity(Kind::Highway);
        f.d.entities.at(h).catalog.clear();
        f.d.entities.at(f.entity(Kind::City)).catalog.clear();
        CHECK(resolveMesh(f.d,h,"Road.6",true,probe).scope==f.entity(Kind::State));
    });
    test("P2.Catalog.BrokenExplicitDoesNotFallThrough",[]{
        Fixture f;
        catalogs(f);
        auto h=f.entity(Kind::Highway);
        f.d.catalogs.at(f.d.entities.at(h).catalog).entries.at("Road.6").assetPath="/Game/Broken";
        auto r=resolveMesh(f.d,h,"Road.6",true,probe);
        CHECK(!r.resolved);
        CHECK(code(r.issues,"BROKEN_ASSET"));
        CHECK(r.scope==h);
    });
    test("P2.Catalog.OptionalDisabledIsNotInheritance",[]{
        Fixture f;
        catalogs(f);
        auto h=f.entity(Kind::Highway);
        f.d.catalogs.at(f.d.entities.at(h).catalog).entries.at("Road.6").assignment=Assignment::Disabled;
        auto r=resolveMesh(f.d,h,"Road.6",false,probe);
        CHECK(r.disabled);
        CHECK(!hasErrors(r.issues));
        CHECK(code(resolveMesh(f.d,h,"Road.6",true,probe).issues,"DISABLED_REQUIRED"));
    });
    test("P2.Catalog.NoPointLevelScope",[]{
        Fixture f;
        catalogs(f);
        auto p=f.entity(Kind::Point);
        f.d.entities.at(p).catalog=f.d.catalogs.begin()->first;
        CHECK(code(validate(f.d),"CATALOG_SCOPE"));
        CHECK(code(resolveMesh(f.d,p,"Road.6",true,probe).issues,"MESH_SCOPE"));
    });
    test("P2.Catalog.SharedOwnerDeterministic",[]{
        Fixture f;
        catalogs(f);
        SharedTopology t;
        t.id=f.next();
        t.meshOwnerHighway=f.entity(Kind::Highway);
        t.approachPoints={
            f.entity(Kind::Point)
        };
        f.d.topology.emplace(t.id,t);
        CHECK(resolveTopologyMesh(f.d,t.id,"Road.6",true,probe).scope==t.meshOwnerHighway);
    });
    test("P2.Catalog.PortCompatibility",[]{
        Fixture f;
        auto&a=f.d.profiles.at(f.profile(6));
        auto&b=f.d.profiles.at(f.profile(4));
        auto m=interfaceFor(a,b);
        CHECK(!hasErrors(compatible(m,a,b)));
        m.ports[1].forwardWidths[0]+=0.1;
        CHECK(code(compatible(m,a,b),"PORT_WIDTH"));
    });
    test("P2.Catalog.FrameAndMarkingValidation",[]{
        Fixture f;
        auto&p=f.d.profiles.at(f.profile(6));
        auto m=interfaceFor(p,p);
        m.pivot="Center";
        m.textureMarkings=false;
        CHECK(code(compatible(m,p,p),"MESH_FRAME"));
        CHECK(code(compatible(m,p,p),"MESH_SURFACE"));
    });
    test("P2.Adapter.Forward3To2AndReverse2To3",[]{
        Fixture f;
        auto&a=f.segment().adapters[0];
        auto&p=f.d.profiles.at(a.before);
        auto&q=f.d.profiles.at(a.after);
        CHECK(!hasErrors(validateAdapter(a,p,q,160)));
        int merges=0,branches=0;
        for(const auto&l:a.links){
            if(l.direction==Direction::Forward&&l.kind==LinkKind::Merge)merges++;
            if(l.direction==Direction::Reverse&&l.kind==LinkKind::Branch)branches++;
        }
        CHECK(merges==1);
        CHECK(branches==1);
    });
    test("P2.Adapter.MissingSuccessorBlocksCompile",[]{
        Fixture f;
        auto&a=f.segment().adapters[0];
        a.links.erase(a.links.begin()+2);
        CHECK(code(f.compile().issues,"MISSING_SUCCESSOR"));
    });
    test("P2.Adapter.WrongWayLinkRejected",[]{
        Fixture f;
        auto&a=f.segment().adapters[0];
        auto&after=f.d.profiles.at(a.after);
        for(const auto&l:after.lanes)if(l.direction==Direction::Reverse){
            a.links[0].toLane=l.id;
            break;
        }
        CHECK(code(f.compile().issues,"WRONG_WAY_LINK"));
    });
    test("P2.Adapter.UnapprovedCanSaveButCannotCompile",[]{
        Fixture f;
        f.segment().adapters[0].links[0].approved=false;
        CHECK(!hasErrors(validate(f.d)));
        CHECK(code(f.compile().issues,"LINK_UNAPPROVED"));
    });
    test("P2.Adapter.ShortTaperRejected",[]{
        Fixture f;
        auto&a=f.segment().adapters[0];
        a.endM=a.startM+2;
        CHECK(code(f.compile().issues,"TAPER_TOO_SHORT"));
    });
    test("P2.Adapter.Direct6To2RequiresStaging",[]{
        Fixture f;
        auto&a=f.segment().adapters[0];
        a.after=f.profile(2);
        a.links=proposeLinks(f.d.profiles.at(a.before),f.d.profiles.at(a.after),[&]{
            return f.next();
        });
        for(auto&l:a.links)l.approved=true;
        CHECK(code(validateAdapter(a,f.d.profiles.at(a.before),f.d.profiles.at(a.after),160),"STAGED_ADAPTER_REQUIRED"));
    });
    test("P2.Adapter.Staged6To4To2Compiles",[]{
        Fixture f;
        auto r=f.compile();
        if(!r.success)for(const auto&i:r.issues)std::cerr<<i.code<<":"<<i.message<<'\n';
        CHECK(r.success);
        CHECK(r.snapshot.roads.size()==2);
        CHECK(!r.snapshot.productionPublishable);
    });
    test("P2.Adapter.ProfileChangeWithoutAdapterRejected",[]{
        Fixture f;
        f.segment().adapters.clear();
        CHECK(code(f.compile().issues,"UNMAPPED_PROFILE_CHANGE"));
    });
    test("P2.Adapter.InteriorStationAndOutOfRange",[]{
        Fixture f;
        auto r=f.compile();
        CHECK(r.success);
        CHECK(f.segment().adapters[0].startM>0);
        f.segment().adapters[0].endM=200;
        CHECK(code(f.compile().issues,"ADAPTER_INTERVAL"));
    });
    test("P2.Profile.WidthEditPreservesLaneId",[]{
        Fixture f;
        auto&p=f.d.profiles.at(f.profile(4));
        auto id=p.lanes[0].id;
        p.lanes[0].widthM=3.7;
        CHECK(f.compile().success);
        CHECK(p.lanes[0].id==id);
    });
    test("P2.Profile.AsymmetricAndOneWay",[]{
        Fixture f;
        for(auto&kv:f.d.profiles){
            auto&p=kv.second;
            unsigned count=0;
            for(auto&l:p.lanes){
                l.direction=Direction::Forward;
                l.ordinal=count++;
            }
        }
        for(auto&kv:f.d.segments){
            kv.second.adapters.clear();
            kv.second.sections={
                {
                    0,f.profile(2)
                }
            };
        }
        CHECK(f.compile().success);
        auto&p=f.d.profiles.at(f.profile(6));
        p.lanes[4].direction=Direction::Reverse;
        p.lanes[4].ordinal=0;
        p.lanes[5].direction=Direction::Reverse;
        p.lanes[5].ordinal=1;
        CHECK(!hasErrors(validate(f.d)));
    });
    test("P2.Profile.VehicleClassIntersection",[]{
        Fixture f;
        auto&a=f.segment().adapters[0];
        auto&after=f.d.profiles.at(a.after);
        for(auto&l:after.lanes)l.access=Bus;
        for(auto&l:f.d.profiles.at(a.before).lanes)l.access=Car;
        CHECK(code(f.compile().issues,"LINK_ACCESS"));
    });
    test("P2.Profile.DrivingSideMirrorsPhysicalLane",[]{
        Fixture f;
        auto r=f.compile();
        CHECK(r.success);
        double x=0;
        for(const auto&c:r.snapshot.roads[0].corridors)if(c.direction==Direction::Forward){
            x=c.samples.front().center.y;
            break;
        }
        f.d.entities.at(f.entity(Kind::Highway)).drivingSide=DrivingSide::Left;
        auto l=f.compile();
        CHECK(l.success);
        double y=0;
        for(const auto&c:l.snapshot.roads[0].corridors)if(c.direction==Direction::Forward){
            y=c.samples.front().center.y;
            break;
        }
        CHECK(std::abs(x+y)<1e-7);
    });
    test("P2.Geometry.MeshAndLaneSameRevision",[]{
        Fixture f;
        auto r=f.compile();
        CHECK(r.success);
        CHECK(r.snapshot.documentFingerprint==contentFingerprint(f.d));
        CHECK(r.snapshot.sourceFingerprint==f.source.fingerprint);
        CHECK(r.snapshot.revision==f.d.revision);
    });
    test("P2.Geometry.DeterministicCompile",[]{
        Fixture f;
        auto a=f.compile(),b=f.compile();
        CHECK(a.success&&b.success);
        CHECK(a.snapshot.contentHash==b.snapshot.contentHash);
    });
    test("P2.Geometry.IndicesNormalsAndOpenEnds",[]{
        Fixture f;
        auto r=f.compile();
        CHECK(r.success);
        for(const auto&road:r.snapshot.roads)for(const auto&s:road.surfaces){
            CHECK(s.role!="EndCap");
            for(auto i:s.indices)CHECK(i<s.vertices.size());
            for(const auto&v:s.vertices)CHECK(finite(v.position)&&finite(v.normal));
        }
        auto&s=r.snapshot.roads[0].surfaces[0];
        auto n=cross(s.vertices[s.indices[1]].position-s.vertices[s.indices[0]].position,s.vertices[s.indices[2]].position-s.vertices[s.indices[0]].position);
        CHECK(n.z>0);
    });
    test("P2.Geometry.TextureMarkingsNotSeparateMesh",[]{
        Fixture f;
        auto r=f.compile();
        CHECK(r.success);
        auto&road=r.snapshot.roads[0];
        CHECK(road.markingRGBA.size()==road.textureWidth*road.textureHeight*4);
        for(const auto&s:road.surfaces)CHECK(s.role.find("Marker")==std::string::npos);
    });
    test("P2.Geometry.NarrowVehicleCorridorRejected",[]{
        Fixture f;
        CompileOptions o;
        o.requireAssets=false;
        o.vehicleWidthM=5;
        auto r=compilePreview(f.d,f.source,{
        },o);
        CHECK(!r.success);
        CHECK(code(r.issues,"VEHICLE_CORRIDOR"));
    });
    test("P2.Geometry.BankedFramePreserved",[]{
        Fixture f;
        for(auto&kv:f.source.segments){
            for(auto&x:kv.second.samples){
                x.up={
                    0,-std::sin(0.2),std::cos(0.2)
                };
            }
            kv.second.fingerprint+="banked";
        }
        for(auto&kv:f.d.segments)kv.second.acceptedSourceFingerprint=f.source.segments.at(kv.second.sourceKey).fingerprint;
        auto r=f.compile();
        CHECK(r.success);
        auto&v=r.snapshot.roads[0].surfaces[0].vertices;
        CHECK(std::abs(v[0].position.z-v[1].position.z)>1);
    });
    test("P2.Geometry.BadFrameAndGradeBlocked",[]{
        Fixture f;
        f.source.segments.begin()->second.samples[2].up={
            1,0,0
        };
        CHECK(code(f.compile().issues,"SOURCE_FRAME"));
    });
    test("P2.Build.MissingAssetsBlockStrictMode",[]{
        Fixture f;
        auto r=compilePreview(f.d,f.source,probe,{
        });
        CHECK(!r.success);
        CHECK(code(r.issues,"MISSING_MESH"));
    });
    test("P2.Build.CancelDoesNotReturnPartialOutput",[]{
        Fixture f;
        CompileOptions o;
        o.requireAssets=false;
        o.canceled=[](){
            return true;
        };
        auto r=compilePreview(f.d,f.source,{
        },o);
        CHECK(!r.success);
        CHECK(code(r.issues,"CANCELED"));
        CHECK(r.snapshot.roads.empty());
    });
    test("P2.Build.StaleSourceBlocked",[]{
        Fixture f;
        f.source.segments.begin()->second.fingerprint="changed";
        CHECK(code(f.compile().issues,"SOURCE_STALE"));
    });
    test("P2.Build.UnavailableSourceBlocked",[]{
        Fixture f;
        f.source.segments.clear();
        CHECK(code(f.compile().issues,"SOURCE_UNAVAILABLE"));
    });
    test("P2.Build.SourceNotMutated",[]{
        Fixture f;
        auto before=f.source.segments.at("demo#S0").samples[80].position;
        auto tangent=f.source.segments.at("demo#S0").end.signedTangentM;
        CHECK(f.compile().success);
        CHECK(length(f.source.segments.at("demo#S0").samples[80].position-before)==0);
        CHECK(f.source.segments.at("demo#S0").end.signedTangentM==tangent);
    });
    test("P2.History.CatalogEditUndoRoundTrip",[]{
        Fixture f;
        catalogs(f);
        Session s(f.d);
        auto h=f.entity(Kind::Highway);
        auto c=f.d.entities.at(h).catalog;
        s.draft().catalogs.at(c).entries["Road.6"].assignment=Assignment::Inherit;
        CHECK(!hasErrors(s.apply()));
        CHECK(resolveMesh(s.committed(),h,"Road.6",true,probe).scope==f.entity(Kind::City));
        CHECK(s.undo());
        CHECK(resolveMesh(s.committed(),h,"Road.6",true,probe).scope==h);
        Document out;
        CHECK(!hasErrors(decode(encode(s.committed()),out)));
        CHECK(resolveMesh(out,h,"Road.6",true,probe).scope==h);
    });
    test("P1.Codec.EmptyAndShortInputsDoNotReplaceDocument",[]{
        Fixture f;
        for(unsigned n=0;n<32;++n){
            auto d=f.d;
            CHECK(hasErrors(decode(Bytes(n,0),d)));
            CHECK(contentFingerprint(d)==contentFingerprint(f.d));
        }
    });
    test("P1.Codec.PayloadBoundPreventsUnreadableSave",[]{
        Fixture f;
        f.d.entities.begin()->second.payload.bytes=std::string(4*1024*1024+1,'x');
        CHECK(code(validate(f.d),"PAYLOAD_LIMIT"));
    });
    test("P1.Codec.CorruptFileMutationSweep",[]{
        Fixture f;
        const auto b=encode(f.d);
        for(std::size_t i=0;i<b.size();i+=13){
            auto broken=b;
            broken[i]^=0x71;
            auto out=f.d;
            CHECK(hasErrors(decode(broken,out)));
            CHECK(contentFingerprint(out)==contentFingerprint(f.d));
        }
    });
    test("P1.Source.InvalidAvailabilityRejected",[]{
        Fixture f;
        f.segment().availability=static_cast<Availability>(99);
        CHECK(code(validate(f.d),"ENUM"));
    });
    test("P1.Topology.MapKeyMustMatchIdentity",[]{
        Fixture f;
        SharedTopology t;
        t.id=f.next();
        t.meshOwnerHighway=f.entity(Kind::Highway);
        f.d.topology.emplace(f.next(),t);
        CHECK(code(validate(f.d),"KEY_ID"));
    });
    test("P1.Storage.UnicodeRoundTrip",[&]{
        Fixture f;
        const auto path=output/fs::path(u8"world-नेपाल.nv8");
        std::string name;
#if __cplusplus >= 202002L
        auto utf=path.u8string();
        name.assign(reinterpret_cast<const char*>(utf.data()),utf.size());
#else
        name=path.u8string();
#endif
        CHECK(saveAtomic(name,f.d,fileFingerprint(name)).saved);
        Document d;
        CHECK(!hasErrors(loadFile(name,d)));
        CHECK(contentFingerprint(d)==contentFingerprint(f.d));
    });
    test("P2.Mesh.NonfiniteAssetUnitsRejected",[]{
        Fixture f;
        const auto&p=f.d.profiles.at(f.profile(6));
        auto m=interfaceFor(p,p);
        m.metersPerUnit=std::numeric_limits<double>::quiet_NaN();
        CHECK(code(compatible(m,p,p),"MESH_FRAME"));
    });
    test("P2.Mesh.EveryGenericSlotUseValidated",[]{
        Fixture f;
        // Remove the second segment, then author a width-only six-lane transition.
        auto second=f.segment(1).id;
        f.d.segments.erase(second);
        auto a=f.profile(6);
        Profile p=f.d.profiles.at(a);
        p.id=f.next();
        p.name="Wide 6";
        for(auto&l:p.lanes){
            l.id=f.next();
            l.widthM=4;
        }
        auto b=p.id;
        f.d.profiles.emplace(b,p);
        auto&s=f.segment();
        s.sections={
            {
                0,a
            },{
                120,b
            }
        };
        auto&ad=s.adapters.front();
        ad.before=a;
        ad.after=b;
        ad.links=proposeLinks(f.d.profiles.at(a),p,[&]{
            return f.next();
        });
        for(auto&l:ad.links)l.approved=true;
        Catalog c;
        c.id=f.next();
        c.name="Ambiguous generic";
        MeshEntry e;
        e.assignment=Assignment::Assigned;
        e.assetPath="/Game/Test";
        e.interface=interfaceFor(p,p);
        c.entries["Road.6"]=e;
        e.interface=interfaceFor(f.d.profiles.at(a),p);
        c.entries["Adapter.6-6"]=e;
        f.d.entities.at(f.entity(Kind::World)).catalog=c.id;
        f.d.catalogs.emplace(c.id,c);
        auto r=compilePreview(f.d,f.source,probe,{
        });
        CHECK(!r.success);
        CHECK(code(r.issues,"PORT_WIDTH"));
        auto&catalog=f.d.catalogs.at(c.id);
        e.interface=interfaceFor(f.d.profiles.at(a),f.d.profiles.at(a));
        catalog.entries["Road.Profile."+a]=e;
        e.interface=interfaceFor(p,p);
        catalog.entries["Road.Profile."+b]=e;
        auto fixed=compilePreview(f.d,f.source,probe,{
        });
        CHECK(fixed.success);
    });
    test("P2.Mesh.ExactBrokenVariantDoesNotHideBehindGeneric",[]{
        Fixture f;
        Catalog c;
        c.id=f.next();
        c.name="Root";
        MeshEntry e;
        e.assignment=Assignment::Assigned;
        e.assetPath="/Game/Broken";
        c.entries["Road.Profile."+f.profile(6)]=e;
        e.assetPath="/Game/Valid";
        e.interface=interfaceFor(f.d.profiles.at(f.profile(6)),f.d.profiles.at(f.profile(6)));
        c.entries["Road.6"]=e;
        f.d.entities.at(f.entity(Kind::World)).catalog=c.id;
        f.d.catalogs.emplace(c.id,c);
        auto r=compilePreview(f.d,f.source,probe,{
        });
        CHECK(!r.success);
        CHECK(code(r.issues,"BROKEN_ASSET"));
    });
    test("P2.Mesh.DisabledExactVariantBlocksRequiredRoad",[]{
        Fixture f;
        Catalog c;
        c.id=f.next();
        c.name="Root";
        MeshEntry e;
        e.assignment=Assignment::Disabled;
        c.entries["Road.Profile."+f.profile(6)]=e;
        f.d.entities.at(f.entity(Kind::World)).catalog=c.id;
        f.d.catalogs.emplace(c.id,c);
        CHECK(code(compilePreview(f.d,f.source,probe,{
        }).issues,"DISABLED_REQUIRED"));
    });
    test("P2.Geometry.ZeroWidthEdgesContainNoDegenerateTriangles",[]{
        Fixture f;
        for(auto&p:f.d.profiles){
            p.second.sidewalkLeftM=0;
            p.second.sidewalkRightM=0;
            p.second.borderLeftM=0;
            p.second.borderRightM=0;
            p.second.sidewalkHeightM=0;
        }
        auto r=f.compile();
        CHECK(r.success);
        for(const auto&road:r.snapshot.roads)for(const auto&s:road.surfaces){
            if(s.role!="Road")CHECK(s.indices.empty());
            for(std::size_t i=0;i<s.indices.size();i+=3){
                auto a=s.vertices[s.indices[i]].position,b=s.vertices[s.indices[i+1]].position,c=s.vertices[s.indices[i+2]].position;
                CHECK(length(cross(b-a,c-a))>1e-10);
            }
        }
    });
    test("P2.Geometry.OrdinaryJoinRejectsHiddenCountChange",[]{
        Fixture f;
        f.segment(1).adapters.clear();
        f.segment(1).sections={
            {
                0,f.profile(6)
            }
        };
        auto r=f.compile();
        CHECK(!r.success);
        CHECK(code(r.issues,"JOIN_PROFILE_MISMATCH"));
    });
    test("P2.Geometry.OrdinaryJoinAcceptsMatchingProfiles",[]{
        Fixture f;
        auto r=f.compile();
        CHECK(r.success);
        const auto&a=r.snapshot.roads[0].surfaces[0].vertices;
        const auto&b=r.snapshot.roads[1].surfaces[0].vertices;
        CHECK(length(a[a.size()-2].position-b[0].position)<1e-9);
        CHECK(length(a.back().position-b[1].position)<1e-9);
    });
    test("P2.Build.LateCancelClearsPartialSnapshot",[]{
        Fixture f;
        int polls=0;
        CompileOptions o;
        o.requireAssets=false;
        o.canceled=[&]{
            return ++polls>=2;
        };
        auto r=compilePreview(f.d,f.source,{
        },o);
        CHECK(!r.success);
        CHECK(code(r.issues,"CANCELED"));
        CHECK(r.snapshot.roads.empty());
    });
    test("P2.Build.FinalCancelKeepsNonpublishableState",[]{
        Fixture f;
        int polls=0;
        CompileOptions o;
        o.requireAssets=false;
        o.canceled=[&]{
            return ++polls>=3;
        };
        auto r=compilePreview(f.d,f.source,{
        },o);
        CHECK(!r.success);
        CHECK(r.snapshot.roads.empty());
        CHECK(!r.snapshot.productionPublishable);
    });
    test("P2.Build.InvalidSamplingBudgetRejected",[]{
        Fixture f;
        CompileOptions o;
        o.requireAssets=false;
        o.sampleSpacingM=0;
        CHECK(code(compilePreview(f.d,f.source,{
        },o).issues,"COMPILE_OPTIONS"));
    });
    test("P2.Geometry.TriangleWindingMatchesDeclaredNormal",[]{
        Fixture f;
        auto r=f.compile();
        CHECK(r.success);
        for(const auto&road:r.snapshot.roads)for(const auto&s:road.surfaces)for(std::size_t i=0;i<s.indices.size();i+=3){
            const auto&a=s.vertices[s.indices[i]],&b=s.vertices[s.indices[i+1]],&c=s.vertices[s.indices[i+2]];
            CHECK(dot(cross(b.position-a.position,c.position-a.position),a.normal)>0);
        }
    });
    test("P2.Profile.SpeedAndAccessMustBeFiniteAndNonzero",[]{
        Fixture f;
        auto&l=f.d.profiles.begin()->second.lanes[0];
        l.speedMps=0;
        CHECK(code(validate(f.d),"LANE_VALUE"));
        l.speedMps=10;
        l.access=0;
        CHECK(code(validate(f.d),"LANE_VALUE"));
    });
    test("P2.Mesh.HighwayGenericPrecedesRootExactVariant",[]{
        Fixture f;
        catalogs(f);
        const auto h=f.entity(Kind::Highway),root=f.entity(Kind::World);
        auto c=f.d.entities.at(root).catalog;
        auto e=f.d.catalogs.at(c).entries.at("Road.6");
        e.assetPath="/Game/RootExact";
        f.d.catalogs.at(c).entries["Road.Profile."+f.profile(6)]=e;
        auto r=resolveMeshVariant(f.d,h,"Road.Profile."+f.profile(6),"Road.6",true,probe);
        CHECK(r.resolved);
        CHECK(r.scope==h);
        CHECK(r.slot=="Road.6");
    });
    int failed=0;
    for(const auto&r:results)if(!r.error.empty())failed++;
    std::ofstream json(output/"results.json");
    json<<"{\n  \"suite\": \"actual shared C++ domain; not Unreal engine QA\",\n  \"tests\": "<<results.size()<<", \"failed\": "<<failed<<", \"assertions\": "<<assertions<<",\n  \"results\": [\n";
    for(std::size_t i=0;i<results.size();++i){
        const auto&r=results[i];
        json<<"    {\"name\":\""<<escape(r.name)<<"\",\"status\":\""<<(r.error.empty()?"PASS":"FAIL")<<"\",\"assertions\":"<<r.checks<<",\"error\":\""<<escape(r.error)<<"\"}"<<(i+1<results.size()?",":"")<<'\n';
    }
    json<<"  ]\n}\n";
    std::cout<<"TOTAL "<<results.size()<<" | FAILED "<<failed<<" | ASSERTIONS "<<assertions<<'\n';
    return failed?1:0;
}
