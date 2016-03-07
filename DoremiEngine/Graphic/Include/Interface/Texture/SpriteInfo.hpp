#pragma once

#include <string>
#include <DirectXMath.h>

struct ID3D11Buffer;

namespace DoremiEngine
{
    namespace Graphic
    {
        struct SpriteData
        {
            SpriteData() {}
            SpriteData(const SpriteData& p_spriteData)
                : position(p_spriteData.position),
                  origo(p_spriteData.origo),
                  halfsize(p_spriteData.halfsize),
                  txtPos(p_spriteData.txtPos),
                  txtSize(p_spriteData.txtSize),
                  filler(DirectX::XMFLOAT2(0, 0))
            {
            }

            DirectX::XMFLOAT2 position;
            DirectX::XMFLOAT2 origo;

            DirectX::XMFLOAT2 halfsize;
            DirectX::XMFLOAT2 txtPos;

            DirectX::XMFLOAT2 txtSize;
            DirectX::XMFLOAT2 filler;
        };

        class SpriteInfo
        {
        public:
            virtual SpriteData& GetData() = 0;
            virtual const std::string& GetFileName() const = 0;

            virtual void SetFileName(std::string p_fileName) = 0;
            virtual void SetData(const SpriteData& p_spriteData) = 0;
        };
    }
}