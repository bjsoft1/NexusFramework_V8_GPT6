#include "NexusV8EditorSession.h"
#include "Editor.h"
#include "Misc/Paths.h"
#include "ScopedTransaction.h"
namespace { std::string Utf(const FString& S) { return TCHAR_TO_UTF8(*S); } }
nexus::Id FNexusV8EditorSession::NewId() { return Utf(FGuid::NewGuid().ToString(EGuidFormats::Digits).ToLower()); }
FNexusV8EditorSession::FNexusV8EditorSession()
{
    NewDocument(false);
    if(GEditor) GEditor->RegisterForUndo(this);
}
FNexusV8EditorSession::~FNexusV8EditorSession() { if(GEditor) GEditor->UnregisterForUndo(this); }
void FNexusV8EditorSession::ResetAsset()
{
    Asset.Reset(NewObject<UNexusV8DocumentAsset>(GetTransientPackage(), NAME_None, RF_Transactional|RF_Transient));
    FString Error; Asset->Write(Base,Error);
    Draft=Base; bStaleDraft=false; DiskHash.reset(); RecoveryHash.reset();
}
void FNexusV8EditorSession::NewDocument(bool bDemo)
{
    Base=nexus::makeExample([] { return NewId(); });
    if(bDemo) Source=nexus::makeExampleScan();
    else
    {
        Base.sourceWorldKey.clear(); Base.segments.clear();
        for(auto It=Base.entities.begin();It!=Base.entities.end();)
            if(It->second.kind>=nexus::Kind::Point) It=Base.entities.erase(It); else ++It;
        Source={};
    }
    Path.Empty(); ResetAsset(); Changed.Broadcast();
}
bool FNexusV8EditorSession::IsDirty() const { return nexus::contentFingerprint(Base)!=nexus::contentFingerprint(Draft); }
nexus::Issues FNexusV8EditorSession::Apply()
{
    if(bStaleDraft) return {{"STALE_DRAFT",Draft.id,"Undo/external state changed while a draft was open. Preserve recovery, then explicitly Revert or reconcile.",true}};
    auto Issues=nexus::validate(Draft);
    if(nexus::hasErrors(Issues)||!IsDirty()) return Issues;
    auto Candidate=Draft; Candidate.revision=Base.revision+1;
    FString Error;
    {
        const FScopedTransaction Transaction(NSLOCTEXT("NexusV8","ApplyDocument","Nexus V8 — Apply authoring changes"));
        Asset->Modify();
        if(!Asset->Write(Candidate,Error)) return {{"ASSET_WRITE",Draft.id,Utf(Error),true}};
    }
    Base=MoveTemp(Candidate); Draft=Base; Changed.Broadcast(); return Issues;
}
void FNexusV8EditorSession::Revert() { Draft=Base; bStaleDraft=false; Changed.Broadcast(); }
void FNexusV8EditorSession::Undo() { if(GEditor&&!IsDirty()) GEditor->UndoTransaction(); }
void FNexusV8EditorSession::Redo() { if(GEditor&&!IsDirty()) GEditor->RedoTransaction(); }
void FNexusV8EditorSession::PostUndo(bool bSuccess)
{
    if(!bSuccess||!Asset.Get()) return;
    nexus::Document Current; FString Error;
    if(!Asset->Read(Current,Error)) return;
    if(nexus::contentFingerprint(Current)==nexus::contentFingerprint(Base)) return;
    const bool bWasDirty=IsDirty(); Base=MoveTemp(Current);
    if(bWasDirty) bStaleDraft=true; else Draft=Base;
    Changed.Broadcast();
}
nexus::Issues FNexusV8EditorSession::Open(const FString& File)
{
    if(IsDirty()) return {{"DIRTY_DRAFT",Draft.id,"Apply/save or Revert the current draft before opening another document.",true}};
    nexus::Document Candidate;
    auto Issues=nexus::loadFile(Utf(File),Candidate); if(nexus::hasErrors(Issues)) return Issues;
    Base=MoveTemp(Candidate); Path=File; Source={}; ResetAsset();
    DiskHash=nexus::fileFingerprint(Utf(File));
    if(Base.sourceWorldKey=="Demo://NexusV8") Source=nexus::makeExampleScan();
    Changed.Broadcast(); return Issues;
}
nexus::Issues FNexusV8EditorSession::Save(const FString& File,bool bExplicitOverwrite)
{
    if(IsDirty()) return {{"DIRTY_DRAFT",Draft.id,"Apply or Revert the draft before saving committed data.",true}};
    const bool bNewPath=File!=Path;
    const auto Expected=bNewPath?(bExplicitOverwrite?nexus::fileFingerprint(Utf(File)):std::optional<std::string>{}):DiskHash;
    const auto Result=nexus::saveAtomic(Utf(File),Base,Expected);
    if(Result.saved){Path=File;DiskHash=Result.hash;RecoveryHash.reset();Changed.Broadcast();}
    return Result.issues;
}
FString FNexusV8EditorSession::RecoveryPath() const
{
    if(!Path.IsEmpty()) return Path+TEXT(".draft");
    return FPaths::ProjectSavedDir()/TEXT("NexusV8/Recovery")/(UTF8_TO_TCHAR(Base.id.c_str())+FString(TEXT(".draft")));
}
nexus::Issues FNexusV8EditorSession::SaveRecovery()
{
    if(!IsDirty()) return {};
    const auto Result=nexus::saveDraftAtomic(Utf(RecoveryPath()),Base,Draft,RecoveryHash);
    if(Result.saved) RecoveryHash=Result.hash;
    return Result.issues;
}
nexus::Issues FNexusV8EditorSession::Recover()
{
    if(IsDirty()) return {{"DIRTY_DRAFT",Draft.id,"Revert the current draft before recovering another.",true}};
    nexus::Document Candidate;
    auto Issues=nexus::recoverDraft(Utf(RecoveryPath()),Base,Candidate);
    if(!nexus::hasErrors(Issues)){Draft=MoveTemp(Candidate);RecoveryHash=nexus::fileFingerprint(Utf(RecoveryPath()));Changed.Broadcast();}
    return Issues;
}
