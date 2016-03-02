#include <DoremiEditor\DRMEditorPlugin\Include\MessageHandler.hpp>
#include <DoremiEditor\DRMEditorPlugin\Include\NodeHandler.hpp>
#include <DoremiEditor\DRMEditorPlugin\Include\ApplicationContext.hpp>
#include <DoremiEditor\DRMEditorPlugin\Include\MessageBuilder.hpp>
#include <DoremiEditor\DRMEditorPlugin\Include\Filemapping.hpp>

#include "MessageHandler.hpp"
#include "NodeHandler.hpp"
#include "ApplicationContext.hpp"
#include "MessageBuilder.hpp"
#include "Filemapping.hpp"
namespace DoremiEditor
{
    namespace Plugin
    {
        MessageHandler::MessageHandler()
        {
            m_msgTypes.push_back("Invalid msg type");
            m_msgTypes.push_back("Added");
            m_msgTypes.push_back("Edited");
            m_msgTypes.push_back("Deleted");
            m_msgTypes.push_back("Renamed");
            m_msgTypes.push_back("Switched");

            m_nodeTypes.push_back("NO NODE TYPE");
            m_nodeTypes.push_back("Mesh");
            m_nodeTypes.push_back("Transform");
            m_nodeTypes.push_back("Camera");
            m_nodeTypes.push_back("Light");
            m_nodeTypes.push_back("Material");
        }
        MessageHandler::~MessageHandler() { m_msgVector.clear(); }
        void MessageHandler::Initialize()
        {
            m_nodeHandler = ApplicationContext::GetInstance().GetNodeHandler();
            m_messageBuilder = ApplicationContext::GetInstance().GetMessageBuilder();
            m_filemapping = ApplicationContext::GetInstance().GetFilemapping();
        }
        bool MessageHandler::AddMessage(const std::string p_nodeName, const NodeType p_nodeType, const MessageType p_messageType, const std::string p_secondName)
        {
            try
            {
                bool result = false;
                switch(p_messageType)
                {
                    case(MessageType::msgAdded):
                        if(SendInstantMessage(p_nodeName, p_nodeType, p_messageType, p_secondName))
                        {
                            result = true;
                        }
                        break;
                    case(MessageType::msgEdited):
                        AddDelayedMessage(p_nodeName, p_nodeType, p_messageType, p_secondName);
                        result = true;
                        break;
                    case(MessageType::msgSwitched):
                        if(SendInstantMessage(p_nodeName, p_nodeType, p_messageType, p_secondName))
                        {
                            result = true;
                        }
                        break;
                    case(MessageType::msgRenamed):
                        RemoveMessage(p_nodeName);
                        if(SendInstantMessage(p_nodeName, p_nodeType, p_messageType, p_secondName))
                        {
                            result = true;
                        }
                        break;
                    case(MessageType::msgDeleted):
                        RemoveMessage(p_nodeName);
                        if(SendInstantMessage(p_nodeName, p_nodeType, p_messageType, p_secondName))
                        {
                            result = true;
                        }
                        break;
                }
                return result;
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }

        bool MessageHandler::RemoveMessage(const std::string p_nodeName)
        {
            try
            {
                for(std::vector<MessageInfo>::size_type i = 0; i < m_msgVector.size(); ++i)
                {
                    if(strcmp(p_nodeName.c_str(), m_msgVector.at(i).nodeName.c_str()) == 0)
                    {
                        PrintInfo("Removed message from queue ( " + MString(p_nodeName.c_str()) + " )");
                        m_msgVector.erase(m_msgVector.begin() + i);
                    }
                }
                return true;
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
            return true;
        }
        void MessageHandler::ClearMessageVectors()
        {
            try
            {
                m_msgVector.clear();
                PrintInfo("Message Vectors cleared!");
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        bool MessageHandler::SendInstantMessage(const std::string p_nodeName, const NodeType p_nodeType, const MessageType p_messageType, const std::string p_secondName)
        {
            try
            {
                MessageInfo t_messageInfo;
                bool t_addToVector = false;
                t_messageInfo.nodeName = p_nodeName;
                t_messageInfo.nodeType = p_nodeType;
                t_messageInfo.msgType = p_messageType;
                t_messageInfo.oldName = p_secondName;
                if(p_messageType == MessageType::msgDeleted || p_messageType == MessageType::msgRenamed)
                {
                    if(!m_filemapping->TrySendRenameDelete(t_messageInfo))
                    {
                        t_addToVector = true;
                    }
                }
                else
                {
                    switch(p_nodeType)
                    {
                        case(NodeType::nTransform):
                            if(!m_filemapping->TrySendTransform(t_messageInfo))
                            {
                                t_addToVector = true;
                            }
                            break;
                        case(NodeType::nMesh):
                            if(!m_filemapping->TrySendMesh(t_messageInfo))
                            {
                                t_addToVector = true;
                            }
                            break;
                        case(NodeType::nCamera):
                            if(!m_filemapping->TrySendCamera(t_messageInfo))
                            {
                                t_addToVector = true;
                            }
                            break;
                        case(NodeType::nLight):
                            if(!m_filemapping->TrySendLight(t_messageInfo))
                            {
                                t_addToVector = true;
                            }
                            break;
                        case(NodeType::nMaterial):
                            if(!m_filemapping->TrySendMaterial(t_messageInfo))
                            {
                                t_addToVector = true;
                            }
                            break;
                    }
                }

                if(t_addToVector)
                {
                    AddDelayedMessage(t_messageInfo.nodeName, t_messageInfo.nodeType, t_messageInfo.msgType, t_messageInfo.oldName);
                }
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
            return true;
        }

        void MessageHandler::AddDelayedMessage(const std::string p_nodeName, const NodeType p_nodeType, const MessageType p_messageType, const std::string p_secondName)
        {
            try
            {
                bool t_add = true;
                for(std::vector<MessageInfo>::size_type i = 0; i < m_msgVector.size(); ++i)
                {
                    if(strcmp(p_nodeName.c_str(), m_msgVector.at(i).nodeName.c_str()) == 0)
                    {
                        if(m_msgVector.at(i).msgType == MessageType::msgEdited && p_messageType == MessageType::msgEdited)
                        {
                            t_add = false;
                        }
                    }
                }
                if(t_add)
                {
                    MessageInfo t_messageInfo;
                    t_messageInfo.nodeName = p_nodeName;
                    t_messageInfo.nodeType = p_nodeType;
                    t_messageInfo.msgType = p_messageType;
                    t_messageInfo.oldName = p_secondName;
                    m_msgVector.push_back(t_messageInfo);
                    PrintDebug("Added message! NodeType: " + MString(m_nodeTypes[static_cast<int>(p_nodeType)].c_str()) + " Message Type: " +
                               MString(m_msgTypes[static_cast<int>(p_messageType)].c_str()));
                }
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        bool MessageHandler::SendDelayedMessages()
        {
            try
            {
                bool result = false;
                bool t_run = true;
                // Transfer vector to queue
                for(std::vector<MessageInfo>::size_type i = 0; i < m_msgVector.size(); ++i)
                {
                    m_msgQueue.push(m_msgVector.at(i));
                }
                m_msgVector.clear();
                // Loop through queue and send messages accordingly. If a message is unable to be sent, the process is canceled
                if(m_filemapping->GetFilemapStatus())
                {
                    while(!m_msgQueue.empty() && t_run == true)
                    {
                        if(m_msgQueue.front().msgType == MessageType::msgDeleted || m_msgQueue.front().msgType == MessageType::msgRenamed)
                        {
                            if(m_filemapping->TrySendRenameDelete(m_msgQueue.front()))
                            {
                                m_msgQueue.pop();
                                result = true;
                            }
                            else
                            {
                                t_run = false;
                            }
                        }
                        else
                        {
                            switch(m_msgQueue.front().nodeType)
                            {
                                case NodeType::nTransform:
                                    if(m_filemapping->TrySendTransform(m_msgQueue.front()))
                                    {
                                        m_msgQueue.pop();
                                        result = true;
                                    }
                                    else
                                    {
                                        t_run = false;
                                    }
                                    break;
                                case NodeType::nMesh:
                                    if(m_filemapping->TrySendMesh(m_msgQueue.front()))
                                    {
                                        m_msgQueue.pop();
                                        result = true;
                                    }
                                    else
                                    {
                                        t_run = false;
                                    }
                                    break;
                                case NodeType::nCamera:
                                    if(m_filemapping->TrySendCamera(m_msgQueue.front()))
                                    {
                                        m_msgQueue.pop();
                                        result = true;
                                    }
                                    else
                                    {
                                        t_run = false;
                                    }
                                    break;
                                case NodeType::nLight:
                                    if(m_filemapping->TrySendLight(m_msgQueue.front()))
                                    {
                                        m_msgQueue.pop();
                                        result = true;
                                    }
                                    else
                                    {
                                        t_run = false;
                                    }
                                    break;
                                case NodeType::nMaterial:
                                    if(m_filemapping->TrySendMaterial(m_msgQueue.front()))
                                    {
                                        m_msgQueue.pop();
                                        result = true;
                                    }
                                    else
                                    {
                                        t_run = false;
                                    }
                                    break;
                            }
                        }
                    }
                    // If not all messages was able to be sent, repopulate vector from queue.
                    if(!m_msgQueue.empty() && t_run == false)
                    {
                        while(!m_msgQueue.empty())
                        {
                            m_msgVector.push_back(m_msgQueue.front());
                            m_msgQueue.pop();
                        }
                    }
                }
                return result;
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        void MessageHandler::PrintVectorInfo(bool p_printMessages)
        {
            try
            {
                PrintInfo("Messages in message vector: " + MString() + m_msgVector.size());
                if(p_printMessages)
                {
                    for(std::vector<MessageInfo>::size_type i = 0; i < m_msgVector.size(); ++i)
                    {
                        MString str = MString() + i + ". ";
                        str += MString(m_nodeTypes.at(static_cast<int>(m_msgVector.at(i).nodeType)).c_str()) + " ";
                        str += MString(m_msgTypes.at(static_cast<int>(m_msgVector.at(i).msgType)).c_str()) + " ";
                        str += "( " + MString(m_msgVector.at(i).nodeName.c_str()) + " )";
                        if(m_msgVector.at(i).oldName.length() > 0)
                        {
                            str += " Other name : ( " + MString(m_msgVector.at(i).oldName.c_str()) + " )";
                        }
                        PrintInfo(str);
                    }
                }
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
    }
}