#pragma once
// Project specific
#include <Doremi/Core/Include/Manager/Manager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Mesh/MeshInfo.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Mesh/MaterialInfo.hpp>
#include <DoremiEngine/Graphic/Include/Interface/State/DepthStencilState.hpp>
#include <DoremiEngine/Graphic/Include/Interface/State/RasterizerState.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Shader/PixelShader.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Shader/VertexShader.hpp>

#include <DirectXMath.h>
namespace Doremi
{
    namespace Core
    {
        class SkyBoxHandler
        {
        public:
            static void StartupSkyBoxHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);
            static SkyBoxHandler* GetInstance();
            void Draw();
            void CreateSkyBox();
            // If the manager is a Subscriber, inherits the subscriber class, it will have to override OnEvent funciton
        private:
            SkyBoxHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);
            ~SkyBoxHandler();

            static SkyBoxHandler* m_sigleton;

            const DoremiEngine::Core::SharedContext& m_sharedContext;

            DoremiEngine::Graphic::MaterialInfo* m_materialInfo;
            DoremiEngine::Graphic::MeshInfo* m_meshInfo;
            DoremiEngine::Graphic::PixelShader* m_skyBoxPixelShader;
            DoremiEngine::Graphic::VertexShader* m_skyBoxVertexShader;
            DoremiEngine::Graphic::DepthStencilState* m_depthStencilState;
            DoremiEngine::Graphic::RasterizerState* m_rasterizerState;
        };
    }
}
