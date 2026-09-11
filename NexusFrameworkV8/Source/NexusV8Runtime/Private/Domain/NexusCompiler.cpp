#include "Domain/NexusDomain.h"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
namespace nexus {
    namespace {
        constexpr double Epsilon=1e-7;
        Issue fail(std::string code,Id id,std::string msg){
            return {
                std::move(code),std::move(id),std::move(msg),true
            };
        }
        void append(Issues&a,const Issues&b){
            a.insert(a.end(),b.begin(),b.end());
        }
        double smooth(double t){
            return t*t*t*(t*(t*6-15)+10);
        }
        double mix(double a,double b,double t){
            return a+(b-a)*t;
        }
        const Lane* findLane(const Profile&p,const Id&id){
            for(const auto&l:p.lanes)if(l.id==id)return &l;
            return nullptr;
        }
        std::vector<const Lane*> ordered(const Profile&p,Direction dir){
            std::vector<const Lane*>ls;
            for(const auto&l:p.lanes)if(l.direction==dir)ls.push_back(&l);
            std::sort(ls.begin(),ls.end(),[](const Lane*a,const Lane*b){
                return a->ordinal<b->ordinal;
            });
            return ls;
        }
        double sideSign(Direction dir,DrivingSide side){
            return (dir==Direction::Forward?1.0:-1.0)*(side==DrivingSide::Right?1.0:-1.0);
        }
        double centerOffset(const Profile&p,const Lane&lane,DrivingSide side){
            double offset=p.medianM*0.5;
            for(const auto*l:ordered(p,lane.direction)){
                if(l->id==lane.id)return sideSign(lane.direction,side)*(offset+l->widthM*0.5);
                offset+=l->widthM;
            }
            return 0;
        }
        std::array<double,2> extents(const Profile&p,DrivingSide side){
            double left=p.medianM*0.5,right=p.medianM*0.5;
            for(const auto&l:p.lanes){
                if(sideSign(l.direction,side)<0)left+=l.widthM;
                else right+=l.widthM;
            }
            return {
                -left,right
            };
        }
        Id derived(const std::string&s){
            return fingerprint("NexusV8:"+s)+fingerprint("NexusV8:2:"+s);
        }
        struct CrossSection {
            const Profile*a=nullptr,*b=nullptr;
            double t=0;
        };
        CrossSection at(const Document&d,const Segment&s,double x){
            for(const auto&a:s.adapters)if(x>=a.startM-Epsilon&&x<=a.endM+Epsilon){
                return {
                    &d.profiles.at(a.before),&d.profiles.at(a.after),smooth(std::clamp((x-a.startM)/(a.endM-a.startM),0.0,1.0))
                };
            }
            auto p=profileAt(d,s,x);
            return {
                p,p,0
            };
        }
        std::vector<double> stations(double from,double to,double spacing){
            std::vector<double>r;
            std::size_t count=static_cast<std::size_t>(std::ceil((to-from)/spacing));
            count=std::max<std::size_t>(1,count);
            for(std::size_t i=0;i<=count;++i)r.push_back(mix(from,to,static_cast<double>(i)/count));
            return r;
        }
        void makeStrip(RoadOutput&road,const SourceSegment&source,const std::vector<double>&ss,const std::string&role,const std::function<std::array<double,4>(double)>&coords){
            Surface surf;
            surf.role=role;
            for(double station:ss){
                auto f=sampleSource(source,station);
                auto right=unit(cross(f.up,f.forward));
                const auto v=coords(station);
                Vec3 a=f.position+right*v[0]+f.up*v[1],b=f.position+right*v[2]+f.up*v[3];
                Vec3 normal=unit(cross(f.forward,b-a));
                if(length(normal)<0.5)normal=f.up;
                surf.vertices.push_back({
                    a,normal,0,station
                });
                surf.vertices.push_back({
                    b,normal,1,station
                });
            }
            for(std::size_t i=0;i+1<ss.size();++i){
                auto a=static_cast<std::uint32_t>(i*2);
                for(auto triangle: {
                    std::array<std::uint32_t,3>{
                        a,a+2,a+3
                    },std::array<std::uint32_t,3>{
                        a,a+3,a+1
                    }
                }){
                    const auto A=surf.vertices[triangle[0]].position,B=surf.vertices[triangle[1]].position,C=surf.vertices[triangle[2]].position;
                    if(length(cross(B-A,C-A))>1e-10)surf.indices.insert(surf.indices.end(),triangle.begin(),triangle.end());
                }
            }
            road.surfaces.push_back(std::move(surf));
        }
        Corridor corridor(const SourceSegment&src,const Id&segment,const Profile&a,const Profile&b,const Lane&la,const Lane&lb,double from,double to,DrivingSide side,double spacing,bool blend){
            Corridor c;
            c.id=derived(segment+":"+la.id+":"+lb.id+":"+std::to_string(from)+":"+std::to_string(to));
            c.segment=segment;
            c.fromLane=la.direction==Direction::Forward?la.id:lb.id;
            c.toLane=la.direction==Direction::Forward?lb.id:la.id;
            c.direction=la.direction;
            c.access=la.access&lb.access;
            for(double x:stations(from,to,spacing)){
                auto f=sampleSource(src,x);
                const auto right=unit(cross(f.up,f.forward));
                double t=blend?smooth((x-from)/(to-from)):0;
                double y=mix(centerOffset(a,la,side),centerOffset(b,lb,side),t);
                double width=mix(la.widthM,lb.widthM,t);
                auto center=f.position+right*y;
                auto lateral=right*(la.direction==Direction::Forward?1.0:-1.0);
                c.samples.push_back({
                    x,width,center,center-lateral*(width/2),center+lateral*(width/2)
                });
            }
            if(c.direction==Direction::Reverse)std::reverse(c.samples.begin(),c.samples.end());
            return c;
        }
        void makeMarkingTexture(RoadOutput&road,const Document&d,const Segment&s,DrivingSide side,double sourceLength){
            road.markingRGBA.resize(static_cast<std::size_t>(road.textureWidth)*road.textureHeight*4);
            for(std::uint32_t y=0;y<road.textureHeight;++y){
                const double station=sourceLength*(static_cast<double>(y)+0.5)/road.textureHeight;
                const auto section=at(d,s,station);
                auto ea=extents(*section.a,side),eb=extents(*section.b,side);
                double lo=mix(ea[0],eb[0],section.t),hi=mix(ea[1],eb[1],section.t);
                std::vector<double> dashed;
                for(Direction dir:{
                    Direction::Forward,Direction::Reverse
                }){
                    auto as=ordered(*section.a,dir),bs=ordered(*section.b,dir);
                    double ao=section.a->medianM/2,bo=section.b->medianM/2;
                    const std::size_t count=std::max(as.size(),bs.size());
                    for(std::size_t i=0;i<count;++i){
                        if(i<as.size())ao+=as[i]->widthM;
                        if(i<bs.size())bo+=bs[i]->widthM;
                        if(i+1<count)dashed.push_back(sideSign(dir,side)*mix(ao,bo,section.t));
                    }
                }
                const bool twoWay=!ordered(*section.a,Direction::Forward).empty()&&!ordered(*section.a,Direction::Reverse).empty();
                for(std::uint32_t x=0;x<road.textureWidth;++x){
                    double offset=mix(lo,hi,(x+0.5)/road.textureWidth);
                    auto index=(static_cast<std::size_t>(y)*road.textureWidth+x)*4;
                    unsigned noise=(x*73856093u^y*19349663u)%13;
                    std::uint8_t r=static_cast<std::uint8_t>(48+noise),g=r,b=r;
                    bool edge=std::min(std::abs(offset-(lo+0.12)),std::abs(offset-(hi-0.12)))<0.065;
                    bool dash=false;
                    if(std::fmod(station,6.0)<3.0)for(double boundary:dashed)if(boundary>lo+0.2&&boundary<hi-0.2&&std::abs(offset-boundary)<0.065)dash=true;
                    if(twoWay&&std::abs(offset)<0.1){
                        r=236;
                        g=192;
                        b=40;
                    }else if(edge||dash){
                        r=221;
                        g=224;
                        b=219;
                    }
                    road.markingRGBA[index]=r;
                    road.markingRGBA[index+1]=g;
                    road.markingRGBA[index+2]=b;
                    road.markingRGBA[index+3]=255;
                }
            }
        }
    }
    Issues checkContainment(const Corridor&c,double width,double vehicleLength,double margin){
        Issues out;
        if(width<=0||vehicleLength<=0||margin<0||!std::isfinite(width+vehicleLength+margin))return {
            fail("VEHICLE_DIMENSIONS",c.id,"Vehicle dimensions/margin must be finite and valid.")
        };
        for(std::size_t i=0;i<c.samples.size();++i){
            const auto&s=c.samples[i];
            double curvature=0;
            if(i>0&&i+1<c.samples.size()){
                auto a=s.center-c.samples[i-1].center,b=c.samples[i+1].center-s.center;
                double len=(length(a)+length(b))/2;
                if(len>1e-8)curvature=length(unit(b)-unit(a))/len;
            }
            // Conservative local swept-envelope bound; full physics/controller verification
            // remains a separate engine acceptance test, not implied by this bound.
            const double extra=curvature*vehicleLength*vehicleLength/8;
            if(width/2+margin+extra>s.widthM/2+1e-6){
                out.push_back(fail("VEHICLE_CORRIDOR",c.id,"Vehicle width/turn envelope exceeds usable corridor at station "+std::to_string(s.stationM)));
                break;
            }
        }
        return out;
    }
    CompileResult compilePreview(const Document&d,const SourceScan&source,const AssetProbe&probe,const CompileOptions&options){
        CompileResult result;
        result.issues=validate(d,false);
        if(hasErrors(result.issues))return result;
        auto canceled=[&](){
            return options.canceled&&options.canceled();
        };
        if(!std::isfinite(options.sampleSpacingM)||options.sampleSpacingM<0.1||options.sampleSpacingM>20||!std::isfinite(options.maxGrade)||options.maxGrade<=0||!std::isfinite(options.maxBankRadians)||options.maxBankRadians<=0){
            result.issues.push_back(fail("COMPILE_OPTIONS",d.id,"Sampling/grade/bank limits are invalid."));
            return result;
        }
        if(d.sourceWorldKey!=source.worldKey){
            result.issues.push_back(fail("WRONG_WORLD",d.id,"Scan belongs to a different world."));
            return result;
        }
        auto&snapshot=result.snapshot;
        snapshot.document=d.id;
        snapshot.revision=d.revision;
        snapshot.documentFingerprint=contentFingerprint(d);
        snapshot.sourceFingerprint=source.fingerprint;
        if(d.segments.empty()){
            result.issues.push_back(fail("NO_SEGMENTS",d.id,"Scan and bind source segments before building a preview."));
            return result;
        }
        for(const auto&kv:d.segments){
            const auto&s=kv.second;
            if(canceled()){
                result.issues.push_back(fail("CANCELED",s.id,"Preview canceled; no published output has been changed."));
                snapshot=PreviewSnapshot{
                };
                return result;
            }
            auto si=source.segments.find(s.sourceKey);
            if(si==source.segments.end()||si->second.availability!=Availability::Loaded||s.availability!=Availability::Loaded){
                result.issues.push_back(fail("SOURCE_UNAVAILABLE",s.id,"Required segment is missing/unloaded/deleted; wait or reconcile, never fabricate geometry."));
                continue;
            }
            const auto&src=si->second;
            if(src.fingerprint!=s.acceptedSourceFingerprint){
                result.issues.push_back(fail("SOURCE_STALE",s.id,"Source changed after review; rescan and apply before compiling."));
                continue;
            }
            if(src.samples.size()<2){
                result.issues.push_back(fail("SOURCE_SAMPLES",s.id,"Source needs at least two samples."));
                continue;
            }
            double previous=-1;
            bool bad=false;
            for(const auto&x:src.samples){
                const double horizontal=std::sqrt(x.forward.x*x.forward.x+x.forward.y*x.forward.y);
                if(!std::isfinite(x.stationM)||x.stationM<=previous||!finite(x.position)||!finite(x.forward)||!finite(x.up)||std::abs(length(x.forward)-1)>0.001||std::abs(length(x.up)-1)>0.001||std::abs(dot(x.forward,x.up))>0.001){
                    result.issues.push_back(fail("SOURCE_FRAME",s.id,"Source frames/stations must be finite, monotonic and orthonormal."));
                    bad=true;
                    break;
                }
                previous=x.stationM;
                if(horizontal<1e-8||std::abs(x.forward.z)/horizontal>options.maxGrade||std::acos(std::clamp(x.up.z,-1.0,1.0))>options.maxBankRadians){
                    result.issues.push_back(fail("GRADE_BANK_LIMIT",s.id,"Source exceeds configured grade/bank preview limits."));
                    bad=true;
                    break;
                }
            }
            if(bad)continue;
            const double total=src.samples.back().stationM;
            if(std::abs(src.samples.front().stationM)>1e-8||total/options.sampleSpacingM>100000||total<=0){
                result.issues.push_back(fail("SOURCE_LENGTH",s.id,"Invalid/oversized source station range."));
                continue;
            }
            if(!std::isfinite(src.start.signedTangentM)||!std::isfinite(src.end.signedTangentM)){
                result.issues.push_back(fail("SOURCE_TANGENT",s.id,"Signed per-end tangents must be finite."));
                continue;
            }
            const auto*h=&d.entities.at(s.highway);
            const auto*start=profileAt(d,s,0);
            if(!start){
                result.issues.push_back(fail("PROFILE_MISSING",s.id,"Assign a highway default or section profile."));
                continue;
            }
            for(const auto&section:s.sections)if(section.startM>total)result.issues.push_back(fail("SECTION_RANGE",s.id,"Section starts outside the source segment."));
            for(const auto&ad:s.adapters){
                auto&a=d.profiles.at(ad.before),&b=d.profiles.at(ad.after);
                append(result.issues,validateAdapter(ad,a,b,total));
                const auto*before=profileAt(d,s,ad.startM);
                const auto*after=profileAt(d,s,ad.endM);
                if(!before||!after||before->id!=ad.before||after->id!=ad.after)result.issues.push_back(fail("PROFILE_ADAPTER_MISMATCH",ad.id,"Adapter before/after must agree with the authoritative segment sections."));
                for(const auto&sec:s.sections)if(sec.startM>ad.startM+Epsilon&&sec.startM<ad.endM-Epsilon)result.issues.push_back(fail("SECTION_INSIDE_ADAPTER",ad.id,"A section change cannot hide inside a transition interval."));
            }
            for(std::size_t i=1;i<s.sections.size();++i)if(s.sections[i-1].profile!=s.sections[i].profile){
                bool found=false;
                for(const auto&ad:s.adapters)if(std::abs(ad.endM-s.sections[i].startM)<Epsilon&&ad.before==s.sections[i-1].profile&&ad.after==s.sections[i].profile)found=true;
                if(!found)result.issues.push_back(fail("UNMAPPED_PROFILE_CHANGE",s.id,"Profile changes require a continuous explicit adapter."));
            }
            if(options.requireAssets){
                // Count-only slots are convenient presets, not complete profile identities.
                // Resolve exact profile variants first. Validate EVERY use of a generic slot:
                // two six-lane profiles may have different directions or widths.
                auto need=[&](const Profile&a,const Profile&b,bool adapter){
                    const std::string generic=adapter?"Adapter."+std::to_string(a.lanes.size())+"-"+std::to_string(b.lanes.size()):"Road."+std::to_string(a.lanes.size());
                    const std::string exact=adapter?"Adapter.Profile."+a.id+"-"+b.id:"Road.Profile."+a.id;
                    auto rm=resolveMeshVariant(d,s.highway,exact,generic,true,probe);
                    const std::string used=rm.slot;
                    append(result.issues,rm.issues);
                    if(rm.resolved)append(result.issues,compatible(rm.entry.interface,a,b));
                    snapshot.meshes[s.id][used]=rm;
                };
                need(*start,*start,false);
                for(const auto&sec:s.sections){
                    const auto&p=d.profiles.at(sec.profile);
                    need(p,p,false);
                }
                for(const auto&ad:s.adapters)need(d.profiles.at(ad.before),d.profiles.at(ad.after),true);
            }
            if(hasErrors(result.issues))continue;
            RoadOutput road;
            road.segment=s.id;
            std::set<double> samples;
            for(double x:stations(0,total,options.sampleSpacingM))samples.insert(x);
            for(const auto&ad:s.adapters){
                samples.insert(ad.startM);
                samples.insert(ad.endM);
            }
            std::vector<double>ss(samples.begin(),samples.end());
            auto dimensions=[&](double x){
                auto cs=at(d,s,x);
                auto a=extents(*cs.a,h->drivingSide),b=extents(*cs.b,h->drivingSide);
                return std::array<double,7>{
                    mix(a[0],b[0],cs.t),mix(a[1],b[1],cs.t),mix(cs.a->borderLeftM,cs.b->borderLeftM,cs.t),mix(cs.a->borderRightM,cs.b->borderRightM,cs.t),mix(cs.a->sidewalkLeftM,cs.b->sidewalkLeftM,cs.t),mix(cs.a->sidewalkRightM,cs.b->sidewalkRightM,cs.t),mix(cs.a->sidewalkHeightM,cs.b->sidewalkHeightM,cs.t)
                };
            };
            makeStrip(road,src,ss,"Road",[&](double x){
                auto z=dimensions(x);
                return std::array<double,4>{
                    z[0],0,z[1],0
                };
            });
            makeStrip(road,src,ss,"Border.Left",[&](double x){
                auto z=dimensions(x);
                return std::array<double,4>{
                    z[0]-z[2],z[6],z[0],z[6]
                };
            });
            makeStrip(road,src,ss,"Border.Right",[&](double x){
                auto z=dimensions(x);
                return std::array<double,4>{
                    z[1],z[6],z[1]+z[3],z[6]
                };
            });
            makeStrip(road,src,ss,"Sidewalk.Left",[&](double x){
                auto z=dimensions(x);
                return std::array<double,4>{
                    z[0]-z[2]-z[4],z[6],z[0]-z[2],z[6]
                };
            });
            makeStrip(road,src,ss,"Sidewalk.Right",[&](double x){
                auto z=dimensions(x);
                return std::array<double,4>{
                    z[1]+z[3],z[6],z[1]+z[3]+z[5],z[6]
                };
            });
            makeStrip(road,src,ss,"Curb.Left",[&](double x){
                auto z=dimensions(x);
                return std::array<double,4>{
                    z[0],z[6],z[0],0
                };
            });
            makeStrip(road,src,ss,"Curb.Right",[&](double x){
                auto z=dimensions(x);
                return std::array<double,4>{
                    z[1],0,z[1],z[6]
                };
            });
            std::vector<std::pair<double,double>>regular;
            double cursor=0;
            for(const auto&ad:s.adapters){
                if(ad.startM>cursor+Epsilon)regular.push_back({
                    cursor,ad.startM
                });
                cursor=ad.endM;
                const auto&a=d.profiles.at(ad.before),&b=d.profiles.at(ad.after);
                for(const auto&link:ad.links){
                    const auto*la=findLane(a,link.direction==Direction::Forward?link.fromLane:link.toLane);
                    const auto*lb=findLane(b,link.direction==Direction::Forward?link.toLane:link.fromLane);
                    if(la&&lb)road.corridors.push_back(corridor(src,s.id,a,b,*la,*lb,ad.startM,ad.endM,h->drivingSide,options.sampleSpacingM,true));
                }
            }
            if(cursor<total-Epsilon)regular.push_back({
                cursor,total
            });
            for(const auto&range:regular){
                const auto*p=profileAt(d,s,range.first);
                if(p)for(const auto&l:p->lanes)road.corridors.push_back(corridor(src,s.id,*p,*p,l,l,range.first,range.second,h->drivingSide,options.sampleSpacingM,false));
            }
            for(const auto&c:road.corridors)append(result.issues,checkContainment(c,options.vehicleWidthM,options.vehicleLengthM,options.marginM));
            makeMarkingTexture(road,d,s,h->drivingSide,total);
            snapshot.roads.push_back(std::move(road));
        }
        // Adjacent ordinary sections cannot hide a width/count discontinuity at a shared anchor.
        for(const auto& Point:d.entities) if(Point.second.kind==Kind::Point){
            const Segment* incoming=nullptr;
            const Segment* outgoing=nullptr;
            std::size_t incidence=0;
            for(const auto& Pair:d.segments){
                const auto& S=Pair.second;
                if(S.endPoint==Point.first){
                    incoming=&S;
                    ++incidence;
                }
                if(S.startPoint==Point.first){
                    outgoing=&S;
                    ++incidence;
                }
            }
            if(incidence!=2||!incoming||!outgoing)continue;
            auto src=source.segments.find(incoming->sourceKey);
            if(src==source.segments.end()||src->second.samples.empty())continue;
            const auto* A=profileAt(d,*incoming,src->second.samples.back().stationM);
            const auto* B=profileAt(d,*outgoing,0);
            if(!A||!B)continue;
            bool mismatch=A->lanes.size()!=B->lanes.size();
            for(auto dir:{
                Direction::Forward,Direction::Reverse
            }){
                auto a=ordered(*A,dir),b=ordered(*B,dir);
                if(a.size()!=b.size())mismatch=true;
                else for(std::size_t i=0;i<a.size();++i)if(std::abs(a[i]->widthM-b[i]->widthM)>0.005)mismatch=true;
            }
            const std::array<double,6> a{
                A->medianM,A->sidewalkLeftM,A->sidewalkRightM,A->borderLeftM,A->borderRightM,A->sidewalkHeightM
            },b{
                B->medianM,B->sidewalkLeftM,B->sidewalkRightM,B->borderLeftM,B->borderRightM,B->sidewalkHeightM
            };
            for(std::size_t i=0;i<a.size();++i)if(std::abs(a[i]-b[i])>0.005)mismatch=true;
            if(mismatch)result.issues.push_back(fail("JOIN_PROFILE_MISMATCH",Point.first,"Incoming and outgoing profiles have incompatible lane/sidewalk/border boundaries. Author an explicit adapter."));
        }
        if(canceled())result.issues.push_back(fail("CANCELED",d.id,"Canceled before preview promotion."));
        if(hasErrors(result.issues)){
            snapshot=PreviewSnapshot{
            };
            return result;
        }
        std::ostringstream digest;
        digest<<std::setprecision(17)<<snapshot.documentFingerprint<<snapshot.sourceFingerprint;
        for(const auto&r:snapshot.roads){
            digest<<r.segment;
            for(const auto&s:r.surfaces){
                digest<<s.role;
                for(const auto&v:s.vertices)digest<<v.position.x<<','<<v.position.y<<','<<v.position.z<<';';
            }
            for(const auto&c:r.corridors){
                digest<<c.id;
                for(const auto&x:c.samples)digest<<x.center.x<<','<<x.center.y<<','<<x.center.z<<','<<x.widthM<<';';
            }
            digest<<fingerprint(std::string(r.markingRGBA.begin(),r.markingRGBA.end()));
        }
        snapshot.contentHash=fingerprint(digest.str());
        result.success=true;
        return result;
    }
}
// namespace nexus
