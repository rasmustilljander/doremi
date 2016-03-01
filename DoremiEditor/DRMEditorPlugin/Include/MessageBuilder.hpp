#pragma once
#ifndef MESSAGEBUILDER_HPP
#define MESSAGEBUILDER_HPP
#include <DoremiEditor\DRMEditorPlugin\Include\Includes.hpp>
namespace DoremiEditor
{
    namespace Plugin
    {
        class NodeHandler;
        class MessageBuilder
        {
        public:
            TransformInfo GetTransformData(std::string p_nodeName);
            MeshInfo GetMeshData(std::string p_nodeName);
            CameraInfo GetCameraData(std::string p_nodeName);
            LightInfo GetLightData(std::string p_nodeName);
            MaterialInfo GetMaterialData(std::string p_nodeName);
            size_t GetMeshDataSize(std::string p_nodeName);
            size_t GetTransformDataSize();
            size_t GetCameraDataSize();
            size_t GetLightDataSize();
            size_t GetMaterialDataSize();
        };
    }
}


#endif