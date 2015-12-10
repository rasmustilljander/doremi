#pragma once

#include <string>

struct ID3D11Buffer;

namespace DoremiEngine
{
    namespace Graphic
    {
        class MeshInfo
        {
            public:
            const virtual size_t GetVerticeCount() = 0;
            virtual ID3D11Buffer* GetBufferHandle() = 0;
            const virtual std::string GetFileName() = 0;

            virtual void SetVerticeCount(size_t p_verticeCount) = 0;
            virtual void SetFileName(std::string p_fileName) = 0;
            virtual void SetBufferHandle(ID3D11Buffer* p_bufferHandle) = 0;
        };
    }
}