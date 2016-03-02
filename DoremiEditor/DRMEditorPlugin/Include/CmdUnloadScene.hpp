#pragma once
#ifndef CMDUNLOADSCENE_HPP
#define CMDUNLOADSCENE_HPP
#include <DoremiEditor\DRMEditorPlugin\Include\Includes.hpp>
namespace DoremiEditor
{
    namespace Plugin
    {
        namespace Commands
        {
            class MessageHandler;
            class CmdUnloadScene : public MPxCommand
            {
            public:
                CmdUnloadScene();
                virtual ~CmdUnloadScene();
                MStatus doIt(const MArgList& args);
                static void* creator();
            };
        }
    }
}

#endif