// Project specific
#include <Manager/SkeletalAnimationCoreManager.hpp>
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <EntityComponent/Components/TransformComponent.hpp>
#include <EntityComponent/Components/SkeletalAnimationComponent.hpp>
#include <EntityComponent/Components/LowerSkeletalAnimationComponent.hpp>
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
#include <Doremi/Core/Include/EntityComponent/Components/PressureParticleComponent.hpp>

// EventStuff
#include <Doremi/Core/Include/EventHandler/EventHandler.hpp>
#include <Doremi/Core/Include/EventHandler/Events/AnimationTransitionEvent.hpp>
#include <Doremi/Core/Include/EventHandler/Events/DamageTakenEvent.hpp>
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

            EventHandler* t_eventHandler = EventHandler::GetInstance();
            t_eventHandler->Subscribe(EventType::AnimationTransition, this);
            t_eventHandler->Subscribe(EventType::DamageTaken, this);
        }

        SkeletalAnimationCoreManager::~SkeletalAnimationCoreManager() {}

        bool SkeletalAnimationCoreManager::CheckANDPerformAnimationTransition(const size_t& p_entityID)
        {
            using namespace DirectX;
            bool r_returnValue = false;
            LowerSkeletalAnimationComponent* t_lowerSkeletalAnimationComponent =
                EntityHandler::GetInstance().GetComponentFromStorage<LowerSkeletalAnimationComponent>(p_entityID);
            SkeletalAnimationComponent* t_upperSkeletalAnimationComponent =
                EntityHandler::GetInstance().GetComponentFromStorage<SkeletalAnimationComponent>(p_entityID);
            TransformComponentNext* t_transformComponentNext = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponentNext>(p_entityID);
            TransformComponentPrevious* t_transformComponentPrevious =
                EntityHandler::GetInstance().GetComponentFromStorage<TransformComponentPrevious>(p_entityID);

            float t_epsilon = 0.001f;
            XMFLOAT3 t_movementVector;
            XMStoreFloat3(&t_movementVector, XMLoadFloat3(&t_transformComponentNext->position) - XMLoadFloat3(&t_transformComponentPrevious->position));
            XMFLOAT3 t_movementLengthVector;
            XMStoreFloat3(&t_movementLengthVector, XMVector3Length(XMLoadFloat3(&t_movementVector)));

            // Lower animation flow

            // Om vi inte redan springer så ska vi in å kolla ifall vi har rört på oss
            if(t_lowerSkeletalAnimationComponent->clipName != "Run")
            {
                // Kolla om vi rört på oss
                if(t_movementLengthVector.x > t_epsilon)
                {
                    t_lowerSkeletalAnimationComponent->clipName = "Run";
                    t_lowerSkeletalAnimationComponent->timePosition = 0.0f;
                }
            }
            // Om vi springer ska vi kolla om vi har stannat
            else if(t_lowerSkeletalAnimationComponent->clipName == "Run")
            {
                // Kolla om vi stannat
                if(t_movementLengthVector.x < t_epsilon)
                {
                    // Gör samma som överkroppen
                    t_lowerSkeletalAnimationComponent->clipName = t_upperSkeletalAnimationComponent->clipName;
                    t_lowerSkeletalAnimationComponent->timePosition = t_upperSkeletalAnimationComponent->timePosition;
                }
            }
            // Överkroppens animations flow
            // Om du har rört dig men din animation är idle. Så ska du sättas till run
            if(t_movementLengthVector.x > t_epsilon && t_upperSkeletalAnimationComponent->clipName == "Idle")
            {
                t_upperSkeletalAnimationComponent->clipName = "Run";
                if(t_lowerSkeletalAnimationComponent->clipName == "Run")
                {
                    t_upperSkeletalAnimationComponent->timePosition = t_lowerSkeletalAnimationComponent->timePosition;
                }
                else
                {
                    t_upperSkeletalAnimationComponent->timePosition = 0.0f;
                }
            }
            // Om överkroppen är i runmode men du har stannat ska du köra idle istället
            if(t_upperSkeletalAnimationComponent->clipName == "Run" && t_movementLengthVector.x < t_epsilon)
            {
                t_upperSkeletalAnimationComponent->clipName = "Idle";
                t_upperSkeletalAnimationComponent->timePosition = 0.0f;
            }

            // Orientera underkroppens logik
            if(t_lowerSkeletalAnimationComponent->clipName == "Run")
            {
                XMVECTOR t_orientationQuater = XMLoadFloat4(&t_transformComponentNext->rotation);
                XMFLOAT3 t_frontVector;
                XMStoreFloat3(&t_frontVector, XMVector3Transform(XMLoadFloat3(&XMFLOAT3(0, 0, 1)), XMMatrixRotationQuaternion(t_orientationQuater)));

                XMFLOAT2 t_frontVec2 = XMFLOAT2(t_frontVector.x, t_frontVector.z);
                XMFLOAT2 t_moveVec2 = XMFLOAT2(t_movementVector.x, t_movementVector.z);
                XMFLOAT2 t_positive;
                XMStoreFloat2(&t_positive, XMVector2Cross(XMLoadFloat2(&t_frontVec2), XMLoadFloat2(&t_moveVec2)));
                XMFLOAT3 angleVec;
                XMStoreFloat3(&angleVec, XMVector2AngleBetweenVectors(XMLoadFloat2(&t_frontVec2), XMLoadFloat2(&t_moveVec2)));

                if(t_positive.y < 0)
                {
                    t_positive.y = 1;
                }
                else if(t_positive.y > 0)
                {
                    t_positive.y = -1;
                }
                else
                {
                    t_positive.y = 0;
                }
                angleVec.x *= t_positive.y;
                XMStoreFloat4(&t_lowerSkeletalAnimationComponent->orientation, XMQuaternionRotationAxis(XMLoadFloat3(&XMFLOAT3(0, 1, 0)), angleVec.x));
            }

            return r_returnValue;
        }

        void SkeletalAnimationCoreManager::OnEvent(Event* p_event)
        {
            switch(p_event->eventType)
            {
                case Doremi::Core::EventType::AnimationTransition:
                {
                    AnimationTransitionEvent* t_animationTransitionEvent = static_cast<AnimationTransitionEvent*>(p_event);
                    LowerSkeletalAnimationComponent* t_lowerSkeletalAnimationComponent =
                        EntityHandler::GetInstance().GetComponentFromStorage<LowerSkeletalAnimationComponent>(t_animationTransitionEvent->entityID);
                    SkeletalAnimationComponent* t_upperSkeletalAnimationComponent =
                        EntityHandler::GetInstance().GetComponentFromStorage<SkeletalAnimationComponent>(t_animationTransitionEvent->entityID);
                    if(t_animationTransitionEvent->animation == Animation::ATTACK)
                    {
                        t_upperSkeletalAnimationComponent->clipName = "Attack";
                        t_upperSkeletalAnimationComponent->timePosition = 0.0f;
                        if(t_lowerSkeletalAnimationComponent->clipName != "Run")
                        {
                            t_lowerSkeletalAnimationComponent->clipName = "Attack";
                            t_lowerSkeletalAnimationComponent->timePosition = 0.0f;
                        }
                    }
                    else if(t_animationTransitionEvent->animation == Animation::STOPATTACK)
                    {
                        t_upperSkeletalAnimationComponent->clipName = "Idle";
                        t_upperSkeletalAnimationComponent->timePosition = 0.0f;
                        if(t_lowerSkeletalAnimationComponent->clipName == "Attack")
                        {
                            t_lowerSkeletalAnimationComponent->clipName = "Idle";
                            t_lowerSkeletalAnimationComponent->timePosition = 0.0f;
                        }
                    }
                }
                case EventType::DamageTaken:
                {
                    // A bit stupid... we send a event to our own class to play a sound when we take damage...
                    DamageTakenEvent* t_event = static_cast<DamageTakenEvent*>(p_event);
                    LowerSkeletalAnimationComponent* t_lowerSkeletalAnimationComponent =
                        EntityHandler::GetInstance().GetComponentFromStorage<LowerSkeletalAnimationComponent>(t_event->entityId);
                    SkeletalAnimationComponent* t_upperSkeletalAnimationComponent =
                        EntityHandler::GetInstance().GetComponentFromStorage<SkeletalAnimationComponent>(t_event->entityId);
                    t_upperSkeletalAnimationComponent->clipName = "Hit";
                    t_upperSkeletalAnimationComponent->timePosition = 0.0f;
                    if(t_lowerSkeletalAnimationComponent->clipName != "Run")
                    {
                        t_lowerSkeletalAnimationComponent->clipName = "Hit";
                    }
                    break;
                }
                break;
            }
        }

        void SkeletalAnimationCoreManager::Update(double p_dt)
        {
            // Loop through all entities
            const size_t length = EntityHandler::GetInstance().GetLastEntityIndex();
            // Loop over all entities to perform various functions on enteties that have skeletal animation
            int mask = (int)ComponentType::Render | (int)ComponentType::Transform | (int)ComponentType::UpperBodySkeletalAnimation |
                       (int)ComponentType::LowerBodySkeletalAnimation;
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
                    LowerSkeletalAnimationComponent* t_lowerSkeletalAnimationComponent =
                        EntityHandler::GetInstance().GetComponentFromStorage<LowerSkeletalAnimationComponent>(j);

                    t_skeletalAnimationComponent->timePosition += p_dt;
                    t_lowerSkeletalAnimationComponent->timePosition += p_dt;
                    // Check if animationtimeelapsed is more than the cliplength. If so reset cliptime
                    float t_upperBodyClipTime = t_skeletalAnimationComponent->skeletalInformation->GetClipEndTime(t_skeletalAnimationComponent->clipName);
                    float t_lowerBodyClipTime =
                        t_lowerSkeletalAnimationComponent->skeletalInformation->GetClipEndTime(t_lowerSkeletalAnimationComponent->clipName);
                    if(t_skeletalAnimationComponent->timePosition > t_upperBodyClipTime)
                    {
                        if(!t_skeletalAnimationComponent->skeletalInformation->GetAnimationClip(t_skeletalAnimationComponent->clipName).loop)
                        {
                            t_skeletalAnimationComponent->clipName = "Idle";
                            t_skeletalAnimationComponent->timePosition = 0.0f;
                        }
                        else
                        {
                            t_skeletalAnimationComponent->timePosition -= t_upperBodyClipTime;
                        }
                    }
                    if(t_lowerSkeletalAnimationComponent->timePosition > t_lowerBodyClipTime)
                    {
                        if(!t_skeletalAnimationComponent->skeletalInformation->GetAnimationClip(t_skeletalAnimationComponent->clipName).loop)
                        {
                            t_skeletalAnimationComponent->clipName = "Idle";
                            t_skeletalAnimationComponent->timePosition = 0.0f;
                        }
                        else
                        {
                            t_lowerSkeletalAnimationComponent->timePosition -= t_lowerBodyClipTime;
                        }
                    }

                    // Check if this enity should change animation and start the transition
                    CheckANDPerformAnimationTransition(j);
                    // Make a transformationmatrix per bone
                    int t_numberOfTransformationMatrices = t_skeletalAnimationComponent->skeletalInformation->GetBoneCount();
                    int t_numberOfLowerBodyTransforms = t_lowerSkeletalAnimationComponent->skeletalInformation->GetBoneCount();
                    // Declare and define a vector to the length of the number of joints/bones
                    std::vector<DirectX::XMFLOAT4X4> t_finalTransformations(t_numberOfTransformationMatrices);
                    std::vector<DirectX::XMFLOAT4X4> t_lowerBodyFinalTransformations(t_numberOfLowerBodyTransforms);
                    // Send to graphicmodule to compute the final transformationmatrices for this timeinstant.
                    m_sharedContext.GetGraphicModule().GetSubModuleManager().GetSkeletalAnimationManager().GetFinalTransforms(
                        t_skeletalAnimationComponent->clipName, (float)t_skeletalAnimationComponent->timePosition, t_finalTransformations,
                        t_skeletalAnimationComponent->skeletalInformation);
                    // Do the same for lowerbody
                    m_sharedContext.GetGraphicModule().GetSubModuleManager().GetSkeletalAnimationManager().GetFinalTransforms(
                        t_lowerSkeletalAnimationComponent->clipName, (float)t_lowerSkeletalAnimationComponent->timePosition,
                        t_lowerBodyFinalTransformations, t_lowerSkeletalAnimationComponent->skeletalInformation);
                    // Add the lowerbodytransformations to the list of upperbody and get ready to push the matrices to gpu
                    XMMATRIX t_rotationMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&t_lowerSkeletalAnimationComponent->orientation));
                    for(size_t i = 1; i < t_numberOfLowerBodyTransforms; i++)
                    {
                        // Rotate with the extra quaternion we calculated earlier. So that we can run backwards and sideways in that funny way u know
                        XMStoreFloat4x4(&t_lowerBodyFinalTransformations[i], XMLoadFloat4x4(&t_lowerBodyFinalTransformations[i]) * t_rotationMatrix);
                        t_finalTransformations.push_back(t_lowerBodyFinalTransformations[i]);
                    }
                    // Push the matrices to the gpu
                    m_sharedContext.GetGraphicModule().GetSubModuleManager().GetSkeletalAnimationManager().PushMatricesToDevice(t_finalTransformations);

                    // Start draw pipeline Copypasted from graphic manager
                    RenderComponent* renderComp = EntityHandler::GetInstance().GetComponentFromStorage<RenderComponent>(j);
                    TransformComponent* orientationComp = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(j);
                    DirectX::XMFLOAT4X4 transMat;
                    DirectX::XMVECTOR quaternion = DirectX::XMLoadFloat4(&orientationComp->rotation);

                    DirectX::XMMATRIX tempTransMat = DirectX::XMMatrixTranspose(
                        DirectX::XMMatrixScaling(orientationComp->scale.x, orientationComp->scale.y, orientationComp->scale.z) *
                        DirectX::XMMatrixRotationQuaternion(quaternion) *
                        DirectX::XMMatrixTranslation(orientationComp->position.x, orientationComp->position.y + renderComp->offsetY, orientationComp->position.z));
                    DirectX::XMStoreFloat4x4(&transMat, tempTransMat);
                    submoduleManager.GetMeshManager().AddToRenderList(*renderComp->mesh, *renderComp->material, transMat);
                    // Set the Raster and depthstencilstate
                    m_rasterizerState->GetRasterizerState();
                    m_depthStencilState->GetDepthStencilState();
                    // Draw the skeletalmesh uses another drawmethod than the common one. Since now we have more information in the vertex
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
