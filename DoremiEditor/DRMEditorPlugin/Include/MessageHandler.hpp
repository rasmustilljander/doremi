#pragma once
#ifndef MESSAGEHANDLER_HPP
#define MESSAGEHANDLER_HPP
#include <DoremiEditor\DRMEditorPlugin\Include\Includes.hpp>
#include <vector>
#include <queue>
namespace DoremiEditor
{
    namespace Plugin
    {
        class NodeHandler;
        class MessageBuilder;
        class Filemapping;
        class MessageHandler
        {
        private:
            std::vector<MessageInfo> m_msgVector;
            std::queue<MessageInfo> m_msgQueue;


            NodeHandler* m_nodeHandler;
            MessageBuilder* m_messageBuilder;
            Filemapping* m_filemapping;

        public:
            void Initialize();
            std::vector<std::string> m_msgTypes;
            std::vector<std::string> m_nodeTypes;
            bool AddMessage(const std::string p_nodeName, const NodeType p_nodeType, const MessageType p_messageType, const std::string p_secondName = "");
            bool RemoveMessage(const std::string p_nodeName);
            void ClearMessageVectors();
            bool SendInstantMessage(const std::string p_nodeName, const NodeType p_nodeType, const MessageType p_messageType, const std::string p_secondName = "");
            void AddDelayedMessage(const std::string p_nodeName, const NodeType p_nodeType, const MessageType p_messageType, const std::string p_secondName = "");
            void PrintVectorInfo(bool p_printMessages);
            bool SendDelayedMessages();

            MessageHandler();
            ~MessageHandler();
        };
    }
}

#endif