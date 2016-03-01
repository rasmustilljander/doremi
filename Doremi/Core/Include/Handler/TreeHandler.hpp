#pragma once
#include <Doremi/Core/Include/TreeCreator.hpp>
#include <Doremi/Core/Include/EventHandler/Subscriber.hpp>
#include <Doremi/Core/Include/EntityComponent/Constants.hpp>

#include <DirectXMath.h>
#include <vector>
#include <cstdint>

namespace DoremiEngine
{
    namespace Core
    {
        class SharedContext;
    }

        namespace Graphic
        {
            class DepthStencilState;
            class RasterizerState;
            class PixelShader;
            class VertexShader;
        }
    
}
namespace Doremi
{
    namespace Core
    {
        class TreeHandler : public Subscriber
        {
        public:
            static TreeHandler* GetInstance();

            static void StartupTreeHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);

            void BuildTheTree();

            std::vector<uint32_t> Update();

            void OnEvent(Event* p_event) override;
            void ResetObjectsToDraw();
        private:
            TreeHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);
            ~TreeHandler();
            static TreeHandler* m_singleton;
            std::vector<uint32_t> m_objectsToDraw;
            const DoremiEngine::Core::SharedContext& m_sharedContext;
            TreeCreator* m_treeCreator;
            uint32_t m_viewDist;
            DirectX::XMFLOAT4 m_planes[6];
            bool CollisionCheckForBox(DirectX::XMFLOAT3 p_center, DirectX::XMFLOAT3 p_dimensions);
            // SKITKOD TODOEA

            DirectX::XMFLOAT3 m_playerPos; // DEBUG
            uint32_t drawedLastUpdate = 0;
            DoremiEngine::Graphic::PixelShader* m_pixelShader;
            DoremiEngine::Graphic::VertexShader* m_vertexShader;
            DoremiEngine::Graphic::DepthStencilState* m_depthStencilState;
            DoremiEngine::Graphic::RasterizerState* m_rasterizerState;
            ////
        };
    }
}
