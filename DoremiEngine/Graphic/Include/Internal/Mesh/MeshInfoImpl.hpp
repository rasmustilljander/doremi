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
            const size_t& GetIndexCount() const override { return m_indexCount; }
            ID3D11Buffer* GetBufferHandle() const override { return m_bufferHandle; }
            const std::string& GetFileName() const override { return m_fileName; };
            ID3D11Buffer* GetIndexBufferHandle() const override { return m_indexBufferHandle; }

            // Set
            void SetVerticeCount(size_t p_verticeCount) { m_verticeCount = p_verticeCount; }
            void SetIndexCount(size_t p_indexCount) { m_indexCount = p_indexCount; }
            void SetFileName(std::string p_fileName) { m_fileName = p_fileName; }
            void SetBufferHandle(ID3D11Buffer* p_bufferHandle);
            void SetIndexBufferHandle(ID3D11Buffer* p_bufferHandle) { m_indexBufferHandle = p_bufferHandle; };

        private:
            ID3D11Buffer* m_indexBufferHandle;
            size_t m_verticeCount;
            size_t m_indexCount;
            ID3D11Buffer* m_bufferHandle;
            std::string m_fileName;
        };
    }
}