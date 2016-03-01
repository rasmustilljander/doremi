#pragma once
#ifndef CMDRESETMESSAGES_HPP
#define CMDRESETMESSAGES_HPP
#include <DoremiEditor\DRMEditorPlugin\Include\Includes.hpp>
namespace DoremiEditor
{
    namespace Plugin
    {
        namespace Commands
        {
            class MessageHandler;
            class CmdResetMessages : public MPxCommand
            {
            public:
                CmdResetMessages();
                virtual ~CmdResetMessages();
                MStatus doIt(const MArgList& args);
                static void* creator();
            };
        }
    }
}

#endif