#pragma once
#include "CoreMinimal.h"
#include "SEditorViewport.h"
#include "EditorViewportClient.h"
#include "Domain/NexusDomain.h"
class FNexusV8EditorSession;
class SNexusV8Viewport : public SEditorViewport
{
public:
    SLATE_BEGIN_ARGS(SNexusV8Viewport) {} SLATE_END_ARGS()
    void Construct(const FArguments&, TSharedRef<FNexusV8EditorSession> InSession);
    virtual ~SNexusV8Viewport() override;
    nexus::PreviewSnapshot Snapshot;
    nexus::Id Selected;
    bool bShowLanes=true, bShowSource=true;
    TFunction<void(const nexus::Id&)> OnSelectPoint;
    void FocusNetwork();
    void TopView();
    void PerspectiveView();
    void Refresh();
    TSharedPtr<FNexusV8EditorSession> Session;
protected:
    // UE 5.8 finalizes MakeViewportToolbar; use the base implementation.
    TSharedPtr<FEditorViewportClient> RoadClient;
    virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;
};
