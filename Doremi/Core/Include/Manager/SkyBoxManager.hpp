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
        /**
        Only an example of what a manager might look like
        Doesn't do anything, and could be removed once examples are no longer necessary*/
        class SkyBoxManager : public Manager
        {
            public:
            SkyBoxManager(const DoremiEngine::Core::SharedContext& p_sharedContext);
            virtual ~SkyBoxManager();
            void Update(double p_dt) override;
            void CreateSkyBox();
            // If the manager is a Subscriber, inherits the subscriber class, it will have to override OnEvent funciton
            private:
            DoremiEngine::Graphic::MaterialInfo* m_materialInfo;
            DoremiEngine::Graphic::MeshInfo* m_meshInfo;
            DoremiEngine::Graphic::PixelShader* m_skyBoxPixelShader;
            DoremiEngine::Graphic::VertexShader* m_skyBoxVertexShader;
            DoremiEngine::Graphic::DepthStencilState* m_depthStencilState;
            DoremiEngine::Graphic::RasterizerState* m_rasterizerState;


            DirectX::XMMATRIX m_sphereWorldMatrix;
        };
    }
}
