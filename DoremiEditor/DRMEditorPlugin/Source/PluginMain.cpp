#include <DoremiEditor\DRMEditorPlugin\Include\PluginMain.hpp>
#include <DoremiEditor\DRMEditorPlugin\Include\ApplicationContext.hpp>
#include <DoremiEditor\DRMEditorPlugin\Include\NodeHandler.hpp>
#include <DoremiEditor\DRMEditorPlugin\Include\MessageHandler.hpp>
#include <DoremiEditor\DRMEditorPlugin\Include\CallbackHandler.hpp>
#include <DoremiEditor\DRMEditorPlugin\Include\CmdFileMapToggle.hpp>
#include <DoremiEditor\DRMEditorPlugin\Include\CmdLoadScene.hpp>
#include <DoremiEditor\DRMEditorPlugin\Include\CmdUnloadScene.hpp>
#include <DoremiEditor\DRMEditorPlugin\Include\CmdResetMessages.hpp>
#include <DoremiEditor\DRMEditorPlugin\Include\CmdSetFileName.hpp>
#include <DoremiEditor\DRMEditorPlugin\Include\CmdSetDebugMode.hpp>
#include <DoremiEditor\DRMEditorPlugin\Include\Filemapping.hpp>
#include <iostream>

// using namespace std;

// called when the plugin is loaded
EXPORT MStatus initializePlugin(MObject obj)
{
    MStatus res = MS::kSuccess;
    try
    {
        DoremiEditor::Plugin::ApplicationContext::Startup();
        // most functions will use this variable to indicate for errors

        MFnPlugin myPlugin(obj, "Maya plugin", "1.0", "Any", &res);
        if(MFAIL(res))
        {
            CHECK_MSTATUS(res);
        }

        MGlobal::displayInfo("Doremi Editor Maya plugin loaded! ");
        // if res == kSuccess then the plugin has been loaded,
        // otherwise is has not.


        DoremiEditor::Plugin::ApplicationContext::GetInstance().InitializeClasses();

        myPlugin.registerCommand("drmToggleFilemaps", DoremiEditor::Plugin::Commands::CmdFileMapToggle::creator,
                                 DoremiEditor::Plugin::Commands::CmdFileMapToggle::stateSyntax);
        myPlugin.registerCommand("drmLoadScene", DoremiEditor::Plugin::Commands::CmdLoadScene::creator);
        myPlugin.registerCommand("drmUnloadScene", DoremiEditor::Plugin::Commands::CmdUnloadScene::creator);
        myPlugin.registerCommand("drmResetMessages", DoremiEditor::Plugin::Commands::CmdResetMessages::creator);
        myPlugin.registerCommand("drmSetFileName", DoremiEditor::Plugin::Commands::CmdSetFileName::creator,
                                 DoremiEditor::Plugin::Commands::CmdSetFileName::nameSyntax);
        myPlugin.registerCommand("drmSetDebug", DoremiEditor::Plugin::Commands::CmdSetDebugMode::creator,
                                 DoremiEditor::Plugin::Commands::CmdSetDebugMode::stateSyntax);
    }
    catch(...)
    {
        res = MS::kFailure;
        MGlobal::displayInfo("Somethung krasched! xx");
    }

    return res;
}


EXPORT MStatus uninitializePlugin(MObject obj)
{
    // simply initialize the Function set with the MObject that represents
    // our plugin
    MFnPlugin plugin(obj);
    DoremiEditor::Plugin::ApplicationContext::Shutdown();
    // if any resources have been allocated, release and free here before
    // returning...
    plugin.deregisterCommand("drmToggleFilemaps");
    plugin.deregisterCommand("drmLoadScene");
    plugin.deregisterCommand("drmUnloadScene");
    plugin.deregisterCommand("drmResetMessages");
    plugin.deregisterCommand("drmSetFileName");
    plugin.deregisterCommand("drmSetDebug");
    MGlobal::displayInfo("Doremi Editor Maya plugin unloaded!");

    return MS::kSuccess;
}