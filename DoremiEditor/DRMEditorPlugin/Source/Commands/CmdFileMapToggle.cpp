#include "CmdFileMapToggle.hpp"
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
            const char* m_longOnOffName = "-state";
            const char* m_onOffName = "-s";
            CmdFileMapToggle::CmdFileMapToggle() {}
            CmdFileMapToggle::~CmdFileMapToggle() {}
            MStatus CmdFileMapToggle::doIt(const MArgList& args)
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
                    if(t_parser.isFlagSet(m_onOffName))
                    {
                        bool fmStatus = ApplicationContext::GetInstance().GetFilemapping()->GetFilemapStatus();
                        t_parser.getFlagArgument(m_onOffName, 0, t_inputParameter);
                        if(t_inputParameter)
                        {

                            if(!fmStatus)
                            {
                                ApplicationContext::GetInstance().GetFilemapping()->CreateFilemaps();
                            }
                        }
                        else
                        {
                            if(fmStatus)
                            {
                                ApplicationContext::GetInstance().GetFilemapping()->CloseFilemaps();
                            }
                        }
                    }
                    // ApplicationContext::GetInstance().GetMessageHandler()->PrintVectorInfo(true);
                    /*bool status = ApplicationContext::GetInstance().GetFilemapping()->GetFilemapStatus();
                    if (status)
                    {
                        PrintDebug("Filemaps OFF");
                        ApplicationContext::GetInstance().GetFilemapping()->CloseFilemaps();

                    }
                    else
                    {
                        PrintDebug("Filemaps ON");
                        ApplicationContext::GetInstance().GetFilemapping()->CreateFilemaps();
                    }*/
                    return MS::kSuccess;
                }
                catch(...)
                {
                    const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                    PrintError(MString() + errorMessage.c_str());
                }
            }
            MSyntax CmdFileMapToggle::stateSyntax()
            {
                try
                {
                    MSyntax outSyntax;
                    outSyntax.addFlag(m_onOffName, m_longOnOffName, MSyntax::kBoolean);
                    return outSyntax;
                }
                catch(...)
                {
                    const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                    PrintError(MString() + errorMessage.c_str());
                }
            }
            void* CmdFileMapToggle::creator() { return (void*)(new CmdFileMapToggle); }
        }
    }
}