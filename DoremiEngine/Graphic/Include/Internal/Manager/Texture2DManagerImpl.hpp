#pragma once
#include <Interface/Manager/Texture2DManager.hpp>
#include <string>
namespace DoremiEngine
{
    namespace Graphic
    {
        class Texture2DManagerImpl : public Texture2DManager
        {
        public:
            Texture2DManagerImpl();
            virtual ~Texture2DManagerImpl();
            Texture2D* BuildTexture2D(const std::string& m_fileName) override;
        };
    }
}