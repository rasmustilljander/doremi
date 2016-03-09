#pragma once
#include <DoremiEditor/Core/Include/SharedVariables.hpp>
#include <DoremiEditor/Core/Include/MaterialData.hpp>

namespace DoremiEditor
{
    namespace Core
    {
        struct MaterialMessage
        {
            void SetColor(float r, float g, float b) { data.setColor(r, g, b); }
            void SetAlpha(float a) { data.SetAlpha(a); }
            void AlphaFade() { data.AlphaFade(); }

            MaterialMessage()
                : nodeName(nullptr), diffuseTexturePath(nullptr), glowTexturePath(nullptr), specTexturePath(nullptr), bumpTexturePath(nullptr)
            {
            }

            ~MaterialMessage() {}

            char* nodeName;
            char* diffuseTexturePath;
            char* glowTexturePath;
            char* specTexturePath;
            char* bumpTexturePath;
            int type;
            float pad[2];
            MaterialData data;
        };
    }
}