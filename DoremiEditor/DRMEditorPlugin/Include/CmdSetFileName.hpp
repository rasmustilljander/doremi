#pragma once
#ifndef CMDSETFILENAME_HPP
#define CMDSETFILENAME_HPP
#include <DoremiEditor\DRMEditorPlugin\Include\Includes.hpp>
namespace DoremiEditor
{
    namespace Plugin
    {
        namespace Commands
        {
            class MessageHandler;
            class CmdSetFileName : public MPxCommand
            {
            public:
                CmdSetFileName();
                virtual ~CmdSetFileName();
                MStatus doIt(const MArgList& args);
                static void* creator();
                static MSyntax nameSyntax();
            };
        }
    }
}

#endif