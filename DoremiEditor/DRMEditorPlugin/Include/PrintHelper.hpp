#pragma once
#ifndef PRINTHELPER_HPP
#define PRINTHELPER_HPP
#include <DoremiEditor\DRMEditorPlugin\Include\Includes.hpp>
namespace DoremiEditor
{
    namespace Plugin
    {
        // om problem sätt inline framför void bitch
        void PrintInfo(MString p_str);
        void PrintError(MString p_str);
        void PrintWarning(MString p_str);
        void PrintDebug(MString p_str);
        void PrintMessage(MessageHeader p_header, std::string p_nodeName, std::string p_otherName = "", int p_parentCount = 0);
        MString GetNameStrings(const std::string p_name1, const std::string p_name2);
    }
}


#endif