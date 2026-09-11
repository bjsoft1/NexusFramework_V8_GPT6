#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "NexusV8DocumentAsset.h"
#include "NexusV8EditorSession.h"
#include "NexusV8SourceReader.h"
#include "Domain/NexusDomain.h"
#include "Editor.h"
#include "Engine/World.h"
#include "Misc/Paths.h"
#include "UObject/Package.h"
#include "UObject/StrongObjectPtr.h"
namespace
{
nexus::Document Example(){std::uint64_t N=1;return nexus::makeExample([&]{return nexus::fixtureId(N++);});}
constexpr auto Flags=EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNexusAssetRoundTrip,"NexusV8.Automated.Phase1.AssetRoundTrip",Flags)
bool FNexusAssetRoundTrip::RunTest(const FString&)
{
    TStrongObjectPtr<UNexusV8DocumentAsset> Asset(NewObject<UNexusV8DocumentAsset>());
    const auto Original=Example();FString Error;
    TestTrue(TEXT("Write native payload"),Asset->Write(Original,Error));
    nexus::Document Restored;
    TestTrue(TEXT("Read native payload"),Asset->Read(Restored,Error));
    TestTrue(TEXT("Content fingerprint preserved"),nexus::contentFingerprint(Original)==nexus::contentFingerprint(Restored));
    return !HasAnyErrors();
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNexusNativeUndo,"NexusV8.Automated.Phase1.NativeUndoRedo",Flags)
bool FNexusNativeUndo::RunTest(const FString&)
{
    if(!GEditor){AddError(TEXT("This test requires the editor transaction subsystem."));return false;}
    FNexusV8EditorSession Session;
    const auto Id=Session.Draft.entities.begin()->first;
    const auto Old=Session.Draft.entities.at(Id).name;
    Session.Draft.entities.at(Id).name="Transaction test";
    TestFalse(TEXT("Apply has no blocking error"),nexus::hasErrors(Session.Apply()));
    Session.Undo();TestTrue(TEXT("Native Undo restored payload"),Session.Base.entities.at(Id).name==Old);
    Session.Redo();TestTrue(TEXT("Native Redo restored payload"),Session.Base.entities.at(Id).name=="Transaction test");
    return !HasAnyErrors();
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNexusHierarchy,"NexusV8.Automated.Phase1.Hierarchy",Flags)
bool FNexusHierarchy::RunTest(const FString&)
{
    auto D=Example();TestFalse(TEXT("Fixture valid"),nexus::hasErrors(nexus::validate(D)));
    for(auto& P:D.entities)if(P.second.kind==nexus::Kind::World){P.second.parent=P.first;break;}
    TestTrue(TEXT("World cannot be its own child"),nexus::hasErrors(nexus::validate(D)));
    return !HasAnyErrors();
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNexusPayload,"NexusV8.Automated.Phase1.UnknownPayload",Flags)
bool FNexusPayload::RunTest(const FString&)
{
    auto D=Example();auto& E=D.entities.begin()->second;E.payload={"FutureExtension",7,true,"Keep these bytes"};
    nexus::Document Read;TestFalse(TEXT("Unknown payload survives document load"),nexus::hasErrors(nexus::decode(nexus::encode(D),Read)));
    TestTrue(TEXT("Exact payload retained"),Read.entities.at(E.id).payload.bytes==E.payload.bytes);
    TestTrue(TEXT("Unhandled production payload blocked"),nexus::hasErrors(nexus::validate(Read,true)));
    return !HasAnyErrors();
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNexusSourceNoMutation,"NexusV8.Automated.Phase1.ScanDoesNotDirtyWorld",Flags)
bool FNexusSourceNoMutation::RunTest(const FString&)
{
    UWorld* World=GEditor?GEditor->GetEditorWorldContext().World():nullptr;
    if(!World){AddError(TEXT("Open an editor world before this test."));return false;}
    const bool DirtyBefore=World->GetOutermost()->IsDirty();nexus::Issues Issues;
    const auto Scan=FNexusV8SourceReader::Scan(World,false,Issues);
    TestTrue(TEXT("Scanner identifies world"),!Scan.worldKey.empty());
    TestEqual(TEXT("Read-only scan does not change package dirty state"),World->GetOutermost()->IsDirty(),DirtyBefore);
    for(const auto& I:Issues)if(I.blocking)AddError(UTF8_TO_TCHAR(I.message.c_str()));
    // A blank map only proves the empty-world read path. Full Landscape fixtures are manual requirements.
    return !HasAnyErrors();
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNexusTwoWayAdapter,"NexusV8.Automated.Phase2.DirectionalAdapter",Flags)
bool FNexusTwoWayAdapter::RunTest(const FString&)
{
    auto D=Example();const auto& A=D.segments.begin()->second.adapters.front();
    bool Merge=false,Branch=false;
    for(const auto& L:A.links){Merge|=L.direction==nexus::Direction::Forward&&L.kind==nexus::LinkKind::Merge;Branch|=L.direction==nexus::Direction::Reverse&&L.kind==nexus::LinkKind::Branch;}
    TestTrue(TEXT("Forward direction has merge"),Merge);TestTrue(TEXT("Reverse direction has branch"),Branch);
    return !HasAnyErrors();
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNexusPreviewCompile,"NexusV8.Automated.Phase2.CoherentPreview",Flags)
bool FNexusPreviewCompile::RunTest(const FString&)
{
    const auto D=Example();nexus::CompileOptions O;O.requireAssets=false;
    const auto R=nexus::compilePreview(D,nexus::makeExampleScan(),{},O);
    TestTrue(TEXT("Preview compiles"),R.success);
    TestEqual(TEXT("Two source segments"),int32(R.snapshot.roads.size()),2);
    TestTrue(TEXT("Document fingerprint matches"),R.snapshot.documentFingerprint==nexus::contentFingerprint(D));
    TestFalse(TEXT("Not production publishable"),R.snapshot.productionPublishable);
    return !HasAnyErrors();
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNexusPreviewCancel,"NexusV8.Automated.Phase2.CancelPreservesBoundary",Flags)
bool FNexusPreviewCancel::RunTest(const FString&)
{
    auto D=Example();int32 Polls=0;nexus::CompileOptions O;O.requireAssets=false;O.canceled=[&]{return ++Polls>=2;};
    const auto R=nexus::compilePreview(D,nexus::makeExampleScan(),{},O);
    TestFalse(TEXT("Canceled compilation fails"),R.success);
    TestTrue(TEXT("No partial snapshot returned"),R.snapshot.roads.empty());
    return !HasAnyErrors();
}
#endif
