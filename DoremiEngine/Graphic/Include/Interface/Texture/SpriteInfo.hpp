#pragma once

#include <string>

struct ID3D11Buffer;

namespace DoremiEngine
{
    namespace Graphic
    {
        class SpriteInfo
        {
        public:
            virtual ID3D11Buffer* GetBufferHandle() const = 0;
            virtual const std::string& GetFileName() const = 0;

            virtual void SetFileName(std::string p_fileName) = 0;
            virtual void SetBufferHandle(ID3D11Buffer* p_bufferHandle) = 0;
        };
    }
}