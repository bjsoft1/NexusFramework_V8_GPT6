#include "NexusV8PreviewRenderer.h"
#include "Components/SceneComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/Texture2D.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Materials/Material.h"
#include "Materials/MaterialExpressionTextureSample.h"
#include "Materials/MaterialExpressionVertexColor.h"
#include "ProceduralMeshComponent.h"
#include "Misc/ScopeExit.h"

namespace
{
FVector V(nexus::Vec3 P) { return FVector(P.x*100, P.y*100, P.z*100); }
FVector D(nexus::Vec3 P) { return FVector(P.x, P.y, P.z); }
FString Text(const std::string& S) { return UTF8_TO_TCHAR(S.c_str()); }
void DestroyActors(TArray<TWeakObjectPtr<AActor>>& Actors)
{
    for (const auto& Actor : Actors) if (Actor.IsValid()) Actor->Destroy();
    Actors.Reset();
}
UMaterial* Material(UObject* Outer, UTexture2D* Texture)
{
    UMaterial* M=NewObject<UMaterial>(Outer, NAME_None, RF_Transient);
    M->SetShadingModel(MSM_Unlit);
    M->TwoSided=false;
    if (Texture)
    {
        auto* E=NewObject<UMaterialExpressionTextureSample>(M);
        E->Texture=Texture; E->SamplerType=SAMPLERTYPE_Color;
        M->GetExpressionCollection().AddExpression(E);
        M->GetEditorOnlyData()->EmissiveColor.Connect(0, E);
    }
    else
    {
        auto* E=NewObject<UMaterialExpressionVertexColor>(M);
        M->GetExpressionCollection().AddExpression(E);
        M->GetEditorOnlyData()->EmissiveColor.Connect(0, E);
    }
    M->PostEditChange();
    return M;
}
UTexture2D* MarkingTexture(const nexus::RoadOutput& Road)
{
    UTexture2D* T=UTexture2D::CreateTransient(Road.textureWidth, Road.textureHeight, PF_B8G8R8A8);
    if (!T || !T->GetPlatformData()) return nullptr;
    const uint64 Needed=uint64(Road.textureWidth)*Road.textureHeight*4;
    if (Road.markingRGBA.size()!=Needed) return nullptr;
    T->SRGB=true; T->NeverStream=true;
    T->AddressX=TA_Clamp; T->AddressY=TA_Clamp;
    T->MipGenSettings=TMGS_NoMipmaps;
    auto& Mip=T->GetPlatformData()->Mips[0];
    uint8* Pixels=static_cast<uint8*>(Mip.BulkData.Lock(LOCK_READ_WRITE));
    for(uint64 I=0; I<Needed; I+=4)
    {
        Pixels[I]=Road.markingRGBA[I+2]; Pixels[I+1]=Road.markingRGBA[I+1];
        Pixels[I+2]=Road.markingRGBA[I]; Pixels[I+3]=Road.markingRGBA[I+3];
    }
    Mip.BulkData.Unlock(); T->UpdateResource();
    return T;
}
AActor* Spawn(UWorld* World, const nexus::PreviewSnapshot& Snapshot, const nexus::Id& Segment)
{
    FActorSpawnParameters Params;
    Params.ObjectFlags=RF_Transient|RF_DuplicateTransient;
    Params.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    AActor* Actor=World->SpawnActor<AActor>(AActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, Params);
    if(!Actor) return nullptr;
    Actor->bIsEditorOnlyActor=true;
    Actor->SetActorLabel(TEXT("Nexus V8 Preview — ")+Text(Segment.substr(24)));
    Actor->Tags.Add(TEXT("NexusV8.Preview"));
    Actor->Tags.Add(FName(*(TEXT("Document:")+Text(Snapshot.document))));
    Actor->Tags.Add(FName(*(TEXT("Snapshot:")+Text(Snapshot.contentHash))));
    Actor->SetActorHiddenInGame(true);
    Actor->SetIsTemporarilyHiddenInEditor(true);
    auto* Root=NewObject<USceneComponent>(Actor, TEXT("PreviewRoot"), RF_Transient);
    Actor->AddInstanceComponent(Root); Actor->SetRootComponent(Root); Root->RegisterComponent();
    return Actor;
}
bool Procedural(AActor* Actor, const nexus::RoadOutput& Road, FString& Error)
{
    auto* Component=NewObject<UProceduralMeshComponent>(Actor, NAME_None, RF_Transient);
    Component->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Component->SetCanEverAffectNavigation(false);
    Actor->AddInstanceComponent(Component); Component->SetupAttachment(Actor->GetRootComponent());
    Component->RegisterComponent();
    UTexture2D* Texture=MarkingTexture(Road);
    if(!Texture) { Error=TEXT("Could not allocate marking texture; old preview retained."); return false; }
    UMaterial* RoadMaterial=Material(Actor, Texture);
    UMaterial* PlainMaterial=Material(Actor, nullptr);
    int32 Section=0;
    for(const auto& Surface:Road.surfaces)
    {
        if(Surface.indices.empty()) continue;
        TArray<FVector> Vertices, Normals;
        TArray<FVector2D> UV;
        TArray<int32> Triangles;
        TArray<FLinearColor> Colors;
        TArray<FProcMeshTangent> Tangents;
        double Length=1;
        for(const auto& Vertex:Surface.vertices) Length=FMath::Max(Length, Vertex.v);
        const bool bRoad=Surface.role=="Road";
        const bool bBorder=Surface.role.find("Border")==0 || Surface.role.find("Curb")==0;
        const FLinearColor Color=bRoad?FLinearColor::White:(bBorder?FLinearColor(0.45f,0.46f,0.47f):FLinearColor(0.24f,0.27f,0.29f));
        for(const auto& Vertex:Surface.vertices)
        {
            Vertices.Add(V(Vertex.position)); Normals.Add(D(Vertex.normal));
            UV.Add(FVector2D(Vertex.u, bRoad?Vertex.v/Length:Vertex.v)); Colors.Add(Color);
        }
        for(auto Index:Surface.indices) Triangles.Add(static_cast<int32>(Index));
        Component->CreateMeshSection_LinearColor(Section, Vertices, Triangles, Normals, UV, Colors, Tangents, false);
        Component->SetMaterial(Section, bRoad?RoadMaterial:PlainMaterial); ++Section;
    }
    return true;
}
bool Catalog(AActor* Actor, const nexus::Document& Document, const nexus::SourceScan& Source,
    const nexus::PreviewSnapshot& Snapshot, const nexus::RoadOutput& Road, FString& Error)
{
    const auto& Segment=Document.segments.at(Road.segment);
    const auto& Curve=Source.segments.at(Segment.sourceKey);
    const auto& Meshes=Snapshot.meshes.at(Segment.id);
    auto Place=[&](const std::string& GenericSlot, const std::string& ExactSlot, double From, double To, bool bAdapter)->bool
    {
        auto It=Meshes.find(ExactSlot); if(It==Meshes.end()) It=Meshes.find(GenericSlot);
        if(It==Meshes.end() || !It->second.resolved) { Error=TEXT("No resolved mesh for ")+Text(GenericSlot); return false; }
        UStaticMesh* Mesh=LoadObject<UStaticMesh>(nullptr, *Text(It->second.entry.assetPath));
        if(!Mesh) { Error=TEXT("Mesh disappeared before staging: ")+Text(It->second.entry.assetPath); return false; }
        const auto& Contract=It->second.entry.interface;
        const FBox Bounds=Mesh->GetBoundingBox();
        if(!Bounds.IsValid || FMath::Abs(Bounds.Min.X)>1.0 || FMath::Abs(Bounds.GetSize().X-Contract.lengthM*100.0)>1.0)
        {
            Error=TEXT("Mesh X bounds do not match declared StartCenter pivot / length: ")+Text(It->second.entry.assetPath);
            return false;
        }
        const int32 Count=bAdapter?1:FMath::Max(1, FMath::CeilToInt((To-From)/Contract.lengthM));
        if(Count>10000) { Error=TEXT("Mesh instance safety cap exceeded."); return false; }
        for(int32 I=0; I<Count; ++I)
        {
            const double A=FMath::Lerp(From, To, double(I)/Count), B=FMath::Lerp(From, To, double(I+1)/Count);
            const auto Start=nexus::sampleSource(Curve,A), End=nexus::sampleSource(Curve,B);
            auto* C=NewObject<USplineMeshComponent>(Actor, NAME_None, RF_Transient);
            C->SetMobility(EComponentMobility::Movable);
            C->SetCollisionEnabled(ECollisionEnabled::NoCollision); C->SetCanEverAffectNavigation(false);
            Actor->AddInstanceComponent(C); C->SetupAttachment(Actor->GetRootComponent());
            C->SetStaticMesh(Mesh); C->SetForwardAxis(ESplineMeshAxis::X, false);
            C->SetBoundaryMin(0,false); C->SetBoundaryMax(Contract.lengthM*100.0,false);
            C->SetSplineUpDir(D(Start.up),false);
            const FVector RefUp=D(Start.up), EndForward=D(End.forward), EndUp=D(End.up);
            const FVector RefRight=FVector::CrossProduct(RefUp,EndForward).GetSafeNormal();
            const FVector ProjectedUp=FVector::CrossProduct(EndForward,RefRight).GetSafeNormal();
            C->SetStartRoll(0,false);
            C->SetEndRoll(FMath::Atan2(FVector::DotProduct(FVector::CrossProduct(ProjectedUp,EndUp),EndForward),FVector::DotProduct(ProjectedUp,EndUp)),false);
            C->SetStartAndEnd(V(Start.position),D(Start.forward)*(B-A)*100.0,V(End.position),D(End.forward)*(B-A)*100.0,false);
            C->RegisterComponent(); C->UpdateMesh();
        }
        return true;
    };
    auto RoadRange=[&](double A,double B)->bool
    {
        if(B-A<1e-7) return true;
        const auto* Profile=nexus::profileAt(Document,Segment,A);
        return Profile && Place("Road."+std::to_string(Profile->lanes.size()),"Road.Profile."+Profile->id,A,B,false);
    };
    double Cursor=0;
    for(const auto& Adapter:Segment.adapters)
    {
        if(!RoadRange(Cursor,Adapter.startM)) return false;
        const auto& A=Document.profiles.at(Adapter.before); const auto& B=Document.profiles.at(Adapter.after);
        if(!Place("Adapter."+std::to_string(A.lanes.size())+"-"+std::to_string(B.lanes.size()),"Adapter.Profile."+A.id+"-"+B.id,Adapter.startM,Adapter.endM,true)) return false;
        Cursor=Adapter.endM;
    }
    return RoadRange(Cursor,Curve.samples.back().stationM);
}
}
FNexusV8PreviewRenderer::~FNexusV8PreviewRenderer() { Clear(); }
void FNexusV8PreviewRenderer::Clear() { DestroyActors(Actors); Hash.Empty(); }
void FNexusV8PreviewRenderer::SetStale(bool bStale)
{
    for(const auto& A:Actors) if(A.IsValid()) A->SetIsTemporarilyHiddenInEditor(bStale);
}
bool FNexusV8PreviewRenderer::Replace(UWorld* World,const nexus::Document& Document,const nexus::SourceScan& Source,
    const nexus::PreviewSnapshot& Snapshot,bool bCatalogMeshes,const TFunction<bool()>& Canceled,FString& Error)
{
    if(!World || Snapshot.productionPublishable || Snapshot.documentFingerprint!=nexus::contentFingerprint(Document) || Snapshot.sourceFingerprint!=Source.fingerprint)
    { Error=TEXT("Invalid or stale Phase 1–2 preview input."); return false; }
    TArray<TWeakObjectPtr<AActor>> Staged;
    bool bPromoted=false;
    ON_SCOPE_EXIT { if(!bPromoted) DestroyActors(Staged); };
    for(const auto& Road:Snapshot.roads)
    {
        if(Canceled && Canceled()) { Error=TEXT("Canceled; prior preview retained."); return false; }
        AActor* Actor=Spawn(World,Snapshot,Road.segment);
        if(!Actor) { Error=TEXT("Preview staging could not spawn a transient actor."); return false; }
        Staged.Add(Actor);
        if(bCatalogMeshes ? !Catalog(Actor,Document,Source,Snapshot,Road,Error) : !Procedural(Actor,Road,Error)) return false;
    }
    if(Canceled && Canceled()) { Error=TEXT("Canceled before promotion; prior preview retained."); return false; }
    DestroyActors(Actors); Actors=MoveTemp(Staged);
    for(const auto& A:Actors) if(A.IsValid()) A->SetIsTemporarilyHiddenInEditor(false);
    Hash=Text(Snapshot.contentHash); bPromoted=true; return true;
}
