#include "CmdSetFileName.hpp"
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
            const char* m_setNameLong = "-name";
            const char* m_setName = "-n";
            CmdSetFileName::CmdSetFileName() {}
            CmdSetFileName::~CmdSetFileName() {}
            MStatus CmdSetFileName::doIt(const MArgList& args)
            {
                try
                {
                    MStatus result;
                    MArgDatabase t_parser(syntax(), args, &result);
                    MString t_fileMstring;
                    if(result != MS::kSuccess)
                    {
                        return result;
                    }
                    if(t_parser.isFlagSet(m_setName))
                    {
                        t_parser.getFlagArgument(m_setName, 0, t_fileMstring);
                        // PrintInfo(t_fileMstring);
                        std::string t_fileStr = t_fileMstring.asChar();
                        ApplicationContext::GetInstance().GetFilemapping()->SetExportFileName(t_fileStr);
                    }
                    else
                    {
                        ApplicationContext::GetInstance().GetFilemapping()->PrintFilemapInfo(true, true);
                    }
                    // ApplicationContext::GetInstance().GetMessageHandler()->PrintVectorInfo(true);
                    /*			ApplicationContext::GetInstance().GetCallbackHandler()->UnloadScene();
                                ApplicationContext::GetInstance().GetNodeHandler()->PrintVectorInfo();*/
                    return MS::kSuccess;
                }

                catch(...)
                {
                    const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                    PrintError(MString() + errorMessage.c_str());
                }
            }

            void* CmdSetFileName::creator() { return (void*)(new CmdSetFileName); }

            MSyntax CmdSetFileName::nameSyntax()
            {
                try
                {
                    MSyntax outSyntax;
                    outSyntax.addFlag(m_setName, m_setNameLong, MSyntax::kString);


                    return outSyntax;
                }
                catch(...)
                {
                    const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                    PrintError(MString() + errorMessage.c_str());
                }
            }
        }
    }
}