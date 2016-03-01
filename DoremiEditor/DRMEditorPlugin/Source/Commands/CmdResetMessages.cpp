#include "CmdResetMessages.hpp"
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
            CmdResetMessages::CmdResetMessages() {}
            CmdResetMessages::~CmdResetMessages() {}
            MStatus CmdResetMessages::doIt(const MArgList& args)
            {
                // ApplicationContext::GetInstance().GetMessageHandler()->PrintVectorInfo(true);
                ApplicationContext::GetInstance().GetMessageHandler()->PrintVectorInfo(false);
                ApplicationContext::GetInstance().GetMessageHandler()->ClearMessageVectors();
                ApplicationContext::GetInstance().GetMessageHandler()->PrintVectorInfo(false);
                return MS::kSuccess;
            }
            void* CmdResetMessages::creator() { return (void*)(new CmdResetMessages); }
        }
    }
}