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

            //Get
            size_t GetVerticeCount() { return m_verticeCount; }
            ID3D11Buffer* GetBufferHandle() { return m_bufferHandle; } //TODOKO Might not work because of pre declaration
            std::string GetFileName() { return m_fileName; };

            //Set
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