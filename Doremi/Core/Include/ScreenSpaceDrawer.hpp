
namespace DoremiEngine
{
    namespace Graphic
    {
        class DepthStencilState;
        class RasterizerState;
        class PixelShader;
        class VertexShader;
    }
    namespace Core
    {
        class SharedContext;
    }
}

namespace Doremi
{
    namespace Core
    {
        class ScreenSpaceDrawer
        {
        public:
            ScreenSpaceDrawer(const DoremiEngine::Core::SharedContext& p_sharedContext);
            virtual ~ScreenSpaceDrawer();
            void Draw();

        private:
            const DoremiEngine::Core::SharedContext& m_sharedContext;
            // Help functions
            void DrawMainMenu();
            // Variables
            DoremiEngine::Graphic::PixelShader* m_menuPixelShader;
            DoremiEngine::Graphic::VertexShader* m_menuVertexShader;
        };
    }
}