#pragma once
#include <Interface/Manager/Texture2DManager.hpp>
#include <string>
namespace DoremiEngine
{
    namespace Graphic
    {
        struct GraphicModuleContext;
        class Texture2DManagerImpl : public Texture2DManager
        {
            public:
            Texture2DManagerImpl(const GraphicModuleContext& p_graphicContext);
            virtual ~Texture2DManagerImpl();
            Texture2D* BuildTexture2D(const std::string& m_fileName) override;

            private:
            const GraphicModuleContext& m_graphicContext;
        };
    }
}