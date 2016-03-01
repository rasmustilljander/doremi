#pragma once
#ifndef CMDSETDEBUGMODE_HPP
#define CMDSETDEBUGMODE_HPP
#include <DoremiEditor\DRMEditorPlugin\Include\Includes.hpp>
namespace DoremiEditor
{
    namespace Plugin
    {
        namespace Commands
        {
            class Filemapping;
            class CmdSetDebugMode : public MPxCommand
            {
            public:
                CmdSetDebugMode();
                virtual ~CmdSetDebugMode();
                MStatus doIt(const MArgList& args);
                static void setResult(bool val);
                static void* creator();
                static MSyntax stateSyntax();
            };
        }
    }
}

#endif #pragma once
