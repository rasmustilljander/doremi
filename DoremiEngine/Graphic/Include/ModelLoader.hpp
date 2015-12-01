#pragma once

#include <string>
namespace DoremiEngine
{
    namespace Graphic
    {
        class ModelLoader
        {
        public:
            ModelLoader();
            ~ModelLoader();
            //Loads the mesh specified by filename and pushes vertices to device and returns mesh id
            int LoadMesh(const std::string& p_fileName); //TODORK Implement load
            int LoadMaterial(const std::string& p_materialName);
        };
    }
}
