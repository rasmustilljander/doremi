// Project specific
#include <Manager/SkeletalAnimationCoreManager.hpp>
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <EntityComponent/Components/TransformComponent.hpp>
#include <EntityComponent/Components/SkeletalAnimationComponent.hpp>
#include <EntityComponent/Components/RenderComponent.hpp>
#include <Doremi/Core/Include/PlayerHandler.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/ShaderManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/DirectXManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/MeshManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/SubModuleManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/SkeletalAnimationManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Animation/SkeletalInformation.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Shader/PixelShader.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Shader/VertexShader.hpp>
#include <DoremiEngine/Graphic/Include/Interface/State/DepthStencilState.hpp>
#include <DoremiEngine/Graphic/Include/Interface/State/RasterizerState.hpp>
#include <dxgi.h>
#include <d3d11_1.h>

#include <DirectXMath.h>
// Third party

// Standard
#include <iostream>
#include <algorithm>
using namespace std;

namespace Doremi
{
    namespace Core
    {
        SkeletalAnimationCoreManager::SkeletalAnimationCoreManager(const DoremiEngine::Core::SharedContext& p_sharedContext)
            : Manager(p_sharedContext, "SkeletalAnimationManager")
        {

            D3D11_INPUT_ELEMENT_DESC ied[] = {
                {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
                {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
                {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
                {"MATRIXINDEX", 0, DXGI_FORMAT_R32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
            };
            m_vertexShader =
                m_sharedContext.GetGraphicModule().GetSubModuleManager().GetShaderManager().BuildVertexShader("SkeletalAnimationVertexShader.hlsl",
                                                                                                              ied, ARRAYSIZE(ied));
            m_pixelShader = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetShaderManager().BuildPixelShader("BasicPixelShader.hlsl");

            D3D11_RASTERIZER_DESC rastDesc;
            ZeroMemory(&rastDesc, sizeof(rastDesc));
            rastDesc.FillMode = D3D11_FILL_SOLID;
            rastDesc.CullMode = D3D11_CULL_NONE;
            rastDesc.FrontCounterClockwise = false;
            rastDesc.DepthBias = 0;
            rastDesc.DepthBiasClamp = 0.0f;
            rastDesc.SlopeScaledDepthBias = 0.0f;
            rastDesc.DepthClipEnable = false;
            rastDesc.ScissorEnable = false;
            rastDesc.MultisampleEnable = true;
            rastDesc.AntialiasedLineEnable = false;
            m_rasterizerState = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager().CreateRasterizerState(rastDesc);
            D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
            ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
            depthStencilDesc.DepthEnable = true;
            depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
            depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
            m_depthStencilState = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager().CreateDepthStencilState(depthStencilDesc);
        }

        SkeletalAnimationCoreManager::~SkeletalAnimationCoreManager() {}


        void SkeletalAnimationCoreManager::Update(double p_dt)
        {
            // Loop through all entities
            const size_t length = EntityHandler::GetInstance().GetLastEntityIndex();
            // Loop over all entities to perform various functions on enteties that have skeletal animation
            int mask = (int)ComponentType::Render | (int)ComponentType::Transform | (int)ComponentType::SkeletalAnimation;

            // Set shaders
            DoremiEngine::Graphic::SubModuleManager& submoduleManager = m_sharedContext.GetGraphicModule().GetSubModuleManager();
            submoduleManager.GetShaderManager().SetActiveVertexShader(m_vertexShader);
            submoduleManager.GetShaderManager().SetActivePixelShader(m_pixelShader);
            for(size_t j = 0; j < length; j++)
            {
                // Check if entity has skeletalanimation
                if(EntityHandler::GetInstance().HasComponents(j, mask))
                {
                    // Get component and update time that the animation has been active
                    SkeletalAnimationComponent* t_skeletalAnimationComponent =
                        EntityHandler::GetInstance().GetComponentFromStorage<SkeletalAnimationComponent>(j);
                    t_skeletalAnimationComponent->timePosition += p_dt;
                    // t_skeletalAnimationComponent->timePosition = 0.0f;
                    // Check if animationtimeelapsed is more than the cliplength. If so reset cliptime
                    if(t_skeletalAnimationComponent->timePosition >
                       t_skeletalAnimationComponent->skeletalInformation->GetClipEndTime(t_skeletalAnimationComponent->clipName))
                    {
                        t_skeletalAnimationComponent->timePosition = 0.0f;
                    }

                    int t_numberOfTransformationMatrices = t_skeletalAnimationComponent->skeletalInformation->GetBoneCount();
                    std::vector<DirectX::XMFLOAT4X4> t_finalTransformations(t_numberOfTransformationMatrices);
                    m_sharedContext.GetGraphicModule().GetSubModuleManager().GetSkeletalAnimationManager().GetFinalTransforms(
                        t_skeletalAnimationComponent->clipName, (float)t_skeletalAnimationComponent->timePosition, t_finalTransformations,
                        t_skeletalAnimationComponent->skeletalInformation);

                    // TODOLH ANIM use transformations!
                    m_sharedContext.GetGraphicModule().GetSubModuleManager().GetSkeletalAnimationManager().PushMatricesToDevice(t_finalTransformations);

                    // Start draw pipeline Copypasted from graphic manager


                    RenderComponent* renderComp = EntityHandler::GetInstance().GetComponentFromStorage<RenderComponent>(j);
                    TransformComponent* orientationComp = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(j);
                    DirectX::XMFLOAT4X4 transMat;
                    DirectX::XMVECTOR quaternion = DirectX::XMLoadFloat4(&orientationComp->rotation);
                    DirectX::XMMATRIX tempTransMat = DirectX::XMMatrixTranspose(
                        DirectX::XMMatrixScaling(orientationComp->scale.x, orientationComp->scale.y, orientationComp->scale.z) *
                        DirectX::XMMatrixRotationQuaternion(quaternion) *
                        DirectX::XMMatrixTranslation(orientationComp->position.x, orientationComp->position.y, orientationComp->position.z));
                    DirectX::XMStoreFloat4x4(&transMat, tempTransMat);
                    submoduleManager.GetMeshManager().AddToRenderList(*renderComp->mesh, *renderComp->material,
                                                                      transMat); // BYta till skeletalmeshinfo

                    m_rasterizerState->GetRasterizerState();
                    m_depthStencilState->GetDepthStencilState();
                    submoduleManager.GetDirectXManager().DrawCurrentRenderListSkeletal(m_rasterizerState->GetRasterizerState(),
                                                                                       m_depthStencilState->GetDepthStencilState());
                }
                else
                {
                    // do nothing
                }
            }
        }
    }
}
