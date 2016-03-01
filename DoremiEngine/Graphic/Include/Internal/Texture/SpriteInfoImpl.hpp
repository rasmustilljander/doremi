#pragma once
#include <Interface/Texture/SpriteInfo.hpp>


namespace DoremiEngine
{
    namespace Graphic
    {
        class SpriteInfoImpl : public SpriteInfo
        {
        public:
            SpriteInfoImpl();
            virtual ~SpriteInfoImpl();

            // Get
            ID3D11Buffer* GetBufferHandle() const override { return m_bufferHandle; }
            const std::string& GetFileName() const override { return m_fileName; };

            // Set
            void SetFileName(std::string p_fileName) { m_fileName = p_fileName; }
            void SetBufferHandle(ID3D11Buffer* p_bufferHandle);

        private:
            ID3D11Buffer* m_bufferHandle;
            std::string m_fileName;
        };
    }
}