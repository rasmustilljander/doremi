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
            virtual const size_t& GetVerticeCount() const = 0;
            virtual const size_t& GetIndexCount() const = 0;
            virtual ID3D11Buffer* GetBufferHandle() const = 0;
            virtual ID3D11Buffer* GetIndexBufferHandle() const = 0;
            virtual const std::string& GetFileName() const = 0;


            virtual void SetIndexBufferHandle(ID3D11Buffer* p_bufferHandle) = 0;
            virtual void SetVerticeCount(size_t p_verticeCount) = 0;
            virtual void SetIndexCount(size_t p_indexCount) = 0;
            virtual void SetFileName(std::string p_fileName) = 0;
            virtual void SetBufferHandle(ID3D11Buffer* p_bufferHandle) = 0;
        };
    }
}