#include <DirectXMath.h>
namespace DoremiEngine
{
    namespace Graphic
    {
        class DepthStencilState;
        class RasterizerState;
        class PixelShader;
        class VertexShader;
        class GeometryShader;
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
            static ScreenSpaceDrawer* GetInstance();
            static void StartupScreenSpaceDrawer(const DoremiEngine::Core::SharedContext& p_sharedContext);


            void Draw();
            void DrawLoadingScreen();

        private:
            ScreenSpaceDrawer(const DoremiEngine::Core::SharedContext& p_sharedContext);
            virtual ~ScreenSpaceDrawer();
            static ScreenSpaceDrawer* m_singleton;

            const DoremiEngine::Core::SharedContext& m_sharedContext;
            // Help functions
            void DrawMainMenu();
            void DrawServerBrowser();

            void DrawHUD();
            void DrawVictoryScreen();

            void Begin2DDraw();

            void End2DDraw();

            DirectX::XMFLOAT2 ConvertWithResolution(DirectX::XMFLOAT2 t_point);
            // Variables
            DoremiEngine::Graphic::PixelShader* m_menuPixelShader;
            DoremiEngine::Graphic::VertexShader* m_menuVertexShader;
            DoremiEngine::Graphic::GeometryShader* m_menuGeometryShader;
            DoremiEngine::Graphic::PixelShader* m_postPixelShader;
            DoremiEngine::Graphic::VertexShader* m_basicVertexShader;

            DoremiEngine::Graphic::DepthStencilState* m_depthStencilStatNone;
            DirectX::XMFLOAT2 m_resolution;
        };
    }
}