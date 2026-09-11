#include "Modules/ModuleManager.h"
#include "ToolMenus.h"
#include "Framework/Docking/TabManager.h"
#include "Styling/AppStyle.h"
#include "Widgets/Docking/SDockTab.h"
#include "SNexusV8Editor.h"
#include "HAL/IConsoleManager.h"
#include "Misc/CoreDelegates.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
class FNexusV8EditorModule : public IModuleInterface
{
    IConsoleObject* OpenCommand=nullptr;
    TWeakPtr<SDockTab> OpenTab;
    void Open(){FGlobalTabmanager::Get()->TryInvokeTab(FName("NexusV8.Editor"));}
    TSharedRef<SDockTab> Spawn(const FSpawnTabArgs&)
    {
        auto Tab=SNew(SDockTab).TabRole(ETabRole::NomadTab)[SNew(SNexusV8Editor)];OpenTab=Tab;return Tab;
    }
    void Menus()
    {
        FToolMenuOwnerScoped Owner(this);
        UToolMenu* Menu=UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Tools");
        Menu->FindOrAddSection("NexusV8").AddMenuEntry("NexusV8.Open",NSLOCTEXT("NexusV8","MenuName","Nexus V8 Road Editor"),NSLOCTEXT("NexusV8","MenuTip","Open the Phase 1–2 authoring toolkit."),FSlateIcon(FAppStyle::GetAppStyleSetName(),"LevelEditor.Tabs.Viewports"),FUIAction(FExecuteAction::CreateRaw(this,&FNexusV8EditorModule::Open)));
    }
public:
    virtual void StartupModule() override
    {
        if(IsRunningCommandlet())return;
        FGlobalTabmanager::Get()->RegisterNomadTabSpawner("NexusV8.Editor",FOnSpawnTab::CreateRaw(this,&FNexusV8EditorModule::Spawn))
            .SetDisplayName(NSLOCTEXT("NexusV8","Tab","Nexus V8 Road Editor")).SetMenuType(ETabSpawnerMenuType::Hidden);
        UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this,&FNexusV8EditorModule::Menus));
        OpenCommand=IConsoleManager::Get().RegisterConsoleCommand(TEXT("NexusV8.Open"),TEXT("Open Nexus V8 authoring editor."),FConsoleCommandDelegate::CreateRaw(this,&FNexusV8EditorModule::Open),ECVF_Default);
    }
    virtual void ShutdownModule() override
    {
        if(auto Tab=OpenTab.Pin())Tab->RequestCloseTab();
        if(OpenCommand)IConsoleManager::Get().UnregisterConsoleObject(OpenCommand);
        UToolMenus::UnRegisterStartupCallback(this);UToolMenus::UnregisterOwner(this);
        FGlobalTabmanager::Get()->UnregisterNomadTabSpawner("NexusV8.Editor");
    }
};
IMPLEMENT_MODULE(FNexusV8EditorModule,NexusV8Editor);
