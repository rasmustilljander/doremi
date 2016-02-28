#pragma once
#include <ConfigurationModule.hpp>
#include <map>
#include <string>
#include <iostream>

namespace DoremiEngine
{
    namespace Configuration
    {
        static void InterpretMap(const std::map<std::string, std::string>& p_mapToInterpret, ConfiguartionInfo& o_info)
        {
            if(p_mapToInterpret.count("ScreenHeight"))
            {
                o_info.ScreenHeight = std::stof(p_mapToInterpret.at("ScreenHeight"));
            }
            if(p_mapToInterpret.count("ScreenWidth"))
            {
                o_info.ScreenWidth = std::stof(p_mapToInterpret.at("ScreenWidth"));
            }
            if(p_mapToInterpret.count("Volume"))
            {
                o_info.Volume = std::stof(p_mapToInterpret.at("Volume"));
            }
            if(p_mapToInterpret.count("Forward"))
            {
                o_info.Forward = std::stoi(p_mapToInterpret.at("Forward"));
            }
            if(p_mapToInterpret.count("Backward"))
            {
                o_info.Backward = std::stoi(p_mapToInterpret.at("Backward"));
            }
            if(p_mapToInterpret.count("Left"))
            {
                o_info.Left = std::stoi(p_mapToInterpret.at("Left"));
            }
            if(p_mapToInterpret.count("Right"))
            {
                o_info.Right = std::stoi(p_mapToInterpret.at("Right"));
            }
            if(p_mapToInterpret.count("LeftClick"))
            {
                o_info.LeftClick = std::stoi(p_mapToInterpret.at("LeftClick"));
            }
            if(p_mapToInterpret.count("RightClick"))
            {
                o_info.RightClick = std::stoi(p_mapToInterpret.at("RightClick"));
            }
            if(p_mapToInterpret.count("StartRepeatingAudioRecording"))
            {
                o_info.StartRepeatingAudioRecording = std::stoi(p_mapToInterpret.at("StartRepeatingAudioRecording"));
            }
            if(p_mapToInterpret.count("PlayRepeatableAudioRecording"))
            {
                o_info.PlayRepeatableAudioRecording = std::stoi(p_mapToInterpret.at("PlayRepeatableAudioRecording"));
            }
            if(p_mapToInterpret.count("ExitGame"))
            {
                o_info.ExitGame = std::stoi(p_mapToInterpret.at("ExitGame"));
            }
            if(p_mapToInterpret.count("CameraDistanceFromPlayer"))
            {
                o_info.CameraDistanceFromPlayer = std::stof(p_mapToInterpret.at("CameraDistanceFromPlayer"));
            }
            if(p_mapToInterpret.count("CameraSpeedY"))
            {
                o_info.CameraSpeedY = std::stof(p_mapToInterpret.at("CameraSpeedY"));
            }
            if(p_mapToInterpret.count("CameraViewDistance"))
            {
                o_info.CameraViewDistance = std::stoi(p_mapToInterpret.at("CameraViewDistance"));
            }
            if(p_mapToInterpret.count("CameraFieldOfView"))
            {
                o_info.CameraFieldOfView = std::stoi(p_mapToInterpret.at("CameraFieldOfView"));
            }
            if(p_mapToInterpret.count("UseIPMaster"))
            {
                o_info.UseIPMaster = std::stoi(p_mapToInterpret.at("UseIPMaster"));
            }
            if(p_mapToInterpret.count("URLMaster"))
            {
                o_info.URLMaster = p_mapToInterpret.at("URLMaster");
            }
            if(p_mapToInterpret.count("IPToMaster"))
            {
                o_info.IPToMaster = p_mapToInterpret.at("IPToMaster");
            }
            if(p_mapToInterpret.count("PortMasterClient"))
            {
                o_info.PortMasterClient = std::stoi(p_mapToInterpret.at("PortMasterClient"));
            }
            if(p_mapToInterpret.count("PortMasterServer"))
            {
                o_info.PortMasterServer = std::stoi(p_mapToInterpret.at("PortMasterServer"));
            }
            if(p_mapToInterpret.count("PortServerConnecting"))
            {
                o_info.PortServerConnecting = std::stoi(p_mapToInterpret.at("PortServerConnecting"));
            }
            if(p_mapToInterpret.count("PortServerConnected"))
            {
                o_info.PortServerConnected = std::stoi(p_mapToInterpret.at("PortServerConnected"));
            }
            if(p_mapToInterpret.count("IPToServer"))
            {
                o_info.IPToServer = p_mapToInterpret.at("IPToServer");
            }
            if (p_mapToInterpret.count("LastServerPlayerID"))
            {
                o_info.LastServerPlayerID = std::stoi(p_mapToInterpret.at("LastServerPlayerID"));
            }
            if(p_mapToInterpret.count("AIJumpDistance"))
            {
                o_info.AIJumpDistance = std::stof(p_mapToInterpret.at("AIJumpDistance"));
            }
            if(p_mapToInterpret.count("AIAimOffset"))
            {
                o_info.AIAimOffset = std::stof(p_mapToInterpret.at("AIAimOffset"));
            }
        }

        static std::map<std::string, std::string> SaveConfigToMap(const ConfiguartionInfo& p_info)
        {
            // All values saved in configuration needs to be castable to string
            std::map<std::string, std::string> returnMap;
            returnMap["ScreenHeight"] = std::to_string(p_info.ScreenHeight);
            returnMap["ScreenWidth"] = std::to_string(p_info.ScreenWidth);
            returnMap["Volume"] = std::to_string(p_info.Volume);
            returnMap["Forward"] = std::to_string(p_info.Forward);
            returnMap["Backward"] = std::to_string(p_info.Backward);
            returnMap["Left"] = std::to_string(p_info.Left);
            returnMap["Right"] = std::to_string(p_info.Right);
            returnMap["LeftClick"] = std::to_string(p_info.LeftClick);
            returnMap["RightClick"] = std::to_string(p_info.RightClick);
            returnMap["StartRepeatingAudioRecording"] = std::to_string(p_info.StartRepeatingAudioRecording);
            returnMap["PlayRepeatableAudioRecording"] = std::to_string(p_info.PlayRepeatableAudioRecording);
            returnMap["ExitGame"] = std::to_string(p_info.ExitGame);
            returnMap["CameraDistanceFromPlayer"] = std::to_string(p_info.CameraDistanceFromPlayer);
            returnMap["CameraSpeedY"] = std::to_string(p_info.CameraSpeedY);
            returnMap["CameraViewDistance"] = std::to_string(p_info.CameraViewDistance);
            returnMap["CameraFieldOfView"] = std::to_string(p_info.CameraFieldOfView);
            returnMap["UseIPMaster"] = std::to_string(p_info.UseIPMaster);
            returnMap["URLMaster"] = p_info.URLMaster;
            returnMap["IPToMaster"] = p_info.IPToMaster;
            returnMap["PortMasterClient"] = std::to_string(p_info.PortMasterClient);
            returnMap["PortMasterServer"] = std::to_string(p_info.PortMasterServer);
            returnMap["PortServerConnecting"] = std::to_string(p_info.PortServerConnecting);
            returnMap["PortServerConnected"] = std::to_string(p_info.PortServerConnected);
            returnMap["IPToServer"] = p_info.IPToServer;
            returnMap["LastServerPlayerID"] = std::to_string(p_info.LastServerPlayerID);
            returnMap["AIJumpDistance"] = std::to_string(p_info.AIJumpDistance);
            returnMap["AIAimOffset"] = std::to_string(p_info.AIAimOffset);
            return returnMap;
        }
    }
}