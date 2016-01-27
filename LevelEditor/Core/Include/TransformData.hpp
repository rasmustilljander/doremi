#pragma once
#include <LevelEditor/Core/Include/SharedVariables.hpp>
#include <LevelEditor/Core/Include/CustomAttributes.hpp>
#include <DirectXMath.h>

namespace DoremiEditor
{
    namespace Core
    {
        struct TransformData
        {
            DirectX::XMFLOAT2 pos;
            DirectX::XMFLOAT4 rot;
            DirectX::XMFLOAT3 scale;
            CustomAttributes attributes;
        };
    }
}