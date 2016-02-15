#include <DirectXMath.h>
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
        class VictoryScreen;

        class ScreenSpaceDrawer
        {
        public:
            ScreenSpaceDrawer(const DoremiEngine::Core::SharedContext& p_sharedContext, DirectX::XMFLOAT2 p_resolution);
            virtual ~ScreenSpaceDrawer();
            void Draw();

        private:
            const DoremiEngine::Core::SharedContext& m_sharedContext;
            // Help functions
            void DrawMainMenu();
            void DrawVictoryScreen();
            void CreateVictoryScreen();
            DirectX::XMFLOAT2 ConvertWithResolution(DirectX::XMFLOAT2 t_point);
            // Variables
            DoremiEngine::Graphic::PixelShader* m_menuPixelShader;
            DoremiEngine::Graphic::VertexShader* m_menuVertexShader;
            VictoryScreen* m_victoryScreen;
            DirectX::XMFLOAT2 m_resolution;
        };
    }
}