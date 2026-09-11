#include "Domain/NexusDomain.h"
#include <algorithm>
#include <cmath>
#include <sstream>
namespace nexus {
    namespace {
        Issue fail(std::string code,Id id,std::string msg){
            return {
                std::move(code),std::move(id),std::move(msg),true
            };
        }
        std::string scopeOf(const std::string&key){
            auto n=key.rfind('#');
            return n==std::string::npos?key:key.substr(0,n);
        }
        Availability absentState(const SourceScan&scan,const std::string&key){
            if(scan.deletedKeys.count(key))return Availability::Deleted;
            return scan.completeScopes.count(scopeOf(key))?Availability::Missing:Availability::Unloaded;
        }
    }
    ScanDiff diffScan(const Document&d,const SourceScan&scan,const Id&highway){
        ScanDiff out;
        out.baseRevision=d.revision;
        out.documentFingerprint=contentFingerprint(d);
        out.scanFingerprint=scan.fingerprint;
        out.highway=highway;
        auto h=d.entities.find(highway);
        if(h==d.entities.end()||h->second.kind!=Kind::Highway){
            out.issues.push_back(fail("SCAN_HIGHWAY",highway,"Select a highway before scanning."));
            return out;
        }
        if(!d.sourceWorldKey.empty()&&d.sourceWorldKey!=scan.worldKey){
            out.issues.push_back(fail("WRONG_WORLD",highway,"Source map does not match the bound document. Explicitly migrate/rebind first."));
            return out;
        }
        std::map<std::string,Id> pointMap,segmentMap;
        for(const auto&kv:d.entities)if(kv.second.kind==Kind::Point&&kv.second.parent==highway&&!kv.second.sourceKey.empty())pointMap[kv.second.sourceKey]=kv.first;
        for(const auto&kv:d.segments)if(kv.second.highway==highway&&!kv.second.sourceKey.empty())segmentMap[kv.second.sourceKey]=kv.first;
        for(const auto&kv:scan.points){
            const auto&p=kv.second;
            auto i=pointMap.find(kv.first);
            if(i==pointMap.end()){
                if(p.availability==Availability::Loaded)out.changes.push_back({
                    ChangeKind::Added,false,{
                    },kv.first,p.availability
                });
            } else{
                const auto&e=d.entities.at(i->second);
                if(e.acceptedSourceFingerprint!=p.fingerprint||e.availability!=p.availability)out.changes.push_back({
                    ChangeKind::Changed,false,i->second,kv.first,p.availability
                });
            }
        }
        for(const auto&kv:scan.segments){
            const auto&s=kv.second;
            auto i=segmentMap.find(kv.first);
            if(!scan.points.count(s.start.pointKey)||!scan.points.count(s.end.pointKey)){
                out.issues.push_back(fail("SOURCE_ENDPOINT",{
                },"Source segment endpoint is unavailable: "+kv.first));
                continue;
            }
            for(const auto&existing:d.segments)if(existing.second.sourceKey==kv.first&&existing.second.highway!=highway)out.issues.push_back(fail("SOURCE_OWNED",existing.first,"Segment already belongs to another highway; choose a narrower source scan."));
            if(i==segmentMap.end()){
                if(s.availability==Availability::Loaded)out.changes.push_back({
                    ChangeKind::Added,true,{
                    },kv.first,s.availability
                });
            } else{
                const auto&e=d.segments.at(i->second);
                const auto&a=d.entities.at(e.startPoint);
                const auto&b=d.entities.at(e.endPoint);
                if(a.sourceKey!=s.start.pointKey||b.sourceKey!=s.end.pointKey)out.issues.push_back(fail("REMAP_REQUIRED",e.id,"Source segment endpoints changed. Review/rebind rather than discarding authored sections or adapters."));
                if(e.acceptedSourceFingerprint!=s.fingerprint||e.availability!=s.availability)out.changes.push_back({
                    ChangeKind::Changed,true,i->second,kv.first,s.availability
                });
            }
        }
        for(const auto&kv:pointMap)if(!scan.points.count(kv.first)){
            auto state=absentState(scan,kv.first);
            if(d.entities.at(kv.second).availability!=state)out.changes.push_back({
                state==Availability::Deleted?ChangeKind::Deleted:ChangeKind::Unavailable,false,kv.second,kv.first,state
            });
        }
        for(const auto&kv:segmentMap)if(!scan.segments.count(kv.first)){
            auto state=absentState(scan,kv.first);
            if(d.segments.at(kv.second).availability!=state)out.changes.push_back({
                state==Availability::Deleted?ChangeKind::Deleted:ChangeKind::Unavailable,true,kv.second,kv.first,state
            });
        }
        return out;
    }
    Issues applyScan(Document&d,const SourceScan&scan,const ScanDiff&diff,const std::function<Id()>&newId){
        if(diff.baseRevision!=d.revision||diff.documentFingerprint!=contentFingerprint(d)||diff.scanFingerprint!=scan.fingerprint)return {
            fail("STALE_SCAN",diff.highway,"Document or scan changed since review. Scan again.")
        };
        auto fresh=diffScan(d,scan,diff.highway);
        if(hasErrors(fresh.issues))return fresh.issues;
        // Do not trust externally supplied change rows. Recompute against the bound versions.
        Document next=d;
        if(next.sourceWorldKey.empty())next.sourceWorldKey=scan.worldKey;
        std::map<std::string,Id> points;
        for(const auto&kv:next.entities)if(kv.second.kind==Kind::Point&&kv.second.parent==diff.highway)points[kv.second.sourceKey]=kv.first;
        for(const auto&c:fresh.changes)if(!c.isSegment){
            if(c.kind==ChangeKind::Added){
                Entity e;
                e.id=newId();
                e.parent=diff.highway;
                e.kind=Kind::Point;
                e.name="Point "+std::to_string(points.size()+1);
                e.sourceKey=c.sourceKey;
                e.availability=c.availability;
                e.acceptedSourceFingerprint=scan.points.at(c.sourceKey).fingerprint;
                if(next.entities.count(e.id))return {
                    fail("DUPLICATE_ID",e.id,"ID provider returned a duplicate.")
                };
                points[c.sourceKey]=e.id;
                next.entities.emplace(e.id,std::move(e));
            } else{
                auto&e=next.entities.at(c.entity);
                e.availability=c.availability;
                auto p=scan.points.find(c.sourceKey);
                if(p!=scan.points.end())e.acceptedSourceFingerprint=p->second.fingerprint;
            }
        }
        for(const auto&c:fresh.changes)if(c.isSegment){
            if(c.kind==ChangeKind::Added){
                const auto&src=scan.segments.at(c.sourceKey);
                Segment s;
                s.id=newId();
                s.highway=diff.highway;
                s.startPoint=points.at(src.start.pointKey);
                s.endPoint=points.at(src.end.pointKey);
                s.sourceKey=c.sourceKey;
                s.availability=c.availability;
                s.acceptedSourceFingerprint=src.fingerprint;
                if(next.segments.count(s.id))return {
                    fail("DUPLICATE_ID",s.id,"ID provider returned a duplicate.")
                };
                next.segments.emplace(s.id,std::move(s));
            } else{
                auto&s=next.segments.at(c.entity);
                s.availability=c.availability;
                auto p=scan.segments.find(c.sourceKey);
                if(p!=scan.segments.end())s.acceptedSourceFingerprint=p->second.fingerprint;
            }
        }
        auto issues=validate(next);
        if(!hasErrors(issues))d=std::move(next);
        return issues;
    }
    Issues rebindSource(Document&d,const Id&id,const std::string&expected,const std::string&key,const SourceScan&scan){
        Document next=d;
        auto p=next.entities.find(id);
        auto s=next.segments.find(id);
        if(p!=next.entities.end()&&p->second.kind==Kind::Point){
            auto q=scan.points.find(key);
            if(p->second.sourceKey!=expected||q==scan.points.end()||q->second.availability!=Availability::Loaded)return {
                fail("REBIND_SOURCE",id,"Expected binding changed or target source is not loaded.")
            };
            p->second.sourceKey=key;
            p->second.acceptedSourceFingerprint=q->second.fingerprint;
            p->second.availability=Availability::Loaded;
        } else if(s!=next.segments.end()){
            auto q=scan.segments.find(key);
            if(s->second.sourceKey!=expected||q==scan.segments.end()||q->second.availability!=Availability::Loaded)return {
                fail("REBIND_SOURCE",id,"Expected binding changed or target source is not loaded.")
            };
            if(next.entities.at(s->second.startPoint).sourceKey!=q->second.start.pointKey||next.entities.at(s->second.endPoint).sourceKey!=q->second.end.pointKey)return {
                fail("REBIND_ENDPOINT",id,"Rebind points explicitly before reassigning a segment.")
            };
            s->second.sourceKey=key;
            s->second.acceptedSourceFingerprint=q->second.fingerprint;
            s->second.availability=Availability::Loaded;
        } else return {
            fail("NOT_FOUND",id,"Point or segment not found.")
        };
        auto issues=validate(next);
        if(!hasErrors(issues))d=std::move(next);
        return issues;
    }
    Sample sampleSource(const SourceSegment&s,double station){
        if(s.samples.empty())return {
        };
        if(station<=s.samples.front().stationM)return s.samples.front();
        if(station>=s.samples.back().stationM)return s.samples.back();
        auto hi=std::upper_bound(s.samples.begin(),s.samples.end(),station,[](double v,const Sample&a){
            return v<a.stationM;
        });
        const auto&a=*(hi-1),&b=*hi;
        double t=(station-a.stationM)/(b.stationM-a.stationM);
        Sample p;
        p.stationM=station;
        p.position=a.position*(1-t)+b.position*t;
        p.forward=unit(a.forward*(1-t)+b.forward*t);
        auto up=unit(a.up*(1-t)+b.up*t);
        auto right=unit(cross(up,p.forward));
        p.up=unit(cross(p.forward,right));
        return p;
    }
    std::vector<SourceEnd> connectedEnds(const SourceScan&s,const std::string&key){
        std::vector<SourceEnd>r;
        for(const auto&kv:s.segments){
            if(kv.second.start.pointKey==key)r.push_back(kv.second.start);
            if(kv.second.end.pointKey==key)r.push_back(kv.second.end);
        }
        return r;
    }
    SourceScan makeExampleScan(){
        SourceScan s;
        s.worldKey="Demo://NexusV8";
        s.completeScopes.insert("demo");
        for(int i=0;i<3;++i){
            SourcePoint p;
            p.key="demo#P"+std::to_string(i);
            p.scope="demo";
            p.frame.position={
                i*160.0,0,0
            };
            p.fingerprint=fingerprint(p.key+std::to_string(i*160));
            s.points.emplace(p.key,p);
        }
        for(int i=0;i<2;++i){
            SourceSegment seg;
            seg.key="demo#S"+std::to_string(i);
            seg.scope="demo";
            seg.start.pointKey="demo#P"+std::to_string(i);
            seg.end.pointKey="demo#P"+std::to_string(i+1);
            seg.start.frame=s.points.at(seg.start.pointKey).frame;
            seg.end.frame=s.points.at(seg.end.pointKey).frame;
            seg.start.signedTangentM=160;
            seg.end.signedTangentM=-160;
            seg.start.tangent={
                160,0,0
            };
            seg.end.tangent={
                -160,0,0
            };
            for(int k=0;k<=160;++k)seg.samples.push_back({
                static_cast<double>(k),{
                    i*160.0+k,0,0
                },{
                    1,0,0
                },{
                    0,0,1
                }
            });
            seg.fingerprint=fingerprint(seg.key+"straight-160m");
            s.segments.emplace(seg.key,seg);
        }
        std::string f;
        for(const auto&p:s.points)f+=p.second.fingerprint;
        for(const auto&p:s.segments)f+=p.second.fingerprint;
        s.fingerprint=fingerprint(f);
        return s;
    }
    Document makeExample(const std::function<Id()>&newId){
        Document d;
        d.id=newId();
        Id parent;
        for(Kind k:{
            Kind::World,Kind::State,Kind::City,Kind::Highway
        }){
            Entity e;
            e.id=newId();
            e.parent=parent;
            e.kind=k;
            e.name=k==Kind::World?"World / Root":k==Kind::State?"State":k==Kind::City?"City":"Highway 01";
            parent=e.id;
            d.entities.emplace(e.id,e);
        }
        std::vector<Id> profiles;
        for(int count:{
            6,4,2
        }){
            Profile p;
            p.id=newId();
            p.name=std::to_string(count)+" lanes";
            for(Direction dir:{
                Direction::Forward,Direction::Reverse
            })for(int i=0;i<count/2;++i){
                Lane l;
                l.id=newId();
                l.direction=dir;
                l.ordinal=static_cast<unsigned>(i);
                p.lanes.push_back(l);
            }
            profiles.push_back(p.id);
            d.profiles.emplace(p.id,p);
        }
        d.entities.at(parent).defaultProfile=profiles[0];
        auto scan=makeExampleScan();
        auto diff=diffScan(d,scan,parent);
        applyScan(d,scan,diff,newId);
        for(auto&kv:d.segments){
            auto&s=kv.second;
            const int i=s.sourceKey=="demo#S0"?0:1;
            Adapter a;
            a.id=newId();
            a.startM=40;
            a.endM=120;
            a.before=profiles[i];
            a.after=profiles[i+1];
            a.links=proposeLinks(d.profiles.at(a.before),d.profiles.at(a.after),newId);
            for(auto&l:a.links)l.approved=true;
            s.sections={
                {
                    0,a.before
                },{
                    a.endM,a.after
                }
            };
            s.adapters.push_back(a);
        }
        return d;
    }
}
// namespace nexus
