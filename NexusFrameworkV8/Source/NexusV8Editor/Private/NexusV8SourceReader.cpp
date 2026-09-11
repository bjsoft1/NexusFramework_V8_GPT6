#include "NexusV8SourceReader.h"
#include "Editor.h"
#include "Engine/Selection.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "LandscapeSplinesComponent.h"
#include "LandscapeSplineControlPoint.h"
#include "LandscapeSplineSegment.h"
#include <iomanip>
#include <sstream>
namespace
{
    std::string Utf8(const FString& S){return TCHAR_TO_UTF8(*S);}
    nexus::Vec3 Vec(const FVector& V){return {V.X/100.0,V.Y/100.0,V.Z/100.0};}
    nexus::Vec3 Direction(const FVector& V){return {V.X,V.Y,V.Z};}
    nexus::Quat Quat(const FQuat& Q){return {Q.X,Q.Y,Q.Z,Q.W};}
    std::string Key(const ULandscapeSplinesComponent* C,const UObject* Object)
    {return Utf8(C->GetPathName()+TEXT("#")+Object->GetName());}
    nexus::SourceEnd End(const ULandscapeSplinesComponent* C,const FLandscapeSplineSegmentConnection& Connection)
    {
        nexus::SourceEnd E;
        const ULandscapeSplineControlPoint* P=Connection.ControlPoint.Get();
        if(!P) return E;
        FVector Position; FRotator Rotation;
        P->GetConnectionLocationAndRotation(Connection.SocketName,Position,Rotation);
        const FTransform Transform=C->GetComponentTransform();
        E.pointKey=Key(C,P);
        E.signedTangentM=Connection.TangentLen/100.0;
        E.frame={Vec(Transform.TransformPosition(Position)),Quat(Transform.GetRotation()*Rotation.Quaternion())};
        E.tangent=Vec(Transform.TransformVector(Rotation.Vector()*Connection.TangentLen));
        return E;
    }
}
nexus::SourceScan FNexusV8SourceReader::Scan(UWorld* World,bool bSelectedActorsOnly,nexus::Issues& Issues)
{
    nexus::SourceScan Scan;
    if(!World){Issues.push_back({"NO_EDITOR_WORLD",{},"Open an editor level before scanning.",true});return Scan;}
    Scan.worldKey=Utf8(World->GetOutermost()->GetName());
    for(TActorIterator<AActor> Actor(World);Actor;++Actor)
    {
        if(bSelectedActorsOnly&&(!GEditor||!GEditor->GetSelectedActors()->IsSelected(*Actor)))continue;
        TArray<ULandscapeSplinesComponent*> Components;Actor->GetComponents(Components);
        for(const ULandscapeSplinesComponent* Component:Components)
        {
            if(!Component)continue;
            const std::string Scope=Utf8(Component->GetPathName());
            const FTransform Transform=Component->GetComponentTransform();
            Scan.completeScopes.insert(Scope);
            for(const auto& Pointer:Component->GetControlPoints())
            {
                const ULandscapeSplineControlPoint* Point=Pointer.Get();if(!Point)continue;
                nexus::SourcePoint P;P.key=Key(Component,Point);P.scope=Scope;
                P.frame={Vec(Transform.TransformPosition(Point->Location)),Quat(Transform.GetRotation()*Point->Rotation.Quaternion())};
                std::ostringstream Hash;Hash<<std::setprecision(17)<<P.key<<P.frame.position.x<<','<<P.frame.position.y<<','<<P.frame.position.z<<','<<P.frame.rotation.x<<','<<P.frame.rotation.y<<','<<P.frame.rotation.z<<','<<P.frame.rotation.w;
                P.fingerprint=nexus::fingerprint(Hash.str());Scan.points.emplace(P.key,std::move(P));
            }
            for(const auto& Pointer:Component->GetSegments())
            {
                ULandscapeSplineSegment* Segment=Pointer.Get();if(!Segment)continue;
                nexus::SourceSegment S;S.key=Key(Component,Segment);S.scope=Scope;
                S.start=End(Component,Segment->Connections[0]);S.end=End(Component,Segment->Connections[1]);
                if(S.start.pointKey.empty()||S.end.pointKey.empty()){Issues.push_back({"SOURCE_ENDPOINT",{},"Landscape segment has a null endpoint: "+S.key,true});continue;}
                const auto& Interp=Segment->GetPoints();
                if(Interp.Num()<2){Issues.push_back({"SOURCE_NOT_EVALUATED",{},"Landscape spline has no evaluated samples. Rebuild it in Landscape mode before scanning: "+S.key,true});continue;}
                double Station=0;FVector Last=FVector::ZeroVector;
                std::ostringstream Hash;Hash<<std::setprecision(17)<<S.key<<S.start.pointKey<<S.end.pointKey<<S.start.signedTangentM<<S.end.signedTangentM;
                for(int32 Index=0;Index<Interp.Num();++Index)
                {
                    const FVector Position=Transform.TransformPosition(Interp[Index].Center);
                    if(Index>0){const double Step=FVector::Distance(Last,Position)/100.0;if(Step<1e-7)continue;Station+=Step;}
                    const int32 A=FMath::Max(0,Index-1),B=FMath::Min(Interp.Num()-1,Index+1);
                    const FVector Forward=Transform.TransformVector(Interp[B].Center-Interp[A].Center).GetSafeNormal();
                    FVector Right=Transform.TransformVector(Interp[Index].Right-Interp[Index].Left).GetSafeNormal();
                    FVector Up=FVector::CrossProduct(Forward,Right).GetSafeNormal();
                    if(Up.IsNearlyZero()){Issues.push_back({"SOURCE_FRAME",{},"Degenerate spline frame/width: "+S.key,true});break;}
                    Right=FVector::CrossProduct(Up,Forward).GetSafeNormal();
                    Up=FVector::CrossProduct(Forward,Right).GetSafeNormal();
                    nexus::Sample Sample{Station,Vec(Position),Direction(Forward),Direction(Up)};
                    S.samples.push_back(Sample);Last=Position;
                    Hash<<Sample.stationM<<','<<Sample.position.x<<','<<Sample.position.y<<','<<Sample.position.z<<','<<Sample.forward.x<<','<<Sample.forward.y<<','<<Sample.forward.z<<','<<Sample.up.x<<','<<Sample.up.y<<','<<Sample.up.z<<';';
                }
                S.fingerprint=nexus::fingerprint(Hash.str());Scan.segments.emplace(S.key,std::move(S));
            }
        }
    }
    std::string All=Scan.worldKey;for(const auto& P:Scan.points)All+=P.second.fingerprint;for(const auto& S:Scan.segments)All+=S.second.fingerprint;
    Scan.fingerprint=nexus::fingerprint(All);
    if(Scan.segments.empty())Issues.push_back({"EMPTY_SCAN",{},"No loaded Landscape spline segments found. Select their Landscape/Spline actors or disable selected-only scanning.",false});
    return Scan;
}
