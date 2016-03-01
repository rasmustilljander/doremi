#include <DoremiEditor\DRMEditorPlugin\Include\PrintHelper.hpp>
#include <DoremiEditor\DRMEditorPlugin\Include\ApplicationContext.hpp>
#include <DoremiEditor\DRMEditorPlugin\Include\MessageHandler.hpp>
namespace DoremiEditor
{
    namespace Plugin
    {
        void PrintInfo(MString p_str) { MGlobal::displayInfo("[Editor] " + p_str); }
        void PrintError(MString p_str) { MGlobal::displayError("[Editor Error] " + p_str); }
        void PrintWarning(MString p_str) { MGlobal::displayWarning("[Editor Warning] " + p_str); }
        void PrintDebug(MString p_str)
        {
            if(ApplicationContext::GetInstance().GetDebugStatus())
            {
                MGlobal::displayInfo("[Editor Debug] " + p_str);
            }
            else
            {
            }
        }
        void PrintMessage(MessageHeader p_header, std::string p_nodeName, std::string p_otherName, int p_parentCount)
        {
            // Only print these messages if debug status is OFF
            if(!ApplicationContext::GetInstance().GetDebugStatus())
            {
                MString o_string = "[DRM Message] ";
                o_string += ApplicationContext::GetInstance().GetMessageHandler()->m_nodeTypes[p_header.nodeType].c_str();
                o_string += " " + MString(ApplicationContext::GetInstance().GetMessageHandler()->m_msgTypes[p_header.messageType].c_str());
                o_string += " || " + MString(p_nodeName.c_str());
                if(p_parentCount != 0 && p_otherName != "")
                {
                    o_string += " || Parent Count: " + MString() + p_parentCount;
                }
                else if(p_parentCount == 0 && p_otherName != "")
                {
                    o_string += " || Parent Count: 1*";
                }
                o_string += +" || " + MString(p_otherName.c_str());
                MGlobal::displayInfo(o_string);
            }
        }

        MString GetNameStrings(const std::string p_name1, const std::string p_name2)
        {
            MString str = (" ( " + MString(p_name1.c_str()) + " ) -> ( " + MString(p_name2.c_str()) + " ) ");
            return str;
        }
    }
}