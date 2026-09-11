#include "Domain/NexusDomain.h"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <limits>
#include <sstream>
namespace nexus {
    namespace {
        Issue error(std::string code,Id id,std::string text){
            return {
                std::move(code),std::move(id),std::move(text),true
            };
        }
        bool finitePositive(double v){
            return std::isfinite(v)&&v>0;
        }
        bool finiteNonnegative(double v){
            return std::isfinite(v)&&v>=0;
        }
        bool parentAllowed(Kind child,Kind parent){
            return static_cast<int>(child)==static_cast<int>(parent)+1;
        }
        bool validPose(const Pose& p){
            const auto&q=p.rotation;
            const double n=q.x*q.x+q.y*q.y+q.z*q.z+q.w*q.w;
            return finite(p.position)&&std::isfinite(n)&&std::abs(n-1)<1e-5;
        }
        std::vector<const Lane*> lanes(const Profile& p,Direction d){
            std::vector<const Lane*> r;
            for(const auto&l:p.lanes)if(l.direction==d)r.push_back(&l);
            std::sort(r.begin(),r.end(),[](const Lane*a,const Lane*b){
                return a->ordinal<b->ordinal;
            });
            return r;
        }
        const Lane* lane(const Profile&p,const Id&id){
            for(const auto&l:p.lanes)if(l.id==id)return &l;
            return nullptr;
        }
    }
    bool hasErrors(const Issues& issues){
        return std::any_of(issues.begin(),issues.end(),[](const Issue&i){
            return i.blocking;
        });
    }
    bool validId(const Id& id){
        return id.size()==32&&id.find_first_not_of("0123456789abcdef")==std::string::npos&&id!=std::string(32,'0');
    }
    Id fixtureId(std::uint64_t n){
        std::ostringstream s;
        s<<std::hex<<std::setfill('0')<<std::setw(32)<<n;
        return s.str();
    }
    std::string fingerprint(const std::string& bytes){
        std::uint64_t h=14695981039346656037ull;
        for(unsigned char c:bytes){
            h^=c;
            h*=1099511628211ull;
        }
        std::ostringstream s;
        s<<std::hex<<std::setfill('0')<<std::setw(16)<<h;
        return s.str();
    }
    double dot(Vec3 a,Vec3 b){
        return a.x*b.x+a.y*b.y+a.z*b.z;
    }
    Vec3 cross(Vec3 a,Vec3 b){
        return {
            a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x
        };
    }
    double length(Vec3 a){
        return std::sqrt(dot(a,a));
    }
    Vec3 unit(Vec3 a){
        const double n=length(a);
        return n>1e-12?a*(1/n):Vec3{
        };
    }
    bool finite(Vec3 a){
        return std::isfinite(a.x)&&std::isfinite(a.y)&&std::isfinite(a.z);
    }
    Quat normalized(Quat q){
        const double n=std::sqrt(q.x*q.x+q.y*q.y+q.z*q.z+q.w*q.w);
        if(n<1e-12||!std::isfinite(n))return {
        };
        return {
            q.x/n,q.y/n,q.z/n,q.w/n
        };
    }
    Quat multiply(Quat a,Quat b){
        return normalized({
            a.w*b.x+a.x*b.w+a.y*b.z-a.z*b.y,a.w*b.y-a.x*b.z+a.y*b.w+a.z*b.x,a.w*b.z+a.x*b.y-a.y*b.x+a.z*b.w,a.w*b.w-a.x*b.x-a.y*b.y-a.z*b.z
        });
    }
    Vec3 rotate(Quat q,Vec3 v){
        q=normalized(q);
        Vec3 u{
            q.x,q.y,q.z
        };
        return v+cross(u,v)*(2*q.w)+cross(u,cross(u,v))*2;
    }
    Pose compose(Pose f,Pose p){
        return {
            f.position+rotate(f.rotation,p.position),multiply(f.rotation,p.rotation)
        };
    }
    Pose relative(Pose f,Pose w){
        const auto q=normalized(f.rotation);
        Quat inv{
            -q.x,-q.y,-q.z,q.w
        };
        return {
            rotate(inv,w.position-f.position),multiply(inv,w.rotation)
        };
    }
    Registry defaultRegistry(){
        return {
            {
                "Parking",{
                    1,false
                }
            },{
                "BusStop",{
                    1,false
                }
            },{
                "Hospital",{
                    1,false
                }
            },{
                "CityPark",{
                    1,false
                }
            },{
                "PetrolPump",{
                    1,false
                }
            },{
                "Crosswalk",{
                    1,false
                }
            },{
                "TrafficSignal",{
                    1,false
                }
            }
        };
    }
    Issues validate(const Document&d,bool compile,const Registry& registry){
        Issues r;
        if(d.entities.size()>100000||d.segments.size()>100000||d.profiles.size()>100000||d.catalogs.size()>100000||d.topology.size()>100000)r.push_back(error("RECORD_LIMIT",d.id,"Document collection exceeds the codec safety limit."));
        if(d.sourceWorldKey.size()>8192)r.push_back(error("SOURCE_KEY_LIMIT",d.id,"Source map key is too long."));
        if(d.schema!=SchemaVersion)r.push_back(error("SCHEMA",d.id,"Unsupported schema; original bytes must be retained."));
        if(!validId(d.id))r.push_back(error("DOCUMENT_ID",d.id,"Document GUID is invalid."));
        std::set<Id> global;
        auto claim=[&](const Id&id){
            if(!validId(id)||!global.insert(id).second)r.push_back(error("DUPLICATE_ID",id,"GUID is invalid or duplicated across logical records."));
        };
        std::size_t roots=0;
        for(const auto&kv:d.entities){
            const auto&e=kv.second;
            claim(e.id);
            if(kv.first!=e.id)r.push_back(error("KEY_ID",e.id,"Map key must equal record ID."));
            if(e.name.empty()||e.name.size()>1024)r.push_back(error("NAME",e.id,"Display name must contain 1–1024 bytes."));
            if(static_cast<unsigned>(e.kind)>5||static_cast<unsigned>(e.availability)>4||static_cast<unsigned>(e.drivingSide)>1)r.push_back(error("ENUM",e.id,"Invalid enum value."));
            if(e.kind==Kind::World){
                ++roots;
                if(!e.parent.empty())r.push_back(error("WORLD_PARENT",e.id,"World/Root must have no parent."));
            } else{
                auto p=d.entities.find(e.parent);
                if(p==d.entities.end()||!parentAllowed(e.kind,p->second.kind))r.push_back(error("OWNERSHIP",e.id,"Parent must be the exact preceding hierarchy level."));
            }
            if(!e.catalog.empty()&&(e.kind>Kind::Highway||!d.catalogs.count(e.catalog)))r.push_back(error("CATALOG_SCOPE",e.id,"Catalogs are allowed only at World, State, City and Highway; referenced catalog must exist."));
            if(!e.defaultProfile.empty()&&(e.kind!=Kind::Highway||!d.profiles.count(e.defaultProfile)))r.push_back(error("PROFILE_REF",e.id,"Default lane profile belongs to a highway and must exist."));
            if(!e.sourceKey.empty()&&e.kind!=Kind::Point)r.push_back(error("POINT_BINDING",e.id,"Only points bind a control-point key."));
            if(!validPose(e.local))r.push_back(error("POSE",e.id,"Transform contains nonfinite values or an unnormalized quaternion."));
            if(e.kind!=Kind::Place&&(length(e.local.position)>1e-10||std::abs(e.local.rotation.w-1)>1e-10))r.push_back(error("GEOMETRY_AUTHORITY",e.id,"Only places store authored local offsets; point geometry comes from Landscape."));
            if(e.kind==Kind::Place&&!e.anchorSegment.empty()){
                auto s=d.segments.find(e.anchorSegment);
                if(s==d.segments.end()||(e.anchorAtEnd?s->second.endPoint:s->second.startPoint)!=e.parent)r.push_back(error("ANCHOR",e.id,"Anchor segment-end must be incident to the owning point."));
            }
            if(e.payload.bytes.size()>4u*1024u*1024u||e.payload.type.size()>512||e.sourceKey.size()>8192)r.push_back(error("PAYLOAD_LIMIT",e.id,"Payload/source key exceeds the documented storage bound."));
            if(e.kind==Kind::Place&&e.payload.type.empty())r.push_back(error("PLACE_TYPE",e.id,"Place requires a versioned type."));
            if(compile&&!e.payload.type.empty()&&e.payload.enabled){
                auto t=registry.find(e.payload.type);
                if(t==registry.end()||t->second.version!=e.payload.version||!t->second.compilerSupported)r.push_back(error("UNSUPPORTED_TYPE",e.id,"Payload is preserved but has no supported compiler handler: "+e.payload.type));
            }
        }
        if(roots!=1)r.push_back(error("WORLD_COUNT",d.id,"Exactly one World/Root record is required."));
        for(const auto&kv:d.profiles){
            const auto&p=kv.second;
            claim(p.id);
            if(kv.first!=p.id)r.push_back(error("KEY_ID",p.id,"Profile map key mismatch."));
            if(p.lanes.empty()||p.lanes.size()>32)r.push_back(error("LANE_COUNT",p.id,"A profile needs 1–32 lanes."));
            for(double v:{
                p.medianM,p.sidewalkLeftM,p.sidewalkRightM,p.borderLeftM,p.borderRightM,p.sidewalkHeightM
            })if(!finiteNonnegative(v))r.push_back(error("PROFILE_DIMENSION",p.id,"Dimensions must be finite and nonnegative."));
            for(const auto&l:p.lanes){
                claim(l.id);
                if(static_cast<unsigned>(l.direction)>1||!finitePositive(l.widthM)||!finitePositive(l.speedMps)||l.access==0||(l.access&~15u)!=0)r.push_back(error("LANE_VALUE",l.id,"Lane direction, width, speed or access mask is invalid."));
            }
            for(Direction dir:{
                Direction::Forward,Direction::Reverse
            }){
                auto ls=lanes(p,dir);
                for(std::size_t i=0;i<ls.size();++i)if(ls[i]->ordinal!=i)r.push_back(error("LANE_ORDER",ls[i]->id,"Lane ordinals must be contiguous from median outward for each direction."));
            }
        }
        std::set<std::pair<Id,std::string>> pointBindings,segmentBindings;
        for(const auto&kv:d.entities){
            const auto&e=kv.second;
            if(e.kind==Kind::Point&&!e.sourceKey.empty()&&!pointBindings.insert({
                e.parent,e.sourceKey
            }).second)r.push_back(error("DUPLICATE_BINDING",e.id,"Same source point is bound twice in one highway."));
        }
        for(const auto&kv:d.segments){
            const auto&s=kv.second;
            claim(s.id);
            if(static_cast<unsigned>(s.availability)>4)r.push_back(error("ENUM",s.id,"Invalid source availability."));
            if(kv.first!=s.id)r.push_back(error("KEY_ID",s.id,"Segment map key mismatch."));
            auto h=d.entities.find(s.highway),a=d.entities.find(s.startPoint),b=d.entities.find(s.endPoint);
            if(h==d.entities.end()||h->second.kind!=Kind::Highway||a==d.entities.end()||b==d.entities.end()||a->second.kind!=Kind::Point||b->second.kind!=Kind::Point||a->second.parent!=s.highway||b->second.parent!=s.highway)r.push_back(error("SEGMENT_ENDPOINT",s.id,"Segment endpoints must be points owned by its highway."));
            if(!s.sourceKey.empty()&&!segmentBindings.insert({
                s.highway,s.sourceKey
            }).second)r.push_back(error("DUPLICATE_BINDING",s.id,"Source segment is duplicated in one highway."));
            double previous=-1;
            for(const auto&section:s.sections){
                if(!finiteNonnegative(section.startM)||section.startM<=previous||!d.profiles.count(section.profile))r.push_back(error("SECTION",s.id,"Section stations must increase and reference existing profiles."));
                previous=section.startM;
            }
            if(!s.sections.empty()&&std::abs(s.sections.front().startM)>1e-9)r.push_back(error("SECTION_START",s.id,"First section must start at zero."));
            double lastEnd=-1;
            for(const auto&ad:s.adapters){
                claim(ad.id);
                if(ad.startM<lastEnd)r.push_back(error("ADAPTER_OVERLAP",ad.id,"Adapters must be sorted and nonoverlapping."));
                lastEnd=ad.endM;
                auto p=d.profiles.find(ad.before),q=d.profiles.find(ad.after);
                if(p==d.profiles.end()||q==d.profiles.end())r.push_back(error("ADAPTER_PROFILE",ad.id,"Adapter references a missing profile."));
                else {
                    auto ar=validateAdapter(ad,p->second,q->second,std::numeric_limits<double>::max());
                    for(auto issue:ar) {
                        if(!compile && (issue.code=="LINK_UNAPPROVED"||issue.code=="MISSING_SUCCESSOR"||issue.code=="MISSING_PREDECESSOR")) issue.blocking=false;
                        r.push_back(std::move(issue));
                    }
                }
                for(const auto&link:ad.links)claim(link.id);
            }
        }
        for(const auto&kv:d.catalogs){
            const auto&c=kv.second;
            claim(c.id);
            if(kv.first!=c.id)r.push_back(error("KEY_ID",c.id,"Catalog map key mismatch."));
            for(const auto&e:c.entries){
                if(e.first.empty()||static_cast<unsigned>(e.second.assignment)>2)r.push_back(error("MESH_SLOT",c.id,"Slot key or assignment is invalid."));
            }
        }
        for(const auto&kv:d.topology){
            const auto&t=kv.second;
            claim(t.id);
            if(kv.first!=t.id)r.push_back(error("KEY_ID",t.id,"Shared topology map key mismatch."));
            auto h=d.entities.find(t.meshOwnerHighway);
            if(h==d.entities.end()||h->second.kind!=Kind::Highway)r.push_back(error("MESH_OWNER",t.id,"Shared topology needs an explicit highway mesh owner."));
            std::set<Id> seen;
            for(const auto&p:t.approachPoints){
                auto it=d.entities.find(p);
                if(it==d.entities.end()||it->second.kind!=Kind::Point||!seen.insert(p).second)r.push_back(error("APPROACH",t.id,"Approach must reference a distinct point."));
            }
        }
        return r;
    }
    Issues addEntity(Document&d,Entity e){
        if(d.entities.count(e.id))return {
            error("DUPLICATE_ID",e.id,"Entity already exists.")
        };
        Document next=d;
        next.entities.emplace(e.id,std::move(e));
        auto r=validate(next);
        if(!hasErrors(r))d=std::move(next);
        return r;
    }
    Issues renameEntity(Document&d,const Id&id,const std::string&name){
        auto it=d.entities.find(id);
        if(it==d.entities.end())return {
            error("NOT_FOUND",id,"Entity does not exist.")
        };
        if(name.empty()||name.size()>1024)return {
            error("NAME",id,"Name must contain 1–1024 bytes.")
        };
        it->second.name=name;
        return {
        };
    }
    Issues reparentEntity(Document&d,const Id&id,const Id&parent){
        auto it=d.entities.find(id);
        if(it==d.entities.end())return {
            error("NOT_FOUND",id,"Entity does not exist.")
        };
        Document next=d;
        next.entities.at(id).parent=parent;
        auto r=validate(next);
        if(!hasErrors(r))d=std::move(next);
        return r;
    }
    std::vector<Id> descendants(const Document&d,const Id&id){
        std::vector<Id> out;
        std::set<Id> seen{
            id
        };
        std::vector<Id> todo{
            id
        };
        while(!todo.empty()){
            Id p=todo.back();
            todo.pop_back();
            for(const auto&kv:d.entities)if(kv.second.parent==p&&seen.insert(kv.first).second){
                out.push_back(kv.first);
                todo.push_back(kv.first);
            }
        }
        return out;
    }
    Issues eraseEntity(Document&d,const Id&id,bool all){
        auto it=d.entities.find(id);
        if(it==d.entities.end())return {
            error("NOT_FOUND",id,"Entity does not exist.")
        };
        if(it->second.kind==Kind::World)return {
            error("ROOT_DELETE",id,"Cannot delete the World/Root.")
        };
        auto kids=descendants(d,id);
        if(!all&&!kids.empty())return {
            error("HAS_CHILDREN",id,"Review dependent descendants before deleting.")
        };
        Document next=d;
        next.entities.erase(id);
        for(const auto&x:kids)next.entities.erase(x);
        auto r=validate(next);
        if(!hasErrors(r))d=std::move(next);
        return r;
    }
    std::set<Id> affectedHighways(const Document&d,const Id&scope){
        std::set<Id> out;
        auto xs=descendants(d,scope);
        xs.push_back(scope);
        for(const auto&id:xs){
            auto i=d.entities.find(id);
            if(i!=d.entities.end()&&i->second.kind==Kind::Highway)out.insert(id);
        }
        return out;
    }
    ResolvedMesh resolveMesh(const Document&d,const Id&h,const std::string&slot,bool required,const AssetProbe&probe){
        ResolvedMesh r;
        r.slot=slot;
        auto it=d.entities.find(h);
        if(it==d.entities.end()||it->second.kind!=Kind::Highway){
            r.issues.push_back(error("MESH_SCOPE",h,"Resolution must start from a highway."));
            return r;
        }
        std::set<Id> seen;
        while(it!=d.entities.end()){
            const auto&e=it->second;
            if(!seen.insert(e.id).second){
                r.issues.push_back(error("CYCLE",e.id,"Ownership cycle."));
                return r;
            }
            if(!e.catalog.empty()){
                auto c=d.catalogs.find(e.catalog);
                if(c==d.catalogs.end()){
                    r.issues.push_back(error("BROKEN_CATALOG",e.id,"Explicit catalog reference is broken."));
                    return r;
                }
                auto entry=c->second.entries.find(slot);
                if(entry!=c->second.entries.end()&&entry->second.assignment!=Assignment::Inherit){
                    r.scope=e.id;
                    r.catalog=c->first;
                    r.entry=entry->second;
                    if(r.entry.assignment==Assignment::Disabled){
                        r.disabled=true;
                        if(required)r.issues.push_back(error("DISABLED_REQUIRED",e.id,"Required slot is disabled: "+slot));
                        return r;
                    }
                    if(r.entry.assetPath.empty()||!probe||!probe(r.entry.assetPath)){
                        r.issues.push_back(error("BROKEN_ASSET",e.id,"Explicit mesh cannot be loaded; inheritance is not a repair: "+slot+" / "+r.entry.assetPath));
                        return r;
                    }
                    r.resolved=true;
                    return r;
                }
            }
            it=d.entities.find(e.parent);
        }
        if(required)r.issues.push_back(error("MISSING_MESH",h,"No assignment in Highway → City → State → Root for "+slot));
        return r;
    }
    ResolvedMesh resolveMeshVariant(const Document&d,const Id&highway,const std::string&exact,const std::string&generic,bool required,const AssetProbe&probe){
        auto i=d.entities.find(highway);
        std::set<Id> visited;
        if(i==d.entities.end()||i->second.kind!=Kind::Highway)return resolveMesh(d,highway,exact,required,probe);
        while(i!=d.entities.end()&&visited.insert(i->first).second){
            if(!i->second.catalog.empty()){
                auto c=d.catalogs.find(i->second.catalog);
                if(c==d.catalogs.end())return resolveMesh(d,highway,exact,required,probe);
                for(const auto&slot:{
                    exact,generic
                }){
                    auto e=c->second.entries.find(slot);
                    if(e!=c->second.entries.end()&&e->second.assignment!=Assignment::Inherit)return resolveMesh(d,highway,slot,required,probe);
                }
            }
            i=d.entities.find(i->second.parent);
        }
        return resolveMesh(d,highway,generic,required,probe);
    }
    ResolvedMesh resolveTopologyMesh(const Document&d,const Id&t,const std::string&s,bool required,const AssetProbe&p){
        auto i=d.topology.find(t);
        if(i==d.topology.end()){
            ResolvedMesh r;
            r.issues.push_back(error("TOPOLOGY",t,"Shared topology not found."));
            return r;
        }
        return resolveMesh(d,i->second.meshOwnerHighway,s,required,p);
    }
    Issues compatible(const MeshInterface&m,const Profile&a,const Profile&b,double tolerance){
        Issues r;
        auto fail=[&](std::string c,std::string text){
            r.push_back(error(std::move(c),a.id,std::move(text)));
        };
        if(!finitePositive(tolerance)||!finitePositive(m.lengthM)||!finitePositive(m.metersPerUnit)||std::abs(m.metersPerUnit-0.01)>1e-10||m.forwardAxis!="X"||m.upAxis!="Z"||m.pivot!="StartCenter")fail("MESH_FRAME","Expected centimetre asset units, +X forward, +Z up, StartCenter pivot and positive length.");
        if(!m.openEnds||!m.textureMarkings)fail("MESH_SURFACE","Modular ends must be open; lane markings must be texture-based.");
        if(m.ports.size()!=2){
            fail("MESH_PORTS","Road/adapter mesh needs exactly Start and End port contracts.");
            return r;
        }
        for(std::size_t i=0;i<2;++i){
            const auto&p=i==0?a:b;
            const auto&port=m.ports[i];
            if(port.name!=(i==0?"Start":"End"))fail("PORT_NAME","Port order/names must be Start, End.");
            for(Direction dir:{
                Direction::Forward,Direction::Reverse
            }){
                auto ls=lanes(p,dir);
                const auto&ws=dir==Direction::Forward?port.forwardWidths:port.reverseWidths;
                if(ls.size()!=ws.size())fail("PORT_LANES","Port lane count does not match profile.");
                else for(std::size_t j=0;j<ws.size();++j)if(!finitePositive(ws[j])||std::abs(ws[j]-ls[j]->widthM)>tolerance)fail("PORT_WIDTH","Port lane width does not match profile.");
            }
            const std::array<double,4> actual{
                port.sidewalkLeftM,port.sidewalkRightM,port.borderLeftM,port.borderRightM
            },expected{
                p.sidewalkLeftM,p.sidewalkRightM,p.borderLeftM,p.borderRightM
            };
            for(std::size_t j=0;j<4;++j)if(!finiteNonnegative(actual[j])||std::abs(actual[j]-expected[j])>tolerance)fail("PORT_EDGE","Sidewalk/border boundary does not match profile.");
        }
        return r;
    }
    const Profile* profileAt(const Document&d,const Segment&s,double station){
        Id id;
        auto h=d.entities.find(s.highway);
        if(h!=d.entities.end())id=h->second.defaultProfile;
        for(const auto&section:s.sections){
            if(section.startM>station+1e-9)break;
            id=section.profile;
        }
        auto p=d.profiles.find(id);
        return p==d.profiles.end()?nullptr:&p->second;
    }
    std::vector<LaneLink> proposeLinks(const Profile&a,const Profile&b,const std::function<Id()>&newId){
        std::vector<LaneLink> out;
        for(Direction dir:{
            Direction::Forward,Direction::Reverse
        }){
            const auto src=lanes(dir==Direction::Forward?a:b,dir),dst=lanes(dir==Direction::Forward?b:a,dir);
            if(src.empty()||dst.empty())continue;
            for(std::size_t i=0;i<std::max(src.size(),dst.size());++i){
                const auto*x=src[std::min(i,src.size()-1)],*y=dst[std::min(i,dst.size()-1)];
                out.push_back({
                    newId(),x->id,y->id,dir,i>=dst.size()?LinkKind::Merge:(i>=src.size()?LinkKind::Branch:LinkKind::Continue),false
                });
            }
        }
        return out;
    }
    Issues validateAdapter(const Adapter&ad,const Profile&a,const Profile&b,double sourceLength){
        Issues r;
        if(!finiteNonnegative(ad.startM)||!finitePositive(ad.endM)||ad.endM<=ad.startM||ad.endM>sourceLength||!finitePositive(ad.minimumTaperRatio))r.push_back(error("ADAPTER_INTERVAL",ad.id,"Transition needs a finite, ordered station interval inside its source."));
        if(ad.before!=a.id||ad.after!=b.id)r.push_back(error("ADAPTER_PROFILE",ad.id,"Provided profiles do not match adapter references."));
        std::set<std::pair<Id,Id>> pairs;
        for(Direction dir:{
            Direction::Forward,Direction::Reverse
        }){
            const Profile&sp=dir==Direction::Forward?a:b;
            const Profile&tp=dir==Direction::Forward?b:a;
            auto src=lanes(sp,dir),dst=lanes(tp,dir);
            std::set<Id> coveredSrc,coveredDst;
            double sw=0,tw=0;
            for(auto*l:src)sw+=l->widthM;
            for(auto*l:dst)tw+=l->widthM;
            if(ad.endM-ad.startM+1e-9<std::abs(sw-tw)*ad.minimumTaperRatio)r.push_back(error("TAPER_TOO_SHORT",ad.id,"Transition does not satisfy the configured width-change/taper ratio."));
            if(std::max(src.size(),dst.size())>std::min(src.size(),dst.size())+1)r.push_back(error("STAGED_ADAPTER_REQUIRED",ad.id,"Change at most one lane per direction per adapter; stage 6→2 as 6→4 then 4→2."));
            std::map<Id,int> fromCount,toCount;
            for(const auto&l:ad.links)if(l.direction==dir){
                ++fromCount[l.fromLane];
                ++toCount[l.toLane];
            }
            for(const auto&l:ad.links)if(l.direction==dir){
                const auto*x=lane(sp,l.fromLane),*y=lane(tp,l.toLane);
                if(!x||!y||x->direction!=dir||y->direction!=dir){
                    r.push_back(error("WRONG_WAY_LINK",l.id,"Link must reference the correct directional input/output lanes."));
                    continue;
                }
                if(!pairs.insert({
                    l.fromLane,l.toLane
                }).second)r.push_back(error("DUPLICATE_LINK",l.id,"Duplicate lane connection."));
                if((x->access&y->access)==0)r.push_back(error("LINK_ACCESS",l.id,"Connection has no compatible participant class."));
                if(!l.approved)r.push_back(error("LINK_UNAPPROVED",l.id,"Review and approve this generated lane mapping."));
                if(toCount[l.toLane]>1&&l.kind!=LinkKind::Merge&&x->ordinal!=y->ordinal)r.push_back(error("MERGE_RULE",l.id,"A converging lane needs explicit merge priority semantics."));
                if(fromCount[l.fromLane]>1&&l.kind!=LinkKind::Branch&&x->ordinal!=y->ordinal)r.push_back(error("BRANCH_RULE",l.id,"A diverging lane needs explicit branch semantics."));
                coveredSrc.insert(x->id);
                coveredDst.insert(y->id);
            }
            for(auto*l:src)if(!coveredSrc.count(l->id))r.push_back(error("MISSING_SUCCESSOR",l->id,"Incoming lane has no approved successor; do not guess a route."));
            for(auto*l:dst)if(!coveredDst.count(l->id))r.push_back(error("MISSING_PREDECESSOR",l->id,"Outgoing lane has no incoming connection."));
        }
        for(const auto&l:ad.links)if(static_cast<unsigned>(l.direction)>1||static_cast<unsigned>(l.kind)>2)r.push_back(error("LINK_ENUM",l.id,"Invalid lane-link enum."));
        return r;
    }
    Session::Session(Document d):committed_(std::move(d)),draft_(committed_){
    }
    void Session::revert(){
        draft_=committed_;
        baseEpoch_=epoch_;
    }
    bool Session::dirty()const{
        return contentFingerprint(draft_)!=contentFingerprint(committed_);
    }
    Issues Session::apply(){
        if(baseEpoch_!=epoch_)return {
            error("STALE_DRAFT",draft_.id,"Committed document changed; explicitly rebase or revert the draft.")
        };
        auto r=validate(draft_);
        if(hasErrors(r)||!dirty())return r;
        undo_.push_back(committed_);
        redo_.clear();
        draft_.revision=committed_.revision+1;
        committed_=draft_;
        ++epoch_;
        baseEpoch_=epoch_;
        return r;
    }
    bool Session::undo(){
        if(undo_.empty()||dirty())return false;
        redo_.push_back(committed_);
        committed_=undo_.back();
        undo_.pop_back();
        ++epoch_;
        revert();
        return true;
    }
    bool Session::redo(){
        if(redo_.empty()||dirty())return false;
        undo_.push_back(committed_);
        committed_=redo_.back();
        redo_.pop_back();
        ++epoch_;
        revert();
        return true;
    }
    void Session::replaceExternal(Document d){
        committed_=std::move(d);
        ++epoch_;
        undo_.clear();
        redo_.clear();
    }
}
// namespace nexus
