#include "GameFramework/Actor.h"
#include "SNexusV8Editor.h"
#include "SNexusV8Viewport.h"
#include "NexusV8SourceReader.h"
#include "Editor.h"
#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/DefaultValueHelper.h"
#include "Misc/MessageDialog.h"
#include "Misc/Paths.h"
#include "Misc/ScopedSlowTask.h"
#include "Styling/AppStyle.h"
#include "Brushes/SlateColorBrush.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SSplitter.h"
#include "Widgets/Layout/SWrapBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "LandscapeSplineControlPoint.h"
#include "LandscapeSplineSegment.h"
#include "LandscapeSplinesComponent.h"
#include <exception>
namespace
{
FString F(const std::string& S) { return UTF8_TO_TCHAR(S.c_str()); }
std::string U(const FString& S) { return TCHAR_TO_UTF8(*S); }
FString Kind(nexus::Kind K)
{
    static const TCHAR* Names[]={TEXT("World / Root"),TEXT("State"),TEXT("City"),TEXT("Highway"),TEXT("Highway Point"),TEXT("Place")};
    return Names[FMath::Clamp(int32(K),0,5)];
}
FString IssueText(const nexus::Issues& Items)
{
    FString Out;
    for(const auto& I:Items) Out+=(I.blocking?TEXT("ERROR  "):TEXT("NOTE   "))+F(I.code)+TEXT("  ")+F(I.message)+TEXT("\n");
    return Out;
}
}
void SNexusV8Editor::Construct(const FArguments&)
{
    Session=MakeShared<FNexusV8EditorSession>();
    Session->Changed.AddSP(this,&SNexusV8Editor::Changed);
    SetTheme();
    Notice=TEXT("Phase 1–2 authoring candidate. Start with Demo, or open a level and scan Landscape splines.");
    MapHandle=FEditorDelegates::MapChange.AddSP(this,&SNexusV8Editor::OnMapChanged);
    PropertyHandle=FCoreUObjectDelegates::OnObjectPropertyChanged.AddSP(this,&SNexusV8Editor::OnObjectChanged);
    TSharedRef<SWrapBox> Toolbar=SNew(SWrapBox).UseAllottedSize(true);
    auto Tool=[&](FString Name,FString Hint,TFunction<void()> Action,FName Icon=NAME_None)
    { Toolbar->AddSlot().Padding(2)[Button(Name,Hint,MoveTemp(Action),Icon)]; };
    Tool(TEXT("New"),TEXT("New isolated semantic document; does not alter Landscape."),[this]{if(ConfirmDiscard()){Renderer.Clear();Session->NewDocument(false);Selected.clear();SelectedSegment.clear();}},"Icons.Plus");
    Tool(TEXT("Demo"),TEXT("Load a synthetic 6 → 4 → 2 fixture, not a scan of your Landscape."),[this]{if(ConfirmDiscard()){Renderer.Clear();Session->NewDocument(true);Selected.clear();SelectedSegment.clear();bCatalogMeshes=false;Viewport->FocusNetwork();Notice=TEXT("Synthetic 320 m fixture loaded. Click Build preview.");}},"Icons.Play");
    Tool(TEXT("Open"),TEXT("Open a versioned .nv8 document."),[this]{Open();},"Icons.FolderOpen");
    Tool(TEXT("Save"),TEXT("Save applied data with revision check and backup. Ctrl+S."),[this]{Save();},"Icons.Save");
    Tool(TEXT("Save as"),TEXT("Save to a new .nv8 file."),[this]{Save(true);});
    Tool(TEXT("Scan"),TEXT("Read-only scan → review → accept into draft → Apply."),[this]{ReviewScan();},"Icons.Search");
    Tool(TEXT("Build preview"),TEXT("Build from applied semantics and a fresh source scan. Never production-publishable."),[this]{BuildPreview();},"Icons.Build");
    Tool(TEXT("Apply"),TEXT("Commit draft in one native Unreal transaction. Ctrl+Enter."),[this]{Report(Session->Apply(),TEXT("Changes applied as one Unreal transaction. Save to persist."));},"Icons.Check");
    Tool(TEXT("Revert"),TEXT("Discard draft only; does not undo the previous applied action."),[this]{Session->Revert();Notice=TEXT("Draft reverted.");});
    Tool(TEXT("Undo"),TEXT("Native Unreal undo. Apply/Revert draft first. Ctrl+Z."),[this]{if(Session->IsDirty())Notice=TEXT("Apply or Revert draft before Undo.");else Session->Undo();},"Icons.Undo");
    Tool(TEXT("Redo"),TEXT("Native Unreal redo. Ctrl+Y."),[this]{if(Session->IsDirty())Notice=TEXT("Apply or Revert draft before Redo.");else Session->Redo();},"Icons.Redo");
    Tool(TEXT("Theme"),TEXT("Switch this toolkit between dark/light; does not modify the global UE theme."),[this]{bLight=!bLight;SetTheme();bRefresh=true;});
    Tool(TEXT("Advanced"),TEXT("Show IDs, source bindings, catalogs and additional configuration."),[this]{bAdvanced=!bAdvanced;bRefresh=true;});
    TSharedRef<SHorizontalBox> Workspaces=SNew(SHorizontalBox);
    const TCHAR* Names[]={TEXT("World"),TEXT("Road & lanes"),TEXT("Point features"),TEXT("Places"),TEXT("AI routes"),TEXT("Verify")};
    for(int32 I=0;I<6;++I) Workspaces->AddSlot().AutoWidth().Padding(2)[Button(Names[I],I>=2&&I<=4?TEXT("Phase 3–4 workspace shell; only identity information is available now."):TEXT("Open workspace."),[this,I]{Workspace=I;bRefresh=true;})];
    ChildSlot
    [
        SNew(SBorder).BorderImage(FAppStyle::GetBrush("WhiteBrush")).BorderBackgroundColor_Lambda([this]{return Panel();}).Padding(4)
        [
            SNew(SVerticalBox)
            +SVerticalBox::Slot().AutoHeight()[Toolbar]
            +SVerticalBox::Slot().AutoHeight()[Workspaces]
            +SVerticalBox::Slot().FillHeight(1).Padding(0,5)
            [
                SNew(SSplitter)
                +SSplitter::Slot().Value(0.18f).MinSize(180)
                [
                    SNew(SVerticalBox)
                    +SVerticalBox::Slot().AutoHeight().Padding(5)[Label(TEXT("WORLD HIERARCHY"))]
                    +SVerticalBox::Slot().AutoHeight().Padding(3)[SNew(SSearchBox).HintText(FText::FromString(TEXT("Filter hierarchy"))).OnTextChanged_Lambda([this](const FText& T){Search=T.ToString();bRefresh=true;})]
                    +SVerticalBox::Slot().FillHeight(1)
                    [
                        SAssignNew(Tree,STreeView<TSharedPtr<FNexusTreeItem>>).TreeItemsSource(&Roots)
                        .SelectionMode(ESelectionMode::Single)
                        .OnGenerateRow_Lambda([this](TSharedPtr<FNexusTreeItem> Item,const TSharedRef<STableViewBase>& Owner)
                        {
                            auto It=Session->Draft.entities.find(Item->Id);
                            const FString Name=It==Session->Draft.entities.end()?TEXT("Unavailable"):F(It->second.name);
                            return SNew(STableRow<TSharedPtr<FNexusTreeItem>>,Owner).Padding(3)[Label(Name)];
                        })
                        .OnGetChildren_Lambda([](TSharedPtr<FNexusTreeItem> Item,TArray<TSharedPtr<FNexusTreeItem>>& Out){Out.Append(Item->Children);})
                        .OnSelectionChanged_Lambda([this](TSharedPtr<FNexusTreeItem> Item,ESelectInfo::Type){if(Item)Select(Item->Id);})
                    ]
                    +SVerticalBox::Slot().AutoHeight().Padding(3)
                    [SNew(SHorizontalBox)
                        +SHorizontalBox::Slot().FillWidth(1)[Button(TEXT("+ Child"),TEXT("Add the next allowed ownership level."),[this]{AddChild();},"Icons.Plus")]
                        +SHorizontalBox::Slot().AutoWidth()[Button(TEXT("Delete"),TEXT("Review dependencies before deletion."),[this]{DeleteSelected();},"Icons.Delete")]
                    ]
                ]
                +SSplitter::Slot().Value(0.56f).MinSize(300)
                [
                    SNew(SVerticalBox)
                    +SVerticalBox::Slot().AutoHeight()
                    [SNew(SHorizontalBox)
                        +SHorizontalBox::Slot().AutoWidth()[Button(TEXT("Perspective"),TEXT("Real Unreal scene viewport; right-mouse/WASD camera."),[this]{Viewport->PerspectiveView();})]
                        +SHorizontalBox::Slot().AutoWidth()[Button(TEXT("Top"),TEXT("Orthographic top view."),[this]{Viewport->TopView();})]
                        +SHorizontalBox::Slot().AutoWidth()[Button(TEXT("Frame"),TEXT("Frame the current road network."),[this]{Viewport->FocusNetwork();})]
                        +SHorizontalBox::Slot().AutoWidth()[Button(TEXT("Lane overlay"),TEXT("Toggle compiled preview corridors."),[this]{Viewport->bShowLanes=!Viewport->bShowLanes;Viewport->Refresh();})]
                    ]
                    +SVerticalBox::Slot().FillHeight(1)[SAssignNew(Viewport,SNexusV8Viewport,Session.ToSharedRef())]
                    +SVerticalBox::Slot().AutoHeight().Padding(4)[SNew(STextBlock).ColorAndOpacity_Lambda([this]{return Ink();}).Text_Lambda([this]{return FText::FromString(FString::Printf(TEXT("%s  |  %d segments  |  %s"),Session->Source.worldKey=="Demo://NexusV8"?TEXT("SYNTHETIC FIXTURE"):TEXT("LANDSCAPE SOURCE"),int32(Session->Draft.segments.size()),bSourceDirty?TEXT("SOURCE STALE — RESCAN"):TEXT("Preview only — not runtime output")));})]
                ]
                +SSplitter::Slot().Value(0.26f).MinSize(260)
                [SNew(SScrollBox)+SScrollBox::Slot()[SAssignNew(DetailsHost,SBox)]]
            ]
            +SVerticalBox::Slot().AutoHeight().Padding(5)
            [SNew(STextBlock).AutoWrapText(true).ColorAndOpacity_Lambda([this]{return Ink();}).Text_Lambda([this]{return FText::FromString((Session->IsDirty()?TEXT("DRAFT • "):TEXT("APPLIED • "))+Notice);})]
        ]
    ];
    Viewport->OnSelectPoint=[this](const nexus::Id& Id){Select(Id);};
    Changed();
}
SNexusV8Editor::~SNexusV8Editor()
{
    if(Session) Session->SaveRecovery();
    FEditorDelegates::MapChange.Remove(MapHandle);
    FCoreUObjectDelegates::OnObjectPropertyChanged.Remove(PropertyHandle);
    if(Session) Session->Changed.RemoveAll(this);
    Renderer.Clear();
}
void SNexusV8Editor::SetTheme()
{
    ButtonStyle=FAppStyle::Get().GetWidgetStyle<FButtonStyle>("Button");
    EditStyle=FAppStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("NormalEditableTextBox");
    const FLinearColor Back=bLight?FLinearColor(.88f,.90f,.92f):FLinearColor(.10f,.12f,.15f);
    ButtonStyle.SetNormal(FSlateColorBrush(Back));
    ButtonStyle.SetHovered(FSlateColorBrush(bLight?FLinearColor(.76f,.83f,.90f):FLinearColor(.16f,.24f,.31f)));
    ButtonStyle.SetPressed(FSlateColorBrush(FLinearColor(.17f,.37f,.49f)));
    ButtonStyle.SetNormalForeground(Ink()); ButtonStyle.SetHoveredForeground(Ink());
    EditStyle.SetBackgroundColor(Back); EditStyle.SetForegroundColor(Ink());
}
FSlateColor SNexusV8Editor::Ink() const { return bLight?FLinearColor(.05f,.07f,.09f):FLinearColor(.86f,.89f,.93f); }
FSlateColor SNexusV8Editor::Panel() const { return bLight?FLinearColor(.95f,.96f,.97f):FLinearColor(.055f,.066f,.082f); }
TSharedRef<SWidget> SNexusV8Editor::Label(const FString& Text,bool bMuted) const
{
    return SNew(STextBlock).Text(FText::FromString(Text)).AutoWrapText(true)
        .Font(FAppStyle::GetFontStyle("SmallFont"))
        .ColorAndOpacity_Lambda([this,bMuted]{return bMuted?FSlateColor(bLight?FLinearColor(.25f,.30f,.34f):FLinearColor(.58f,.65f,.71f)):Ink();});
}
TSharedRef<SWidget> SNexusV8Editor::Button(const FString& Text,const FString& Hint,TFunction<void()> Action,FName Icon)
{
    auto Content=SNew(SHorizontalBox);
    if(!Icon.IsNone()) Content->AddSlot().AutoWidth().VAlign(VAlign_Center).Padding(0,0,5,0)[SNew(SImage).Image(FAppStyle::GetBrush(Icon)).DesiredSizeOverride(FVector2D(14,14))];
    Content->AddSlot().AutoWidth().VAlign(VAlign_Center)[Label(Text)];
    return SNew(SButton).ButtonStyle(&ButtonStyle).ContentPadding(FMargin(7,5)).ToolTipText(FText::FromString(Hint))
        .OnClicked_Lambda([this,Action=MoveTemp(Action)]
        {
            try{Action();}catch(const std::exception& E){Report({{"ACTION_EXCEPTION",{},E.what(),true}});}
            return FReply::Handled();
        })[Content];
}
TSharedRef<SWidget> SNexusV8Editor::Choice(const FString& Current,const TArray<TPair<FString,FString>>& Options,TFunction<void(const FString&)> Set)
{
    return SNew(SComboButton).ButtonStyle(&ButtonStyle)
        .OnGetMenuContent_Lambda([Options,Set=MoveTemp(Set)]
        {
            FMenuBuilder Menu(true,nullptr);
            for(const auto& O:Options) Menu.AddMenuEntry(FText::FromString(O.Key),FText::FromString(O.Value),FSlateIcon(),FUIAction(FExecuteAction::CreateLambda([Set,Value=O.Value]{Set(Value);})));
            return Menu.MakeWidget();
        })
        .ButtonContent()[Label(Current)];
}
TSharedRef<SWidget> SNexusV8Editor::Edit(const FString& Value,TFunction<void(const FString&)> Set)
{
    return SNew(SEditableTextBox).Style(&EditStyle).Text(FText::FromString(Value))
        .OnTextCommitted_Lambda([Set=MoveTemp(Set)](const FText& Text,ETextCommit::Type Type){if(Type!=ETextCommit::OnCleared)Set(Text.ToString());});
}
void SNexusV8Editor::Heading(TSharedRef<SVerticalBox> Box,const FString& Text)
{ Box->AddSlot().AutoHeight().Padding(6,12,6,5)[Label(Text)]; }
void SNexusV8Editor::Row(TSharedRef<SVerticalBox> Box,const FString& Name,TSharedRef<SWidget> Value)
{
    Box->AddSlot().AutoHeight().Padding(6,3)
    [SNew(SVerticalBox)+SVerticalBox::Slot().AutoHeight().Padding(0,0,0,3)[Label(Name,true)]+SVerticalBox::Slot().AutoHeight()[Value]];
}
void SNexusV8Editor::Number(TSharedRef<SVerticalBox> Box,const FString& Name,double Value,TFunction<void(double)> Set)
{
    Row(Box,Name,Edit(FString::SanitizeFloat(Value),[this,Set=MoveTemp(Set)](const FString& Text)
    {double V=0;if(!FDefaultValueHelper::ParseDouble(Text, V)||!FMath::IsFinite(V)){Notice=TEXT("Enter a finite numeric value.");return;}Set(V);Changed();}));
}
void SNexusV8Editor::Check(TSharedRef<SVerticalBox> Box,const FString& Name,bool Value,TFunction<void(bool)> Set)
{
    Box->AddSlot().AutoHeight().Padding(6,5)[SNew(SCheckBox).IsChecked(Value?ECheckBoxState::Checked:ECheckBoxState::Unchecked)
        .OnCheckStateChanged_Lambda([Set=MoveTemp(Set)](ECheckBoxState V){Set(V==ECheckBoxState::Checked);})[Label(Name)]];
}
nexus::Entity* SNexusV8Editor::Entity()
{auto It=Session->Draft.entities.find(Selected);return It==Session->Draft.entities.end()?nullptr:&It->second;}
nexus::Id SNexusV8Editor::Highway() const
{
    auto It=Session->Draft.entities.find(Selected);std::set<nexus::Id> Seen;
    while(It!=Session->Draft.entities.end()&&Seen.insert(It->first).second)
    {if(It->second.kind==nexus::Kind::Highway)return It->first;It=Session->Draft.entities.find(It->second.parent);}
    const auto A=nexus::affectedHighways(Session->Draft,Selected); if(!A.empty())return *A.begin();
    for(const auto& P:Session->Draft.entities)if(P.second.kind==nexus::Kind::Highway)return P.first;
    return {};
}
void SNexusV8Editor::Changed()
{
    bRefresh=true; PendingDiff.reset();
    if(Viewport.IsValid()) { Viewport->Selected=Selected; Viewport->Refresh(); }
    InvalidatePreview();
}
void SNexusV8Editor::InvalidatePreview()
{
    if(Viewport.IsValid() && Viewport->Snapshot.documentFingerprint!=nexus::contentFingerprint(Session->Draft))
    {Viewport->Snapshot={};Renderer.SetStale(true);}
}
void SNexusV8Editor::Select(const nexus::Id& Id)
{Selected=Id;if(Viewport.IsValid()){Viewport->Selected=Id;Viewport->Refresh();}bRefresh=true;}
void SNexusV8Editor::RefreshTree()
{
    Roots.Reset();
    TMap<FString,TSharedPtr<FNexusTreeItem>> All;
    for(const auto& P:Session->Draft.entities){auto Item=MakeShared<FNexusTreeItem>();Item->Id=P.first;All.Add(F(P.first),Item);}
    for(const auto& P:Session->Draft.entities)
    {
        auto Item=All[F(P.first)];
        if(P.second.parent.empty())Roots.Add(Item);
        else if(auto* Parent=All.Find(F(P.second.parent)))(*Parent)->Children.Add(Item);
    }
    if(!Search.IsEmpty())
    {
        TFunction<bool(TSharedPtr<FNexusTreeItem>)> Filter=[&](TSharedPtr<FNexusTreeItem> Item)
        {bool Match=F(Session->Draft.entities.at(Item->Id).name).Contains(Search);for(int32 I=Item->Children.Num()-1;I>=0;--I)if(!Filter(Item->Children[I]))Item->Children.RemoveAt(I);return Match||Item->Children.Num()>0;};
        for(int32 I=Roots.Num()-1;I>=0;--I)if(!Filter(Roots[I]))Roots.RemoveAt(I);
    }
    Tree->RequestTreeRefresh();
    for(const auto& P:All)Tree->SetItemExpansion(P.Value,true);
    if(auto* Item=All.Find(F(Selected)))Tree->SetSelection(*Item,ESelectInfo::Direct);
}
void SNexusV8Editor::RefreshDetails()
{
    if(!Session->Draft.entities.count(Selected))
    {Selected=Highway();if(Selected.empty()&&!Session->Draft.entities.empty())Selected=Session->Draft.entities.begin()->first;}
    auto Box=SNew(SVerticalBox);
    auto* E=Entity();
    if(E)Heading(Box,F(E->name)+TEXT("  /  ")+Kind(E->kind));
    if(Workspace==0)WorldDetails(Box);
    else if(Workspace==1)RoadDetails(Box);
    else if(Workspace==5)VerifyDetails(Box);
    else
    {
        Heading(Box,Workspace==4?TEXT("AI navigation — Phase 4"):TEXT("Specialized tools — Phase 3"));
        Row(Box,TEXT("Implementation boundary"),Label(TEXT("This is an explicit future-workspace shell. Phase 1–2 supplies identities, profiles and lane corridor previews, not traffic simulation or service-specific gizmos.")));
        if(E&&E->kind==nexus::Kind::Point)PointDetails(Box);
        if(E&&E->kind==nexus::Kind::Place)PlaceDetails(Box);
    }
    if(!Issues.empty())
    {
        Heading(Box,TEXT("Validation / last operation"));
        int32 Count=0;for(const auto& I:Issues)
        {
            if(++Count>30){Row(Box,TEXT("More issues"),Label(TEXT("Use Export report in Verify to inspect every diagnostic.")));break;}
            Row(Box,F(I.code),Button(F(I.message),TEXT("Select related record when available."),[this,Id=I.entity]{if(Session->Draft.entities.count(Id))Select(Id);else if(Session->Draft.segments.count(Id)){SelectedSegment=Id;Workspace=1;bRefresh=true;}}));
        }
    }
    DetailsHost->SetContent(Box);
}
void SNexusV8Editor::Report(nexus::Issues NewIssues,const FString& Success)
{
    Issues=MoveTemp(NewIssues);Notice=nexus::hasErrors(Issues)?IssueText(Issues):Success;bRefresh=true;
}
void SNexusV8Editor::Tick(const FGeometry& G,double Now,float Delta)
{
    SCompoundWidget::Tick(G,Now,Delta);
    if(bRefresh){bRefresh=false;RefreshTree();RefreshDetails();bRefresh=false;}
    if(Now>=NextRecovery)
    {
        NextRecovery=Now+10;
        if(Session->IsDirty())
        {
            const auto Hash=nexus::contentFingerprint(Session->Draft);
            if(Hash!=LastRecoveryFingerprint)
            {auto Result=Session->SaveRecovery();if(!nexus::hasErrors(Result))LastRecoveryFingerprint=Hash;else{Notice=TEXT("Recovery not written: ")+IssueText(Result);}}
        }
    }
}
FReply SNexusV8Editor::OnKeyDown(const FGeometry& Geometry,const FKeyEvent& Event)
{
    if(Event.IsControlDown())
    {
        if(Event.GetKey()==EKeys::S){Save();return FReply::Handled();}
        if(Event.GetKey()==EKeys::Enter){Report(Session->Apply(),TEXT("Applied."));return FReply::Handled();}
        if(Event.GetKey()==EKeys::Z){if(!Session->IsDirty())Session->Undo();else Notice=TEXT("Apply/Revert draft before Undo.");return FReply::Handled();}
        if(Event.GetKey()==EKeys::Y){if(!Session->IsDirty())Session->Redo();else Notice=TEXT("Apply/Revert draft before Redo.");return FReply::Handled();}
    }
    return SCompoundWidget::OnKeyDown(Geometry,Event);
}
bool SNexusV8Editor::ConfirmDiscard()
{
    if(!Session->IsDirty()&&Session->Base.revision==0)return true;
    const bool Proceed=FMessageDialog::Open(EAppMsgType::YesNo,FText::FromString(TEXT("Leave this document? Save applied changes first. A draft recovery will be attempted, but cannot replace an intentional save.")))==EAppReturnType::Yes;
    if(Proceed&&Session->IsDirty()){
        auto Errors=Session->SaveRecovery();
        if(nexus::hasErrors(Errors)){Report(MoveTemp(Errors));Notice=TEXT("Recovery failed. Explicitly Revert or fix/save the draft before leaving.");return false;}
    }
    return Proceed;
}
void SNexusV8Editor::Open()
{
    if(!ConfirmDiscard())return;
    if(Session->IsDirty()){Session->SaveRecovery();Session->Revert();}
    IDesktopPlatform* Desktop=FDesktopPlatformModule::Get();if(!Desktop)return;
    TArray<FString> Files;
    if(Desktop->OpenFileDialog(FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr),TEXT("Open Nexus V8 document"),FPaths::ProjectSavedDir(),TEXT(""),TEXT("Nexus V8|*.nv8"),0,Files)&&Files.Num())
    {auto Result=Session->Open(Files[0]);if(!nexus::hasErrors(Result)){Renderer.Clear();Viewport->Snapshot={};Selected.clear();SelectedSegment.clear();bSourceDirty=false;}Report(MoveTemp(Result),TEXT("Document opened. Scan Landscape before previewing a non-demo document."));}
}
void SNexusV8Editor::Save(bool bSaveAs)
{
    if(Session->IsDirty()){Notice=TEXT("Click Apply or Revert before Save.");return;}
    FString File=Session->Path;
    if(File.IsEmpty()||bSaveAs)
    {
        IDesktopPlatform* Desktop=FDesktopPlatformModule::Get();if(!Desktop)return;
        TArray<FString> Files;
        if(!Desktop->SaveFileDialog(FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr),TEXT("Save Nexus V8 document"),FPaths::ProjectSavedDir(),TEXT("NexusWorld.nv8"),TEXT("Nexus V8|*.nv8"),0,Files)||!Files.Num())return;
        File=Files[0];if(FPaths::GetExtension(File).IsEmpty())File+=TEXT(".nv8");
    }
    bool bOverwrite=false;
    if(File!=Session->Path&&IFileManager::Get().FileExists(*File))
    {bOverwrite=FMessageDialog::Open(EAppMsgType::YesNo,FText::FromString(TEXT("Replace this existing file after creating a backup?")))==EAppReturnType::Yes;if(!bOverwrite)return;}
    Report(Session->Save(File,bOverwrite),TEXT("Saved applied document with readback verification and backup: ")+File);
}
void SNexusV8Editor::AddChild(const FString& Type)
{
    auto* Parent=Entity();if(!Parent){Notice=TEXT("Select a parent.");return;}
    if(Parent->kind==nexus::Kind::Place){Notice=TEXT("Place is the last ownership level.");return;}
    if(Parent->kind==nexus::Kind::Highway){Notice=TEXT("Highway points must come from a reviewed Landscape scan. Click Scan.");return;}
    nexus::Entity E;E.id=FNexusV8EditorSession::NewId();E.parent=Parent->id;E.kind=nexus::Kind(int(Parent->kind)+1);
    E.name=U(E.kind==nexus::Kind::Place?(Type.IsEmpty()?TEXT("Parking"):Type):Kind(E.kind));
    if(E.kind==nexus::Kind::Place)
    {
        E.payload.type=E.name;
        for(const auto& P:Session->Draft.segments)if(P.second.startPoint==Parent->id||P.second.endPoint==Parent->id){E.anchorSegment=P.first;E.anchorAtEnd=P.second.endPoint==Parent->id;break;}
    }
    if(E.kind==nexus::Kind::Highway&&!Session->Draft.profiles.empty())E.defaultProfile=Session->Draft.profiles.begin()->first;
    auto Result=nexus::addEntity(Session->Draft,E);
    if(!nexus::hasErrors(Result)){Select(E.id);Changed();}Report(MoveTemp(Result),TEXT("Child added to draft. Apply to commit."));
}
void SNexusV8Editor::DeleteSelected()
{
    auto* E=Entity();if(!E)return;
    const auto Children=nexus::descendants(Session->Draft,Selected);
    const FString Prompt=FString::Printf(TEXT("Delete '%s' and %d descendants from the draft? Segment/topology references must be repaired; invalid deletion is blocked. The applied document is unchanged until Apply."),*F(E->name),int32(Children.size()));
    if(FMessageDialog::Open(EAppMsgType::YesNo,FText::FromString(Prompt))!=EAppReturnType::Yes)return;
    auto Result=nexus::eraseEntity(Session->Draft,Selected,true);
    if(!nexus::hasErrors(Result)){Selected.clear();Changed();}Report(MoveTemp(Result),TEXT("Deletion staged. Apply to commit."));
}
void SNexusV8Editor::Reparent(const nexus::Id& Parent)
{
    auto* E=Entity();if(!E)return;
    auto Candidate=Session->Draft;
    if(E->kind==nexus::Kind::Place)
    {
        // Preserve-local is an explicit Phase-1 identity operation; full 3D gizmo reparent modes are Phase 3.
        if(FMessageDialog::Open(EAppMsgType::YesNo,FText::FromString(TEXT("Reparent this place preserving its local offsets? Its world placement may change; the approach anchor must be selected again.")))!=EAppReturnType::Yes)return;
        Candidate.entities.at(Selected).anchorSegment.clear();
    }
    auto Result=nexus::reparentEntity(Candidate,Selected,Parent);
    if(!nexus::hasErrors(Result)){Session->Draft=MoveTemp(Candidate);Changed();}Report(MoveTemp(Result),TEXT("Reparent staged; IDs retained."));
}
void SNexusV8Editor::ReviewScan()
{
    const auto H=Highway();if(H.empty()){Notice=TEXT("Create/select a highway first.");return;}
    if(Session->Draft.sourceWorldKey=="Demo://NexusV8")
    {PendingScan=nexus::makeExampleScan();Issues.clear();}
    else {Issues.clear();PendingScan=FNexusV8SourceReader::Scan(GEditor?GEditor->GetEditorWorldContext().World():nullptr,bSelectedOnly,Issues);}
    if(nexus::hasErrors(Issues)){Report(MoveTemp(Issues));return;}
    PendingDiff=nexus::diffScan(Session->Draft,PendingScan,H);
    Report(PendingDiff->issues,FString::Printf(TEXT("Scan is read-only. Review %d changes in World, then Accept scan → Apply."),int32(PendingDiff->changes.size())));
    Workspace=0;
}
void SNexusV8Editor::AcceptScan()
{
    if(!PendingDiff)return;
    // Read again to detect source edits after the user opened the diff.
    if(PendingScan.worldKey!="Demo://NexusV8")
    {
        nexus::Issues Errors;
        auto Fresh=FNexusV8SourceReader::Scan(GEditor?GEditor->GetEditorWorldContext().World():nullptr,bSelectedOnly,Errors);
        if(nexus::hasErrors(Errors)||Fresh.fingerprint!=PendingScan.fingerprint){Report({{"STALE_SCAN",{},"Landscape changed since review. Scan again.",true}});return;}
    }
    auto Result=nexus::applyScan(Session->Draft,PendingScan,*PendingDiff,[]{return FNexusV8EditorSession::NewId();});
    if(!nexus::hasErrors(Result)){Session->Source=PendingScan;bSourceDirty=false;PendingDiff.reset();Changed();Viewport->FocusNetwork();}
    Report(MoveTemp(Result),TEXT("Reviewed source bindings staged. Apply commits semantics; Landscape was not modified."));
}
void SNexusV8Editor::BuildPreview()
{
    if(Session->IsDirty()){Notice=TEXT("Apply the draft before building a coherent preview.");return;}
    if(Session->Base.sourceWorldKey!="Demo://NexusV8")
    {
        nexus::Issues Errors;auto Fresh=FNexusV8SourceReader::Scan(GEditor?GEditor->GetEditorWorldContext().World():nullptr,bSelectedOnly,Errors);
        if(nexus::hasErrors(Errors)){Report(MoveTemp(Errors));return;}Session->Source=MoveTemp(Fresh);
    }
    FScopedSlowTask Task(3,FText::FromString(TEXT("Compile and stage Nexus V8 preview")));Task.MakeDialog(true);
    nexus::CompileOptions Options;Options.requireAssets=bCatalogMeshes;Options.canceled=[&Task]{return Task.ShouldCancel();};
    Task.EnterProgressFrame(1,FText::FromString(TEXT("Validate source, mesh configuration and lane mappings")));
    auto Result=nexus::compilePreview(Session->Base,Session->Source,[](const std::string& Asset){return LoadObject<UStaticMesh>(nullptr,*F(Asset))!=nullptr;},Options);
    if(!Result.success){Report(MoveTemp(Result.issues));return;}
    Task.EnterProgressFrame(1,FText::FromString(TEXT("Stage replacement without deleting existing preview")));
    FString Error;
    if(!Renderer.Replace(GEditor?GEditor->GetEditorWorldContext().World():nullptr,Session->Base,Session->Source,Result.snapshot,bCatalogMeshes,[&Task]{return Task.ShouldCancel();},Error))
    {Report({{"PREVIEW_STAGING",{},U(Error),true}});return;}
    Task.EnterProgressFrame(1);
    Viewport->Snapshot=MoveTemp(Result.snapshot);Viewport->FocusNetwork();Viewport->Refresh();bSourceDirty=false;
    Report(MoveTemp(Result.issues),TEXT("Preview built. Snapshot ")+Renderer.GetHash()+TEXT(". Collision and production publication are disabled."));
}
void SNexusV8Editor::ExportReport()
{
    const FString File=FPaths::ProjectSavedDir()/TEXT("NexusV8/LastEditorReport.txt");
    IFileManager::Get().MakeDirectory(*FPaths::GetPath(File),true);
    FString Text=TEXT("Nexus V8 Phase 1–2 editor diagnostic report\nNot an engine acceptance-test result.\n");
    Text+=TEXT("Document: ")+F(Session->Base.id)+TEXT("\nRevision: ")+FString::Printf(TEXT("%llu"),static_cast<unsigned long long>(Session->Base.revision));
    Text+=TEXT("\nSaved file: ")+Session->Path+TEXT("\nPreview hash: ")+Renderer.GetHash()+TEXT("\n")+IssueText(Issues);
    Notice=FFileHelper::SaveStringToFile(Text,*File)?TEXT("Report written: ")+File:TEXT("Report write failed.");
}
void SNexusV8Editor::OnMapChanged(uint32)
{Renderer.Clear();Session->Source={};PendingDiff.reset();if(Viewport.IsValid()){Viewport->Snapshot={};Viewport->Refresh();}bSourceDirty=true;bRefresh=true;Notice=TEXT("Map changed. Source handles and preview cleared; authored document retained. Rescan the matching map.");}
void SNexusV8Editor::OnObjectChanged(UObject* Object,FPropertyChangedEvent&)
{
    if(!Object||Session->Base.sourceWorldKey=="Demo://NexusV8")return;
    bool bRelevant=Object->IsA<ULandscapeSplineControlPoint>()||Object->IsA<ULandscapeSplineSegment>()||Object->IsA<ULandscapeSplinesComponent>();
    if(auto* Actor=Cast<AActor>(Object))bRelevant=bRelevant||Actor->FindComponentByClass<ULandscapeSplinesComponent>()!=nullptr;
    if(bRelevant){bSourceDirty=true;Renderer.SetStale(true);if(Viewport.IsValid()){Viewport->Snapshot={};Viewport->Refresh();}PendingDiff.reset();Notice=TEXT("Landscape source changed. Rescan → review → Apply before rebuilding.");bRefresh=true;}
}
