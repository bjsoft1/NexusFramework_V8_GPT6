#pragma once
#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/STreeView.h"
#include "Styling/SlateTypes.h"
#include "NexusV8EditorSession.h"
#include "NexusV8PreviewRenderer.h"
class SNexusV8Viewport;
class SVerticalBox;
class SBox;
struct FNexusTreeItem
{
    nexus::Id Id;
    TArray<TSharedPtr<FNexusTreeItem>> Children;
};
class SNexusV8Editor : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SNexusV8Editor) {} SLATE_END_ARGS()
    void Construct(const FArguments&);
    virtual ~SNexusV8Editor() override;
    virtual bool SupportsKeyboardFocus() const override { return true; }
    virtual FReply OnKeyDown(const FGeometry&,const FKeyEvent&) override;
    virtual void Tick(const FGeometry&,double,float) override;
private:
    TSharedPtr<FNexusV8EditorSession> Session;
    FNexusV8PreviewRenderer Renderer;
    TSharedPtr<SNexusV8Viewport> Viewport;
    TSharedPtr<STreeView<TSharedPtr<FNexusTreeItem>>> Tree;
    TSharedPtr<SBox> DetailsHost;
    TArray<TSharedPtr<FNexusTreeItem>> Roots;
    nexus::Id Selected, SelectedSegment, SelectedProfile;
    std::string SelectedSlot;
    int32 Workspace=0;
    bool bLight=false,bAdvanced=false,bSelectedOnly=false,bCatalogMeshes=false,bRefresh=true;
    bool bSourceDirty=false;
    double NextRecovery=0;
    std::string LastRecoveryFingerprint;
    FString Search, Notice;
    nexus::Issues Issues;
    std::optional<nexus::ScanDiff> PendingDiff;
    nexus::SourceScan PendingScan;
    FDelegateHandle MapHandle, PropertyHandle;
    FButtonStyle ButtonStyle;
    FEditableTextBoxStyle EditStyle;

    void SetTheme();
    FSlateColor Ink() const;
    FSlateColor Panel() const;
    TSharedRef<SWidget> Label(const FString&,bool bMuted=false) const;
    TSharedRef<SWidget> Button(const FString&,const FString&,TFunction<void()>,FName Icon=NAME_None);
    TSharedRef<SWidget> Choice(const FString&,const TArray<TPair<FString,FString>>&,TFunction<void(const FString&)>);
    TSharedRef<SWidget> Edit(const FString&,TFunction<void(const FString&)>);
    void Heading(TSharedRef<SVerticalBox>,const FString&);
    void Row(TSharedRef<SVerticalBox>,const FString&,TSharedRef<SWidget>);
    void Number(TSharedRef<SVerticalBox>,const FString&,double,TFunction<void(double)>);
    void Check(TSharedRef<SVerticalBox>,const FString&,bool,TFunction<void(bool)>);
    void Changed();
    void Select(const nexus::Id&);
    void RefreshDetails();
    void RefreshTree();
    void Report(nexus::Issues,const FString& Success=TEXT("Done."));
    void InvalidatePreview();
    nexus::Id Highway() const;
    nexus::Entity* Entity();
    void AddChild(const FString& Type=TEXT(""));
    void DeleteSelected();
    void Reparent(const nexus::Id&);
    void Open();
    void Save(bool bSaveAs=false);
    bool ConfirmDiscard();
    void ReviewScan();
    void AcceptScan();
    void BuildPreview();
    void ExportReport();
    void WorldDetails(TSharedRef<SVerticalBox>);
    void RoadDetails(TSharedRef<SVerticalBox>);
    void MeshDetails(TSharedRef<SVerticalBox>);
    void VerifyDetails(TSharedRef<SVerticalBox>);
    void PointDetails(TSharedRef<SVerticalBox>);
    void PlaceDetails(TSharedRef<SVerticalBox>);
    void OnMapChanged(uint32);
    void OnObjectChanged(UObject*,FPropertyChangedEvent&);
};
