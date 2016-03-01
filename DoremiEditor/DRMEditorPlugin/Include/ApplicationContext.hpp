#pragma once
#ifndef APPLICATIONCONTEXT_HPP
#define APPLICATIONCONTEXT_HPP
#include <DoremiEditor\DRMEditorPlugin\Include\Includes.hpp>
#include <DoremiEditor\DRMEditorPlugin\Include\PrintHelper.hpp>
namespace DoremiEditor
{
    namespace Plugin
    {
        class NodeHandler;
        class MessageHandler;
        class CallbackHandler;
        class MessageBuilder;
        class Filemapping;
        class ApplicationContext
        {
        public:
            static void Startup();
            static void Shutdown();

            static ApplicationContext& GetInstance() { return *applicationContext; }
            NodeHandler* GetNodeHandler() const { return m_nodeHandler; }
            MessageHandler* GetMessageHandler() const { return m_messageHandler; }
            CallbackHandler* GetCallbackHandler() const { return m_callbackHandler; }
            MessageBuilder* GetMessageBuilder() const { return m_messageBuilder; }
            Filemapping* GetFilemapping() const { return m_filemapping; }
            bool GetDebugStatus() { return m_debugMode; }
            void SetDebugStatus(bool p_debug) { m_debugMode = p_debug; }
            void InitializeClasses();

        private:
            bool m_debugMode;
            NodeHandler* m_nodeHandler = nullptr;
            MessageHandler* m_messageHandler = nullptr;
            CallbackHandler* m_callbackHandler = nullptr;
            MessageBuilder* m_messageBuilder = nullptr;
            Filemapping* m_filemapping = nullptr;
            ApplicationContext();
            ~ApplicationContext();
            ApplicationContext(ApplicationContext const&) = delete;
            void operator=(ApplicationContext const&) = delete;
            static ApplicationContext* applicationContext;
        };
    }
}

#endif
