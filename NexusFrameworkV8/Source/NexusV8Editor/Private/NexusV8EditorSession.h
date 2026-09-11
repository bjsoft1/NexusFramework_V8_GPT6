#pragma once
#include "CoreMinimal.h"
#include "EditorUndoClient.h"
#include "UObject/StrongObjectPtr.h"
#include "NexusV8DocumentAsset.h"
#include "Domain/NexusDomain.h"

class FNexusV8EditorSession : public FEditorUndoClient
{
public:
    FNexusV8EditorSession();
    virtual ~FNexusV8EditorSession() override;
    nexus::Document Base, Draft;
    nexus::SourceScan Source;
    FString Path;
    FSimpleMulticastDelegate Changed;
    bool bStaleDraft=false;
    bool IsDirty() const;
    nexus::Issues Apply();
    void Revert();
    void Undo();
    void Redo();
    void NewDocument(bool bDemo);
    nexus::Issues Open(const FString& File);
    nexus::Issues Save(const FString& File, bool bExplicitOverwrite=false);
    nexus::Issues SaveRecovery();
    nexus::Issues Recover();
    FString RecoveryPath() const;
    virtual void PostUndo(bool bSuccess) override;
    virtual void PostRedo(bool bSuccess) override { PostUndo(bSuccess); }
    static nexus::Id NewId();
private:
    TStrongObjectPtr<UNexusV8DocumentAsset> Asset;
    std::optional<std::string> DiskHash, RecoveryHash;
    void ResetAsset();
};
