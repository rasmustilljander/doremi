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
            SpriteData& GetData() override { return m_datainfo; }
            const std::string& GetFileName() const override { return m_fileName; };

            // Set
            void SetFileName(std::string p_fileName) { m_fileName = p_fileName; }
            void SetData(const SpriteData& p_spriteData);

        private:
            SpriteData m_datainfo;
            std::string m_fileName;
        };
    }
}