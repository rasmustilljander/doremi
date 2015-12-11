#pragma once
#include <Interface/Mesh/MeshInfo.hpp>


namespace DoremiEngine
{
    namespace Graphic
    {
        class MeshInfoImpl : public MeshInfo
        {
            public:
            MeshInfoImpl();
            virtual ~MeshInfoImpl();

            // Get
            const size_t& GetVerticeCount() const override { return m_verticeCount; }
            ID3D11Buffer* GetBufferHandle() const override { return m_bufferHandle; }
            const std::string& GetFileName() const override { return m_fileName; };

            // Set
            void SetVerticeCount(size_t p_verticeCount) { m_verticeCount = p_verticeCount; }
            void SetFileName(std::string p_fileName) { m_fileName = p_fileName; }
            void SetBufferHandle(ID3D11Buffer* p_bufferHandle);

            private:
            size_t m_verticeCount;
            ID3D11Buffer* m_bufferHandle;
            std::string m_fileName;
        };
    }
}