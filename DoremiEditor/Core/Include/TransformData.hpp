#pragma once
#include <DoremiEditor/Core/Include/SharedVariables.hpp>
#include <DoremiEditor/Core/Include/CustomAttributes.hpp>
#include <DirectXMath.h>

namespace DoremiEditor
{
    namespace Core
    {
        struct TransformData
        {
            DirectX::XMFLOAT3 translation;
            DirectX::XMFLOAT4 rotation;
            DirectX::XMFLOAT3 scale;
            CustomAttributes attributes;
        };
    }
}