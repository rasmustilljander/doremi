#include "CmdSetDebugMode.hpp"
#include "ApplicationContext.hpp"
#include "MessageHandler.hpp"
#include "Filemapping.hpp"
#include "PrintHelper.hpp"
namespace DoremiEditor
{
    namespace Plugin
    {
        namespace Commands
        {
            const char* m_longDebugOnOffName = "-state";
            const char* m_debugOnOffName = "-s";
            CmdSetDebugMode::CmdSetDebugMode() {}
            CmdSetDebugMode::~CmdSetDebugMode() {}
            MStatus CmdSetDebugMode::doIt(const MArgList& args)
            {
                try
                {
                    MStatus result;
                    MArgDatabase t_parser(syntax(), args, &result);
                    bool t_inputParameter = false;
                    if(result != MS::kSuccess)
                    {
                        PrintWarning("CmdFileMapToggle failed");
                        return result;
                    }
                    if(t_parser.isFlagSet(m_debugOnOffName))
                    {
                        bool dStatus = ApplicationContext::GetInstance().GetDebugStatus();
                        t_parser.getFlagArgument(m_debugOnOffName, 0, t_inputParameter);
                        if(t_inputParameter)
                        {
                            if(!dStatus)
                            {
                                ApplicationContext::GetInstance().SetDebugStatus(true);
                            }
                        }
                        else
                        {
                            if(dStatus)
                            {
                                ApplicationContext::GetInstance().SetDebugStatus(false);
                            }
                        }
                    }
                    else
                    {
                        if(ApplicationContext::GetInstance().GetDebugStatus())
                        {
                            PrintInfo("Debug status is ON");
                        }
                        else
                        {
                            PrintInfo("Debug status is OFF");
                        }
                    }

                    return MS::kSuccess;
                }
                catch(...)
                {
                    const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                    PrintError(MString() + errorMessage.c_str());
                }
            }
            void CmdSetDebugMode::setResult(bool val) {}
            MSyntax CmdSetDebugMode::stateSyntax()
            {
                try
                {
                    MSyntax outSyntax;
                    outSyntax.addFlag(m_debugOnOffName, m_longDebugOnOffName, MSyntax::kBoolean);
                    return outSyntax;
                }
                catch(...)
                {
                    const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                    PrintError(MString() + errorMessage.c_str());
                }
            }
            void* CmdSetDebugMode::creator() { return (void*)(new CmdSetDebugMode); }
        }
    }
}