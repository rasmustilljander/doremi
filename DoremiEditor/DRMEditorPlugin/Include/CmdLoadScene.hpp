#pragma once
#ifndef CMDLOADSCENE_HPP
#define CMDLOADSCENE_HPP
#include <DoremiEditor\DRMEditorPlugin\Include\Includes.hpp>
namespace DoremiEditor
{
    namespace Plugin
    {
        namespace Commands
        {
            class MessageHandler;
            class CmdLoadScene : public MPxCommand
            {
            public:
                CmdLoadScene();
                virtual ~CmdLoadScene();
                MStatus doIt(const MArgList& args);
                static void* creator();
            };
        }
    }
}

#endif