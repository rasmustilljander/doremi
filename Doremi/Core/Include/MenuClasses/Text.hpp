#pragma once
// Project specific
#include <DirectXMath.h>
#include <DoremiEngine/Graphic/Include/Interface/Mesh/MaterialInfo.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Texture/SpriteInfo.hpp>
#include <vector>

namespace DoremiEngine
{
    namespace Core
    {
        class SharedContext;
    }
}


namespace Doremi
{
    namespace Core
    {
        using namespace DirectX;

        class Text
        {
        public:
            Text(DoremiEngine::Graphic::MaterialInfo* p_textMaterial, DirectX::XMFLOAT2 p_textSize, XMFLOAT2 p_position, XMFLOAT2 p_origo,
                 XMFLOAT2 p_tableCharSize, XMFLOAT2 p_tableOffset);

            Text();

            ~Text();

            void SetText(const DoremiEngine::Core::SharedContext& p_sharedContext, std::string p_text);

            void UpdateText(const DoremiEngine::Core::SharedContext& p_sharedContext, const std::string& p_text);

            void UpdatePosition(XMFLOAT2 p_position);

            void DeleteText();

            DoremiEngine::Graphic::MaterialInfo* m_textMaterial;

            // Add "Mesh" info here, but its buffer
            std::vector<DoremiEngine::Graphic::SpriteInfo*> m_textInfo;

        private:
            std::string m_text;

            DirectX::XMFLOAT2 m_position;
            DirectX::XMFLOAT2 m_origo;
            DirectX::XMFLOAT2 m_textSize;
            DirectX::XMFLOAT2 m_tableCharSize;
            DirectX::XMFLOAT2 m_tableOffset;
        };
    }
}
