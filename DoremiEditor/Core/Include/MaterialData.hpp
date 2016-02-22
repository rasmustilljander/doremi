#pragma once
#include <DoremiEditor/Core/Include/SharedVariables.hpp>

namespace DoremiEditor
{
    namespace Core
    {
        enum class ActiveTextures : int
        {
            COLORMAP = 0x01,
            GLOWMAP = 0x02,
            SPECMAP = 0x04,
            BUMPMAP = 0x08
        };
        struct MaterialData
        {
            MaterialData()
            {
                mapMasks = 0;
                diffuse = 0;
                color[0] = color[1] = color[2] = 0.5f;
                ambColor[0] = ambColor[1] = ambColor[2] = 0.0f;
                specColor[0] = specColor[1] = specColor[2] = 0.0f;
                specCosine = specEccentricity = specRollOff = 0;
            }
            float mapMasks;
            float color[3];
            float diffuse;
            float ambColor[3];
            float specCosine;
            float specColor[3];
            float specEccentricity;
            float specRollOff;
            float pad[2];
        };
    }
}