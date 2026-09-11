#include "SNexusV8Editor.h"
#include "SNexusV8Viewport.h"
#include "Editor.h"
#include "Engine/StaticMesh.h"
#include "AssetRegistry/AssetData.h"
#include "PropertyCustomizationHelpers.h"
#include "Misc/MessageDialog.h"
#include "Widgets/SBoxPanel.h"
#include <algorithm>
namespace
{
FString F(const std::string& S){return UTF8_TO_TCHAR(S.c_str());}
std::string U(const FString& S){return TCHAR_TO_UTF8(*S);}
FString ProfileName(const nexus::Document& D,const nexus::Id& Id)
{auto I=D.profiles.find(Id);return I==D.profiles.end()?TEXT("Not assigned"):F(I->second.name);}
FString LaneName(const nexus::Profile& P,const nexus::Id& Id)
{for(const auto& L:P.lanes)if(L.id==Id)return FString::Printf(TEXT("%s%d · %.2f m"),L.direction==nexus::Direction::Forward?TEXT("F"):TEXT("R"),L.ordinal+1,L.widthM);return TEXT("MISSING LANE");}
TArray<TPair<FString,FString>> ProfileOptions(const nexus::Document& D)
{TArray<TPair<FString,FString>> Out;for(const auto& P:D.profiles)Out.Add({F(P.second.name),F(P.first)});return Out;}
TArray<TPair<FString,FString>> LaneOptions(const nexus::Profile& P,nexus::Direction Direction)
{TArray<TPair<FString,FString>> Out;for(const auto& L:P.lanes)if(L.direction==Direction)Out.Add({LaneName(P,L.id),F(L.id)});return Out;}
nexus::MeshInterface DeclaredInterface(const nexus::Profile& A,const nexus::Profile& B)
{
    nexus::MeshInterface M;
    for(const auto* P:{&A,&B})
    {
        nexus::Port Port;Port.name=M.ports.empty()?"Start":"End";
        for(auto Dir:{nexus::Direction::Forward,nexus::Direction::Reverse})for(unsigned N=0;N<32;++N)for(const auto& L:P->lanes)
            if(L.direction==Dir&&L.ordinal==N)(Dir==nexus::Direction::Forward?Port.forwardWidths:Port.reverseWidths).push_back(L.widthM);
        Port.sidewalkLeftM=P->sidewalkLeftM;Port.sidewalkRightM=P->sidewalkRightM;
        Port.borderLeftM=P->borderLeftM;Port.borderRightM=P->borderRightM;M.ports.push_back(Port);
    }
    return M;
}
nexus::Adapter* Adapter(nexus::Document& D,const nexus::Id& Segment,const nexus::Id& Id)
{auto S=D.segments.find(Segment);if(S!=D.segments.end())for(auto& A:S->second.adapters)if(A.id==Id)return &A;return nullptr;}
}
void SNexusV8Editor::WorldDetails(TSharedRef<SVerticalBox> Box)
{
    auto* E=Entity();if(!E)return;const auto Id=E->id;
    Row(Box,TEXT("Name"),Edit(F(E->name),[this,Id](const FString& Value){Report(nexus::renameEntity(Session->Draft,Id,U(Value)),TEXT("Name changed in draft."));Changed();}));
    if(E->kind!=nexus::Kind::World)
    {
        TArray<TPair<FString,FString>> Options;
        for(const auto& P:Session->Draft.entities)if(int(P.second.kind)+1==int(E->kind))Options.Add({F(P.second.name),F(P.first)});
        auto Parent=Session->Draft.entities.find(E->parent);
        Row(Box,TEXT("Parent"),Choice(Parent==Session->Draft.entities.end()?TEXT("Missing parent"):F(Parent->second.name),Options,[this](const FString& P){Reparent(U(P));}));
    }
    if(bAdvanced)
    {
        Row(Box,TEXT("Stable GUID"),Label(F(E->id)));
        Row(Box,TEXT("Descendant impact"),Label(FString::Printf(TEXT("%d dependent entities; %d descendant highways"),int32(nexus::descendants(Session->Draft,E->id).size()),int32(nexus::affectedHighways(Session->Draft,E->id).size()))));
    }
    if(E->kind<=nexus::Kind::Highway)MeshDetails(Box);
    if(E->kind==nexus::Kind::Point)PointDetails(Box);
    if(E->kind==nexus::Kind::Place)PlaceDetails(Box);
    Heading(Box,TEXT("LANDSCAPE SOURCE"));
    Check(Box,TEXT("Scan selected Landscape / spline actors only"),bSelectedOnly,[this](bool V){bSelectedOnly=V;PendingDiff.reset();bRefresh=true;});
    Row(Box,TEXT("Source map"),Label(Session->Draft.sourceWorldKey.empty()?TEXT("Not bound; first reviewed scan chooses the source map."):F(Session->Draft.sourceWorldKey)));
    Row(Box,TEXT("Scan workflow"),Label(TEXT("Scan reads geometry only. Accept stages bindings, then Apply commits semantics. Missing or unloaded sources are retained, not deleted.")));
    if(PendingDiff)
    {
        Heading(Box,TEXT("PENDING SCAN REVIEW"));
        int32 Count=0;
        for(const auto& C:PendingDiff->changes)
        {
            if(++Count>40){Row(Box,TEXT("More changes"),Label(TEXT("Large scan: inspect the source scope before accepting all changes.")));break;}
            const TCHAR* Names[]={TEXT("Add"),TEXT("Changed"),TEXT("Unavailable"),TEXT("Explicit deletion")};
            Row(Box,FString(Names[int(C.kind)])+(C.isSegment?TEXT(" segment"):TEXT(" point")),Label(F(C.sourceKey)));
        }
        Row(Box,TEXT("Review decision"),Button(TEXT("Accept scan into draft"),TEXT("Apply reviewed source bindings; source geometry remains untouched."),[this]{AcceptScan();}));
        Row(Box,TEXT("Cancel"),Button(TEXT("Discard scan review"),TEXT("Leaves source and authored data unchanged."),[this]{PendingDiff.reset();bRefresh=true;}));
    }
    if(bAdvanced)
    {
        Heading(Box,TEXT("RECOVERY"));
        Row(Box,TEXT("Recovery file"),Label(Session->RecoveryPath()));
        Row(Box,TEXT("Recovery action"),Button(TEXT("Recover draft"),TEXT("Requires the same committed baseline; stale recovery is never blindly overwritten."),[this]{Report(Session->Recover(),TEXT("Recovery loaded into draft; review before Apply."));}));
        Row(Box,TEXT("Preserve current draft"),Button(TEXT("Write recovery now"),TEXT("Writes a separate .draft, not the applied document."),[this]{Report(Session->SaveRecovery(),TEXT("Draft recovery checked."));}));
        Row(Box,TEXT("Legacy import"),Label(TEXT("V0–V7 UAssets are not auto-converted. Unknown schemas are rejected without changing the original file. Exported V8 .nv8 documents can be reviewed through Open.")));
    }
}
void SNexusV8Editor::PointDetails(TSharedRef<SVerticalBox> Box)
{
    auto* E=Entity();if(!E||E->kind!=nexus::Kind::Point)return;const auto Id=E->id;
    Heading(Box,TEXT("POINT CONNECTIONS"));
    for(const auto& Pair:Session->Draft.segments)
    {
        const auto& S=Pair.second;if(S.startPoint!=Id&&S.endPoint!=Id)continue;
        const bool bEnd=S.endPoint==Id;auto Source=Session->Source.segments.find(S.sourceKey);
        const double Station=bEnd?((Source==Session->Source.segments.end()||Source->second.samples.empty())?(S.sections.empty()?0:S.sections.back().startM):Source->second.samples.back().stationM):0;
        const auto* Profile=nexus::profileAt(Session->Draft,S,Station);
        const FString Name=(bEnd?TEXT("Incoming end — "):TEXT("Outgoing start — "))+F(Profile?Profile->name:"Profile missing");
        Row(Box,Name,Button(F(S.sourceKey),TEXT("Inspect this segment's lane profile and adapters."),[this,Segment=Pair.first]{SelectedSegment=Segment;Workspace=1;bRefresh=true;}));
        if(bAdvanced&&Source!=Session->Source.segments.end())
        {
            const auto& End=bEnd?Source->second.end:Source->second.start;
            Row(Box,TEXT("Independent end tangent"),Label(FString::Printf(TEXT("Signed %.3f m | vector %.3f, %.3f, %.3f m"),End.signedTangentM,End.tangent.x,End.tangent.y,End.tangent.z)));
        }
    }
    TArray<TPair<FString,FString>> Types;
    for(const auto* Type:{TEXT("Parking"),TEXT("BusStop"),TEXT("Hospital"),TEXT("CityPark"),TEXT("PetrolPump")})Types.Add({Type,Type});
    Row(Box,TEXT("Attach place"),Choice(TEXT("+ Choose place type"),Types,[this](const FString& Type){AddChild(Type);}));
    Row(Box,TEXT("Point features"),Label(TEXT("Crosswalk/junction/roundabout payload editors are Phase 3. A point is an anchor; each connected segment end retains its own tangent.")));
    if(bAdvanced)
    {
        Row(Box,TEXT("Source binding / state"),Label(F(E->sourceKey)+FString::Printf(TEXT("  [state %d]"),int(E->availability))));
        Row(Box,TEXT("Rebind loaded control point"),Edit(F(E->sourceKey),[this,Id,Old=E->sourceKey](const FString& Key)
        {auto Result=nexus::rebindSource(Session->Draft,Id,Old,U(Key),Session->Source);if(!nexus::hasErrors(Result))Changed();Report(MoveTemp(Result),TEXT("Point binding staged. Review incident segments before Apply."));}));
    }
}
void SNexusV8Editor::PlaceDetails(TSharedRef<SVerticalBox> Box)
{
    auto* E=Entity();if(!E||E->kind!=nexus::Kind::Place)return;const auto Id=E->id;
    Heading(Box,TEXT("PLACE IDENTITY / LOCAL OFFSETS"));
    Row(Box,TEXT("Type"),Label(F(E->payload.type)+TEXT(" — identity shell, specialized behavior starts in Phase 3.")));
    TArray<TPair<FString,FString>> Ends;
    for(const auto& P:Session->Draft.segments)
    {
        if(P.second.startPoint==E->parent)Ends.Add({F(P.second.sourceKey)+TEXT(" · Start"),F(P.first)+TEXT(":0")});
        if(P.second.endPoint==E->parent)Ends.Add({F(P.second.sourceKey)+TEXT(" · End"),F(P.first)+TEXT(":1")});
    }
    Row(Box,TEXT("Road frame / approach"),Choice(E->anchorSegment.empty()?TEXT("Choose segment end"):F(E->anchorSegment.substr(24))+(E->anchorAtEnd?TEXT(" · End"):TEXT(" · Start")),Ends,[this,Id](const FString& Value)
    {auto& P=Session->Draft.entities.at(Id);P.anchorSegment=U(Value.Left(32));P.anchorAtEnd=Value.EndsWith(TEXT(":1"));Changed();}));
    Number(Box,TEXT("Along-road X offset (m)"),E->local.position.x,[this,Id](double V){Session->Draft.entities.at(Id).local.position.x=V;});
    Number(Box,TEXT("Right Y offset (m)"),E->local.position.y,[this,Id](double V){Session->Draft.entities.at(Id).local.position.y=V;});
    Number(Box,TEXT("Up Z offset (m)"),E->local.position.z,[this,Id](double V){Session->Draft.entities.at(Id).local.position.z=V;});
    const FRotator R=FQuat(E->local.rotation.x,E->local.rotation.y,E->local.rotation.z,E->local.rotation.w).Rotator();
    auto SetAngle=[this,Id](int32 Axis,double Value)
    {auto& P=Session->Draft.entities.at(Id);FRotator Rot=FQuat(P.local.rotation.x,P.local.rotation.y,P.local.rotation.z,P.local.rotation.w).Rotator();if(Axis==0)Rot.Yaw=Value;else if(Axis==1)Rot.Pitch=Value;else Rot.Roll=Value;const FQuat Q=Rot.Quaternion();P.local.rotation={Q.X,Q.Y,Q.Z,Q.W};};
    Number(Box,TEXT("Yaw offset (degrees)"),R.Yaw,[SetAngle](double V){SetAngle(0,V);});
    if(bAdvanced){Number(Box,TEXT("Pitch offset (degrees)"),R.Pitch,[SetAngle](double V){SetAngle(1,V);});Number(Box,TEXT("Roll offset (degrees)"),R.Roll,[SetAngle](double V){SetAngle(2,V);});}
    Row(Box,TEXT("Scope"),Label(TEXT("Offsets are persisted now. Place meshes, route access and independent viewport gizmos are deliberately not presented as completed in Phase 1–2.")));
}
void SNexusV8Editor::RoadDetails(TSharedRef<SVerticalBox> Box)
{
    auto& D=Session->Draft;const auto H=Highway();
    if(H.empty()){Row(Box,TEXT("Road"),Label(TEXT("Create a highway under a city first.")));return;}
    Heading(Box,TEXT("HIGHWAY DEFAULTS"));
    Row(Box,TEXT("Active highway"),Label(F(D.entities.at(H).name)));
    Row(Box,TEXT("Default profile"),Choice(ProfileName(D,D.entities.at(H).defaultProfile),ProfileOptions(D),[this,H](const FString& Value){Session->Draft.entities.at(H).defaultProfile=U(Value);Changed();}));
    TArray<TPair<FString,FString>> Sides={{TEXT("Right-hand traffic"),TEXT("R")},{TEXT("Left-hand traffic"),TEXT("L")}};
    Row(Box,TEXT("Driving convention"),Choice(D.entities.at(H).drivingSide==nexus::DrivingSide::Right?TEXT("Right-hand traffic"):TEXT("Left-hand traffic"),Sides,[this,H](const FString& Value){Session->Draft.entities.at(H).drivingSide=Value==TEXT("R")?nexus::DrivingSide::Right:nexus::DrivingSide::Left;Changed();}));
    TArray<TPair<FString,FString>> Segments;
    for(const auto& P:D.segments)if(P.second.highway==H)Segments.Add({F(P.second.sourceKey),F(P.first)});
    if(!D.segments.count(SelectedSegment)||D.segments.at(SelectedSegment).highway!=H)SelectedSegment=Segments.Num()?U(Segments[0].Value):"";
    Heading(Box,TEXT("SEGMENT SECTIONS / ADAPTERS"));
    if(SelectedSegment.empty())Row(Box,TEXT("No source segments"),Label(TEXT("Use World → Scan → Accept → Apply, or load Demo.")));
    else
    {
        const auto SegmentId=SelectedSegment;auto& S=D.segments.at(SegmentId);
        Row(Box,TEXT("Segment"),Choice(F(S.sourceKey),Segments,[this](const FString& Value){SelectedSegment=U(Value);bRefresh=true;}));
        const auto* Base=nexus::profileAt(D,S,0);
        Row(Box,TEXT("Profile at segment start"),Choice(Base?F(Base->name):TEXT("Inherit highway default"),ProfileOptions(D),[this,SegmentId](const FString& Value)
        {auto& S=Session->Draft.segments.at(SegmentId);if(S.sections.empty())S.sections.push_back({0,U(Value)});else S.sections.front().profile=U(Value);Changed();}));
        if(bAdvanced)
        {
            for(const auto& Section:S.sections)Row(Box,TEXT("Resolved section"),Label(FString::Printf(TEXT("%.2f m → "),Section.startM)+ProfileName(D,Section.profile)));
            Row(Box,TEXT("Rebind segment"),Edit(F(S.sourceKey),[this,SegmentId,Old=S.sourceKey](const FString& Key){auto Result=nexus::rebindSource(Session->Draft,SegmentId,Old,U(Key),Session->Source);if(!nexus::hasErrors(Result))Changed();Report(MoveTemp(Result));}));
        }
        for(const auto& A:S.adapters)
        {
            const auto AdId=A.id;
            Heading(Box,ProfileName(D,A.before)+TEXT(" → ")+ProfileName(D,A.after));
            Number(Box,TEXT("Transition start station (m)"),A.startM,[this,SegmentId,AdId](double V){if(auto* A=Adapter(Session->Draft,SegmentId,AdId))A->startM=V;});
            Number(Box,TEXT("Transition end station (m)"),A.endM,[this,SegmentId,AdId](double V)
            {if(auto* A=Adapter(Session->Draft,SegmentId,AdId)){for(auto& S:Session->Draft.segments.at(SegmentId).sections)if(FMath::IsNearlyEqual(S.startM,A->endM,1e-7))S.startM=V;A->endM=V;}});
            if(bAdvanced)Number(Box,TEXT("Minimum taper ratio (project rule)"),A.minimumTaperRatio,[this,SegmentId,AdId](double V){if(auto* A=Adapter(Session->Draft,SegmentId,AdId))A->minimumTaperRatio=V;});
            Row(Box,TEXT("Mapping proposals"),Button(TEXT("Regenerate lane mappings"),TEXT("Replaces this adapter's draft links; new links start unapproved."),[this,SegmentId,AdId]
            {auto* A=Adapter(Session->Draft,SegmentId,AdId);if(!A)return;A->links=nexus::proposeLinks(Session->Draft.profiles.at(A->before),Session->Draft.profiles.at(A->after),[]{return FNexusV8EditorSession::NewId();});Changed();}));
            Row(Box,TEXT("Approve inspected mappings"),Button(TEXT("Approve all links"),TEXT("Explicitly acknowledge every visible mapping. Validation still rejects missing or wrong-way links."),[this,SegmentId,AdId]
            {if(auto* A=Adapter(Session->Draft,SegmentId,AdId))for(auto& L:A->links)L.approved=true;Changed();}));
            for(const auto& L:A.links)
            {
                const auto LinkId=L.id;
                const auto& From=D.profiles.at(L.direction==nexus::Direction::Forward?A.before:A.after);
                const auto& To=D.profiles.at(L.direction==nexus::Direction::Forward?A.after:A.before);
                const FString K=L.kind==nexus::LinkKind::Merge?TEXT("MERGE"):L.kind==nexus::LinkKind::Branch?TEXT("BRANCH"):TEXT("CONTINUE");
                Row(Box,(L.approved?TEXT("✓ "):TEXT("REVIEW "))+LaneName(From,L.fromLane)+TEXT(" → ")+LaneName(To,L.toLane),Label(K));
                if(bAdvanced)
                {
                    Row(Box,TEXT("Destination lane"),Choice(LaneName(To,L.toLane),LaneOptions(To,L.direction),[this,SegmentId,AdId,LinkId](const FString& Value)
                    {if(auto* A=Adapter(Session->Draft,SegmentId,AdId))for(auto& L:A->links)if(L.id==LinkId){L.toLane=U(Value);L.approved=false;}Changed();}));
                    TArray<TPair<FString,FString>> Kinds={{TEXT("Continue"),TEXT("0")},{TEXT("Merge"),TEXT("1")},{TEXT("Branch"),TEXT("2")}};
                    Row(Box,TEXT("Connection kind"),Choice(K,Kinds,[this,SegmentId,AdId,LinkId](const FString& Value)
                    {if(auto* A=Adapter(Session->Draft,SegmentId,AdId))for(auto& L:A->links)if(L.id==LinkId){L.kind=nexus::LinkKind(FCString::Atoi(*Value));L.approved=false;}Changed();}));
                    Row(Box,TEXT("Negative test / repair"),Button(TEXT("Remove this link"),TEXT("Preview is blocked until every lane is mapped again."),[this,SegmentId,AdId,LinkId]
                    {if(auto* A=Adapter(Session->Draft,SegmentId,AdId))A->links.erase(std::remove_if(A->links.begin(),A->links.end(),[&](const auto& L){return L.id==LinkId;}),A->links.end());Changed();}));
                }
            }
        }
        Row(Box,TEXT("Add transition to profile"),Choice(TEXT("Choose target profile"),ProfileOptions(D),[this,SegmentId](const FString& Value)
        {
            auto& S=Session->Draft.segments.at(SegmentId);const double Start=S.adapters.empty()?20:S.adapters.back().endM+20;
            const auto* Before=nexus::profileAt(Session->Draft,S,Start);if(!Before){Notice=TEXT("Set the segment's input profile first.");return;}
            nexus::Adapter A;A.id=FNexusV8EditorSession::NewId();A.startM=Start;A.endM=Start+80;A.before=Before->id;A.after=U(Value);
            A.links=nexus::proposeLinks(*Before,Session->Draft.profiles.at(A.after),[]{return FNexusV8EditorSession::NewId();});
            if(S.sections.empty())S.sections.push_back({0,A.before});S.sections.push_back({A.endM,A.after});S.adapters.push_back(A);Changed();
            Notice=TEXT("Adapter proposed. Set a valid interval, inspect every lane, then approve links. Direct 6→2 is blocked; use staged 6→4→2.");
        }));
    }
    Heading(Box,TEXT("REUSABLE LANE PROFILE"));
    if(!D.profiles.count(SelectedProfile))SelectedProfile=D.entities.at(H).defaultProfile;
    Row(Box,TEXT("Edit shared profile"),Choice(ProfileName(D,SelectedProfile),ProfileOptions(D),[this](const FString& Value){SelectedProfile=U(Value);bRefresh=true;}));
    if(D.profiles.count(SelectedProfile))
    {
        const auto ProfileId=SelectedProfile;const auto& P=D.profiles.at(ProfileId);
        Row(Box,TEXT("Sharing"),Label(TEXT("Edits affect every reference to this profile. Clone before making a local variant.")));
        Row(Box,TEXT("Name"),Edit(F(P.name),[this,ProfileId](const FString& V){Session->Draft.profiles.at(ProfileId).name=U(V);Changed();}));
        Row(Box,TEXT("Create variant"),Button(TEXT("Clone profile"),TEXT("New profile and lane GUIDs; existing roads are unchanged until assigned."),[this,ProfileId]
        {auto P=Session->Draft.profiles.at(ProfileId);P.id=FNexusV8EditorSession::NewId();P.name+=" copy";for(auto& L:P.lanes)L.id=FNexusV8EditorSession::NewId();SelectedProfile=P.id;Session->Draft.profiles.emplace(P.id,P);Changed();}));
        for(const auto& L:P.lanes)
        {
            const auto LaneId=L.id;Heading(Box,LaneName(P,LaneId));
            Number(Box,TEXT("Width (m)"),L.widthM,[this,ProfileId,LaneId](double V){for(auto& L:Session->Draft.profiles.at(ProfileId).lanes)if(L.id==LaneId)L.widthM=V;});
            if(bAdvanced)
            {
                Number(Box,TEXT("Speed (km/h)"),L.speedMps*3.6,[this,ProfileId,LaneId](double V){for(auto& L:Session->Draft.profiles.at(ProfileId).lanes)if(L.id==LaneId)L.speedMps=V/3.6;});
                TArray<TPair<FString,FString>> Access={{TEXT("All road vehicles"),TEXT("15")},{TEXT("Car"),TEXT("1")},{TEXT("Bus only"),TEXT("2")},{TEXT("Emergency"),TEXT("4")},{TEXT("Service"),TEXT("8")}};
                Row(Box,TEXT("Allowed vehicle classes"),Choice(FString::Printf(TEXT("Mask %u"),L.access),Access,[this,ProfileId,LaneId](const FString& V){for(auto& L:Session->Draft.profiles.at(ProfileId).lanes)if(L.id==LaneId)L.access=FCString::Atoi(*V);Changed();}));
                Row(Box,TEXT("Remove lane"),Button(TEXT("Remove lane; review adapters"),TEXT("Stable IDs of remaining lanes are preserved. Regenerate affected mappings before Apply."),[this,ProfileId,LaneId]
                {auto& Lanes=Session->Draft.profiles.at(ProfileId).lanes;Lanes.erase(std::remove_if(Lanes.begin(),Lanes.end(),[&](const auto& L){return L.id==LaneId;}),Lanes.end());for(auto Dir:{nexus::Direction::Forward,nexus::Direction::Reverse}){unsigned N=0;for(auto& L:Lanes)if(L.direction==Dir)L.ordinal=N++;}Changed();}));
            }
        }
        TArray<TPair<FString,FString>> Dirs={{TEXT("Forward lane"),TEXT("0")},{TEXT("Reverse lane"),TEXT("1")}};
        Row(Box,TEXT("Add lane"),Choice(TEXT("+ Select direction"),Dirs,[this,ProfileId](const FString& Value)
        {auto& P=Session->Draft.profiles.at(ProfileId);nexus::Lane L;L.id=FNexusV8EditorSession::NewId();L.direction=nexus::Direction(FCString::Atoi(*Value));for(const auto& Old:P.lanes)if(Old.direction==L.direction)++L.ordinal;P.lanes.push_back(L);Changed();}));
        Number(Box,TEXT("Median width (m)"),P.medianM,[this,ProfileId](double V){Session->Draft.profiles.at(ProfileId).medianM=V;});
        Number(Box,TEXT("Left sidewalk width (m)"),P.sidewalkLeftM,[this,ProfileId](double V){Session->Draft.profiles.at(ProfileId).sidewalkLeftM=V;});
        Number(Box,TEXT("Right sidewalk width (m)"),P.sidewalkRightM,[this,ProfileId](double V){Session->Draft.profiles.at(ProfileId).sidewalkRightM=V;});
        Number(Box,TEXT("Left border width (m)"),P.borderLeftM,[this,ProfileId](double V){Session->Draft.profiles.at(ProfileId).borderLeftM=V;});
        Number(Box,TEXT("Right border width (m)"),P.borderRightM,[this,ProfileId](double V){Session->Draft.profiles.at(ProfileId).borderRightM=V;});
        Number(Box,TEXT("Sidewalk / curb height (m)"),P.sidewalkHeightM,[this,ProfileId](double V){Session->Draft.profiles.at(ProfileId).sidewalkHeightM=V;});
    }
    Heading(Box,TEXT("PREVIEW MODE"));
    Check(Box,TEXT("Use assigned catalog meshes (strict asset checks)"),bCatalogMeshes,[this](bool V){bCatalogMeshes=V;bRefresh=true;});
    Row(Box,TEXT("Geometry-only mode"),Label(TEXT("Unchecked: generated road/sidewalk/curb and texture markings, with lane overlays; catalog assets are not required. Checked: your compatible meshes are deformed along the source; mesh contracts are mandatory.")));
}
void SNexusV8Editor::MeshDetails(TSharedRef<SVerticalBox> Box)
{
    auto* E=Entity();if(!E||E->kind>nexus::Kind::Highway)return;const auto Scope=E->id;auto& D=Session->Draft;
    Heading(Box,TEXT("MESH DEFAULTS / FOUR-LEVEL FALLBACK"));
    Row(Box,TEXT("Resolution order"),Label(TEXT("Highway → City → State → World/Root. Each slot inherits independently. Broken explicit assets block; they never silently inherit.")));
    TArray<TPair<FString,FString>> Catalogs={{TEXT("Not assigned — inherit"),TEXT("")}};
    for(const auto& P:D.catalogs)Catalogs.Add({F(P.second.name),F(P.first)});
    Row(Box,TEXT("Catalog at this level"),Choice(D.catalogs.count(E->catalog)?F(D.catalogs.at(E->catalog).name):TEXT("Not assigned — inherit"),Catalogs,[this,Scope](const FString& V){Session->Draft.entities.at(Scope).catalog=U(V);Changed();}));
    Row(Box,TEXT("Reusable configuration"),Button(TEXT("Create local catalog"),TEXT("Creates an empty catalog; unassigned entries inherit."),[this,Scope]
    {nexus::Catalog C;C.id=FNexusV8EditorSession::NewId();C.name=Session->Draft.entities.at(Scope).name+" mesh defaults";Session->Draft.catalogs.emplace(C.id,C);Session->Draft.entities.at(Scope).catalog=C.id;Changed();}));
    if(!D.catalogs.count(E->catalog))return;const auto CatalogId=E->catalog;
    Row(Box,TEXT("Shared catalog name"),Edit(F(D.catalogs.at(CatalogId).name),[this,CatalogId](const FString& V){Session->Draft.catalogs.at(CatalogId).name=U(V);Changed();}));
    Row(Box,TEXT("Make independent copy"),Button(TEXT("Clone catalog for this level"),TEXT("Prevents edits affecting other levels sharing the same catalog."),[this,Scope,CatalogId]
    {auto C=Session->Draft.catalogs.at(CatalogId);C.id=FNexusV8EditorSession::NewId();C.name+=" copy";Session->Draft.catalogs.emplace(C.id,C);Session->Draft.entities.at(Scope).catalog=C.id;Changed();}));
    std::map<std::string,std::pair<nexus::Id,nexus::Id>> SlotProfiles;
    TArray<TPair<FString,FString>> Slots;
    for(const auto& P:D.profiles)
    {
        const auto Generic="Road."+std::to_string(P.second.lanes.size());
        if(!SlotProfiles.count(Generic)){SlotProfiles[Generic]={P.first,P.first};Slots.Add({F(Generic),F(Generic)});}
        const auto Exact="Road.Profile."+P.first;SlotProfiles[Exact]={P.first,P.first};
        Slots.Add({TEXT("Road variant · ")+F(P.second.name),F(Exact)});
    }
    for(const auto& S:D.segments)for(const auto& A:S.second.adapters)
    {
        const auto& P=D.profiles.at(A.before);const auto& Q=D.profiles.at(A.after);
        const auto Generic="Adapter."+std::to_string(P.lanes.size())+"-"+std::to_string(Q.lanes.size());
        if(!SlotProfiles.count(Generic)){SlotProfiles[Generic]={P.id,Q.id};Slots.Add({F(Generic),F(Generic)});}
        const auto Exact="Adapter.Profile."+P.id+"-"+Q.id;
        if(!SlotProfiles.count(Exact)){SlotProfiles[Exact]={P.id,Q.id};Slots.Add({TEXT("Adapter variant · ")+F(P.name)+TEXT(" → ")+F(Q.name),F(Exact)});}
    }
    for(const auto* Key:{"BusStop.Shelter","Parking.Base","Hospital.Base","CityPark.Base","PetrolPump.Base"})Slots.Add({F(Key)+TEXT(" (Phase 3 slot)"),F(Key)});
    if(SelectedSlot.empty()&&Slots.Num())SelectedSlot=U(Slots[0].Value);
    Row(Box,TEXT("Semantic mesh slot"),Choice(F(SelectedSlot),Slots,[this](const FString& V){SelectedSlot=U(V);bRefresh=true;}));
    const auto Slot=SelectedSlot;
    auto It=D.catalogs.at(CatalogId).entries.find(Slot);const auto Entry=It==D.catalogs.at(CatalogId).entries.end()?nexus::MeshEntry{}:It->second;
    TArray<TPair<FString,FString>> States={{TEXT("Inherit"),TEXT("0")},{TEXT("Assigned"),TEXT("1")},{TEXT("Disabled"),TEXT("2")}};
    Row(Box,TEXT("Local assignment state"),Choice(Entry.assignment==nexus::Assignment::Assigned?TEXT("Assigned"):Entry.assignment==nexus::Assignment::Disabled?TEXT("Disabled"):TEXT("Inherit"),States,[this,CatalogId,Slot](const FString& V){Session->Draft.catalogs.at(CatalogId).entries[Slot].assignment=nexus::Assignment(FCString::Atoi(*V));Changed();}));
    Row(Box,TEXT("Static mesh asset"),SNew(SObjectPropertyEntryBox).AllowedClass(UStaticMesh::StaticClass()).ObjectPath(F(Entry.assetPath))
        .OnObjectChanged_Lambda([this,CatalogId,Slot](const FAssetData& Asset){auto& E=Session->Draft.catalogs.at(CatalogId).entries[Slot];E.assetPath=U(Asset.GetSoftObjectPath().ToString());E.assignment=Asset.IsValid()?nexus::Assignment::Assigned:nexus::Assignment::Inherit;Changed();}));
    Row(Box,TEXT("Clear local value"),Button(TEXT("Clear → inherit"),TEXT("Removes this override instead of copying parent values."),[this,CatalogId,Slot]{Session->Draft.catalogs.at(CatalogId).entries.erase(Slot);Changed();}));
    const auto H=Highway();
    if(!H.empty())
    {
        const auto Resolved=nexus::resolveMesh(D,H,Slot,false,[](const std::string& A){return LoadObject<UStaticMesh>(nullptr,*F(A))!=nullptr;});
        Row(Box,TEXT("Effective value / provenance"),Label(Resolved.resolved?F(Resolved.entry.assetPath)+TEXT("\nFrom: ")+F(D.entities.at(Resolved.scope).name):Resolved.disabled?TEXT("Explicitly disabled"):nexus::hasErrors(Resolved.issues)?F(Resolved.issues[0].message):TEXT("Unassigned at every level")));
    }
    if(SlotProfiles.count(Slot))
    {
        const auto Profiles=SlotProfiles.at(Slot);
        Row(Box,TEXT("Port contract"),Button(TEXT("Declare ports from selected profile"),TEXT("Writes expected metadata, not a proof of mesh compatibility. Check your asset dimensions/pivot before using it."),[this,CatalogId,Slot,Profiles]
        {const double OldLength=Session->Draft.catalogs.at(CatalogId).entries[Slot].interface.lengthM;auto M=DeclaredInterface(Session->Draft.profiles.at(Profiles.first),Session->Draft.profiles.at(Profiles.second));M.lengthM=OldLength;Session->Draft.catalogs.at(CatalogId).entries[Slot].interface=M;Changed();}));
    }
    Number(Box,TEXT("Actual source mesh length (m)"),Entry.interface.lengthM,[this,CatalogId,Slot](double V){Session->Draft.catalogs.at(CatalogId).entries[Slot].interface.lengthM=V;});
    if(bAdvanced)
    {
        Row(Box,TEXT("Expected asset convention"),Label(TEXT("Centimeters; +X forward; +Z up; pivot at start center. Open modular ends and texture-based markings. Actual X bounds are checked when staging assigned meshes.")));
        Row(Box,TEXT("Declaration status"),Label(FString::Printf(TEXT("%d declared ports. Slot widths, sidewalk/border and directions must match the profiles. Material/UV/end-face quality still requires the documented engine review."),int32(Entry.interface.ports.size()))));
    }
}
void SNexusV8Editor::VerifyDetails(TSharedRef<SVerticalBox> Box)
{
    Heading(Box,TEXT("VERIFY PHASE 1–2"));
    Row(Box,TEXT("Semantic checks"),Button(TEXT("Validate current draft"),TEXT("Runs hierarchy, profiles, references and adapter validation."),[this]{Report(nexus::validate(Session->Draft),TEXT("No blocking semantic errors. Engine acceptance still requires separate tests."));}));
    Row(Box,TEXT("Full runtime payload gate"),Button(TEXT("Check production eligibility"),TEXT("Phase 1–2 feature shells are intentionally unsupported by the production compiler."),[this]{Report(nexus::validate(Session->Draft,true),TEXT("Semantic validation returned no errors; Phase 1–2 output remains preview-only."));}));
    Row(Box,TEXT("Preview"),Button(TEXT("Build / replace preview"),TEXT("Builds a coherent preview with source and document fingerprints."),[this]{BuildPreview();}));
    Check(Box,TEXT("Use assigned catalog meshes"),bCatalogMeshes,[this](bool V){bCatalogMeshes=V;bRefresh=true;});
    Row(Box,TEXT("Evidence export"),Button(TEXT("Export diagnostic report"),TEXT("Writes Saved/NexusV8/LastEditorReport.txt. This is not an automation pass report."),[this]{ExportReport();}));
    Row(Box,TEXT("Required engine command"),Label(TEXT("Scripts/Test-Unreal.ps1 runs NexusV8.* automation and retains the report. Follow Docs/ENGINE-TEST-CHECKLIST.md for the 30 phase scenarios.")));
    Row(Box,TEXT("Native implementation status"),Label(TEXT("Source delivered; Unreal compilation and interactive acceptance were NOT executed in the delivery environment. Portable C++ test results are separate evidence.")));
    Row(Box,TEXT("Future scope"),Label(TEXT("Phase 3: service and intersection editing/gizmos. Phase 4: full route-query tooling. Phase 5: production compiler, traffic controllers and streaming. Nothing here claims GTA-level behavior is already complete.")));
}
