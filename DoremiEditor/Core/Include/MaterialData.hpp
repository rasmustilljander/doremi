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
                diffuse = 0.0f;
                color[0] = color[1] = color[2] = 0.1f;
                ambColor[0] = ambColor[1] = ambColor[2] = 0.0f;
                specColor[0] = specColor[1] = specColor[2] = 0.0f;
                specCosine = specEccentricity = specRollOff = 0.0f;
                alpha = 1.f;
            }
            void setColor(float r, float g, float b)
            {
                color[0] = r;
                color[1] = g;
                color[2] = b;
            }

            void SetAlpha(float a) { alpha = a; }

            int mapMasks;
            float color[3];
            float diffuse;
            float ambColor[3];
            float specCosine;
            float specColor[3];
            float specEccentricity;
            float specRollOff;
            float alpha;
            float pad;
        };
    }
}