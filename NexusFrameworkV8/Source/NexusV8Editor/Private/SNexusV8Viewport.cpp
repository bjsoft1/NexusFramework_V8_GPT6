#include "SNexusV8Viewport.h"
#include "NexusV8EditorSession.h"
#include "Editor.h"
#include "Engine/World.h"
#include "HitProxies.h"
#include "SceneManagement.h"
#include "Widgets/SNullWidget.h"
struct HNexusV8Point : public HHitProxy
{
    DECLARE_HIT_PROXY();
    nexus::Id Id;
    explicit HNexusV8Point(nexus::Id In) : HHitProxy(HPP_UI), Id(MoveTemp(In)) {}
};
IMPLEMENT_HIT_PROXY(HNexusV8Point,HHitProxy);
namespace
{
FVector V(nexus::Vec3 P) { return {P.x*100,P.y*100,P.z*100}; }
class FNexusViewportClient : public FEditorViewportClient
{
    TWeakPtr<SNexusV8Viewport> Owner;
public:
    explicit FNexusViewportClient(TSharedRef<SNexusV8Viewport> Widget)
        :FEditorViewportClient(nullptr,nullptr,Widget),Owner(Widget)
    {
        SetViewMode(VMI_Unlit); SetRealtime(true); bSetListenerPosition=false;
        SetViewLocation(FVector(14000,-20000,22000)); SetViewRotation(FRotator(-48,90,0));
        EngineShowFlags.SetSelectionOutline(false);
    }
    virtual UWorld* GetWorld() const override { return GEditor?GEditor->GetEditorWorldContext().World():nullptr; }
    virtual void Draw(const FSceneView* View,FPrimitiveDrawInterface* PDI) override
    {
        FEditorViewportClient::Draw(View,PDI);
        auto W=Owner.Pin(); if(!W||!W->Session) return;
        const auto& Source=W->Session->Source;
        if(W->bShowSource)
        {
            for(const auto& Pair:Source.segments)
            {
                const auto& Points=Pair.second.samples;
                for(std::size_t I=1; I<Points.size(); ++I)
                    PDI->DrawLine(V(Points[I-1].position)+FVector(0,0,5),V(Points[I].position)+FVector(0,0,5),FLinearColor(0.7f,0.45f,0.1f),SDPG_World,1);
            }
            for(const auto& Pair:W->Session->Draft.entities)
            {
                const auto& E=Pair.second; if(E.kind!=nexus::Kind::Point) continue;
                auto It=Source.points.find(E.sourceKey); if(It==Source.points.end()) continue;
                PDI->SetHitProxy(new HNexusV8Point(E.id));
                PDI->DrawPoint(V(It->second.frame.position)+FVector(0,0,30),E.id==W->Selected?FLinearColor::Yellow:FLinearColor(0.15f,0.75f,1.0f),E.id==W->Selected?18:12,SDPG_Foreground);
                PDI->SetHitProxy(nullptr);
            }
        }
        if(W->bShowLanes)
        {
            // Diagnostic cap only. It does not remove data from the compiled snapshot.
            int32 Lines=0;
            for(const auto& Road:W->Snapshot.roads) for(const auto& Lane:Road.corridors)
            {
                const FLinearColor C=Lane.direction==nexus::Direction::Forward?FLinearColor(0.1f,0.85f,0.75f):FLinearColor(0.55f,0.4f,1.0f);
                for(std::size_t I=1; I<Lane.samples.size() && Lines<20000; ++I,++Lines)
                    PDI->DrawLine(V(Lane.samples[I-1].center)+FVector(0,0,12),V(Lane.samples[I].center)+FVector(0,0,12),C,SDPG_World,1.5f);
                if(!Lane.samples.empty())
                {
                    const auto& S=Lane.samples[Lane.samples.size()/2];
                    PDI->DrawPoint(V(S.center)+FVector(0,0,12),C,4,SDPG_World);
                }
            }
        }
    }
    virtual void ProcessClick(FSceneView& View,HHitProxy* Proxy,FKey Key,EInputEvent Event,uint32 X,uint32 Y) override
    {
        auto W=Owner.Pin();
        if(W&&Proxy&&Proxy->IsA(HNexusV8Point::StaticGetType()))
        {
            W->Selected=static_cast<HNexusV8Point*>(Proxy)->Id;
            if(W->OnSelectPoint) W->OnSelectPoint(W->Selected);
            Invalidate();
        }
        // This viewport is a read-only source view. Native Landscape editing remains in Landscape Mode.
    }
};
}
void SNexusV8Viewport::Construct(const FArguments&,TSharedRef<FNexusV8EditorSession> InSession)
{
    Session=InSession; SEditorViewport::Construct(SEditorViewport::FArguments());
}
SNexusV8Viewport::~SNexusV8Viewport() { if(RoadClient.IsValid()) RoadClient->Viewport=nullptr; }
TSharedRef<FEditorViewportClient> SNexusV8Viewport::MakeEditorViewportClient()
{ RoadClient=MakeShared<FNexusViewportClient>(SharedThis(this)); return RoadClient.ToSharedRef(); }

void SNexusV8Viewport::Refresh() { if(RoadClient.IsValid()) RoadClient->Invalidate(); }
void SNexusV8Viewport::TopView() { if(RoadClient.IsValid()){RoadClient->SetViewportType(LVT_OrthoXY);RoadClient->SetOrthoZoom(40000);RoadClient->Invalidate();} }
void SNexusV8Viewport::PerspectiveView() { if(RoadClient.IsValid()){RoadClient->SetViewportType(LVT_Perspective);RoadClient->Invalidate();} }
void SNexusV8Viewport::FocusNetwork()
{
    FBox Bounds(ForceInit);
    for(const auto& P:Session->Source.points) Bounds+=V(P.second.frame.position);
    if(RoadClient.IsValid()&&Bounds.IsValid) RoadClient->FocusViewportOnBox(Bounds.ExpandBy(1500));
}
