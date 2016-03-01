#include "CmdLoadScene.hpp"
#include "ApplicationContext.hpp"
#include "MessageHandler.hpp"
#include "CallbackHandler.hpp"
#include "Filemapping.hpp"
#include "PrintHelper.hpp"
namespace DoremiEditor
{
    namespace Plugin
    {
        namespace Commands
        {
            CmdLoadScene::CmdLoadScene() {}
            CmdLoadScene::~CmdLoadScene() {}
            MStatus CmdLoadScene::doIt(const MArgList& args)
            {
                // ApplicationContext::GetInstance().GetMessageHandler()->PrintVectorInfo(true);
                ApplicationContext::GetInstance().GetCallbackHandler()->LoadScene();

                return MS::kSuccess;
            }
            void* CmdLoadScene::creator() { return (void*)(new CmdLoadScene); }
        }
    }
}