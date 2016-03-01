#include "CmdUnloadScene.hpp"
#include "ApplicationContext.hpp"
#include "MessageHandler.hpp"
#include "CallbackHandler.hpp"
#include "NodeHandler.hpp"
#include "Filemapping.hpp"
#include "PrintHelper.hpp"
namespace DoremiEditor
{
    namespace Plugin
    {
        namespace Commands
        {
            CmdUnloadScene::CmdUnloadScene() {}
            CmdUnloadScene::~CmdUnloadScene() {}
            MStatus CmdUnloadScene::doIt(const MArgList& args)
            {
                // ApplicationContext::GetInstance().GetMessageHandler()->PrintVectorInfo(true);
                ApplicationContext::GetInstance().GetCallbackHandler()->UnloadScene();
                ApplicationContext::GetInstance().GetNodeHandler()->PrintVectorInfo();
                return MS::kSuccess;
            }
            void* CmdUnloadScene::creator() { return (void*)(new CmdUnloadScene); }
        }
    }
}