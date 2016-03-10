// Project specific
#include <Manager/SkeletalAnimationCoreManager.hpp>
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>
#include <Doremi/Core/Include/PlayerHandlerClient.hpp>

// Components
#include <EntityComponent/EntityHandler.hpp>
#include <EntityComponent/Components/TransformComponent.hpp>
#include <EntityComponent/Components/SkeletalAnimationComponent.hpp>
#include <EntityComponent/Components/LowerSkeletalAnimationComponent.hpp>
#include <EntityComponent/Components/RenderComponent.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/PressureParticleComponent.hpp>

// Graphic
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

// Physics
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/RayCastManager.hpp>

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

namespace Doremi
{
    namespace Core
    {
        SkeletalAnimationCoreManager::SkeletalAnimationCoreManager(const DoremiEngine::Core::SharedContext& p_sharedContext)
            : Manager(p_sharedContext, "SkeletalAnimationCoreManager")
        {
            // Skapa Vertex layout
            D3D11_INPUT_ELEMENT_DESC ied[] = {
                {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
                {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
                {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
                {"MATRIXINDEX", 0, DXGI_FORMAT_R32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
            };
            // Skapa shaders
            m_vertexShader =
                m_sharedContext.GetGraphicModule().GetSubModuleManager().GetShaderManager().BuildVertexShader("SkeletalAnimationVertexShader.hlsl",
                                                                                                              ied, ARRAYSIZE(ied));
            m_pixelShader = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetShaderManager().BuildPixelShader("BasicPixelShader.hlsl");
            // SKapa rasterizer
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
            // Skapa depthstensicl
            D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
            ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
            depthStencilDesc.DepthEnable = true;
            depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
            depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
            m_depthStencilState = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager().CreateDepthStencilState(depthStencilDesc);
            // Subscribe to events
            EventHandler* t_eventHandler = EventHandler::GetInstance();
            t_eventHandler->Subscribe(EventType::AnimationTransition, this);
            t_eventHandler->Subscribe(EventType::DamageTaken, this);

            // Set the time for animationTransitions
            m_animationTransitionTime = 0.2f;
        }

        SkeletalAnimationCoreManager::~SkeletalAnimationCoreManager() {}

        void SkeletalAnimationCoreManager::UpdateTransitionsPlayer(SkeletalAnimationComponent& p_skeletalAnimationComponent,
                                                                   LowerSkeletalAnimationComponent& p_lowerSkeletalAnimationComponent, double p_dt)
        {
            std::vector<std::string> t_animationNames = p_skeletalAnimationComponent.skeletalInformation->GetAnimationNames();
            size_t numberOfAnimations = t_animationNames.size();
            // Great fulfix. Annars kan run skriva över attack. För att eventet attack kommer efter update. Det gör så att jag inte kan koda för att
            // run och attack kommer på samma frame.
            // Iochmed att attack anländer i slutet av framen. Om det gör det så kommer det bli run i slutändan. eftersom båda kommer in här och
            // attack är först i listan.
            for(size_t i = 0; i < numberOfAnimations; i++)
            {
                auto& clip = (*p_skeletalAnimationComponent.animationTransitions).find(t_animationNames[i]);
                if(clip == (*p_skeletalAnimationComponent.animationTransitions).end())
                {
                    printf("Iteratorn fakkar i updateTransition \n");
                }
                if(clip->second.startTime != 0.0f)
                {
                    clip->second.elapsedSinceStart += (float)p_dt;
                    // cout << "Uppdaterade: " << t_animationNames[i] << " Tid: " << clip->second.elapsedSinceStart - clip->second.startTime << endl;
                }
                if(clip->second.elapsedSinceStart > clip->second.startTime + m_animationTransitionTime)
                {
                    p_skeletalAnimationComponent.clipName = t_animationNames[i];
                    p_skeletalAnimationComponent.timePosition = clip->second.elapsedSinceStart;
                    clip->second.elapsedSinceStart = 0.0f;
                    clip->second.startTime = 0.0f;
                    if(t_animationNames[i] == "Idle")
                    {
                        // cout << "Du transitionade till Idle" << endl;
                    }
                    if(t_animationNames[i] == "Attack")
                    {
                        for(size_t j = 0; j < numberOfAnimations; j++)
                        {
                            auto& lclip = (*p_skeletalAnimationComponent.animationTransitions).find(t_animationNames[j]);
                            lclip->second.elapsedSinceStart = 0.0f;
                            lclip->second.startTime = 0.0f;
                        }
                        // cout << "Du mördade alla animationer som transitionade" << endl;
                    }
                }
                else
                {
                    // Do nothing
                }
                auto& lowerClip = (*p_lowerSkeletalAnimationComponent.animationTransitions).find(t_animationNames[i]);
                if(lowerClip->second.startTime != 0.0f)
                {
                    lowerClip->second.elapsedSinceStart += (float)p_dt;
                }
                if(lowerClip->second.elapsedSinceStart > lowerClip->second.startTime + m_animationTransitionTime)
                {
                    p_lowerSkeletalAnimationComponent.clipName = t_animationNames[i];
                    p_lowerSkeletalAnimationComponent.timePosition = lowerClip->second.elapsedSinceStart;
                    lowerClip->second.elapsedSinceStart = 0.0f;
                    lowerClip->second.startTime = 0.0f;
                }
                else
                {
                    // Do nothing
                }
            }
        }

        void SkeletalAnimationCoreManager::UpdateTransitionsEnemy(SkeletalAnimationComponent& p_skeletalAnimationComponent,
                                                                  LowerSkeletalAnimationComponent& p_lowerSkeletalAnimationComponent, double p_dt)
        {
            std::vector<std::string> t_animationNames = p_skeletalAnimationComponent.skeletalInformation->GetAnimationNames();
            size_t numberOfAnimations = t_animationNames.size();
            for(size_t i = 0; i < numberOfAnimations; i++)
            {
                // p_skeletalAnimationComponent.skeletalInformation->GetAnimationBlend(t_animationNames[i]).UpdateTimeElapsed(p_dt);
                auto& clip = (*p_skeletalAnimationComponent.animationTransitions).find(t_animationNames[i]);
                if(clip == (*p_skeletalAnimationComponent.animationTransitions).end())
                {
                    printf("Iteratorn fakkar i updateTransition \n");
                }
                if(clip->second.startTime != 0)
                {
                    clip->second.elapsedSinceStart += (float)p_dt;
                }
                if(clip->second.elapsedSinceStart > m_animationTransitionTime)
                {
                    p_skeletalAnimationComponent.clipName = t_animationNames[i];
                    p_skeletalAnimationComponent.timePosition = clip->second.elapsedSinceStart;
                    clip->second.startTime = 0.0f;
                    clip->second.elapsedSinceStart = 0.0f;
                    if(t_animationNames[i] == "Attack")
                    {
                        for(size_t j = 0; j < numberOfAnimations; j++)
                        {
                            auto& lclip = (*p_skeletalAnimationComponent.animationTransitions).find(t_animationNames[j]);
                            lclip->second.elapsedSinceStart = 0.0f;
                            lclip->second.startTime = 0.0f;
                        }
                        // cout << "Du mördade alla animationer som transitionade" << endl;
                    }
                }
                else
                {
                    // Do nothing
                }
                auto& lowerClip = (*p_lowerSkeletalAnimationComponent.animationTransitions).find(t_animationNames[i]);
                if(lowerClip->second.startTime != 0)
                {
                    lowerClip->second.elapsedSinceStart += (float)p_dt;
                }
                // p_lowerSkeletalAnimationComponent.skeletalInformation->GetAnimationBlend(t_animationNames[i]).UpdateTimeElapsed(p_dt);
                if(lowerClip->second.elapsedSinceStart > m_animationTransitionTime)
                {
                    p_lowerSkeletalAnimationComponent.clipName = t_animationNames[i];
                    p_lowerSkeletalAnimationComponent.timePosition = lowerClip->second.elapsedSinceStart;
                    lowerClip->second.startTime = 0.0f;
                    lowerClip->second.elapsedSinceStart = 0.0f;
                    /*p_lowerSkeletalAnimationComponent.timePosition =
                    p_lowerSkeletalAnimationComponent.skeletalInformation->GetAnimationBlend(t_animationNames[i]).timeElapsed;
                    p_lowerSkeletalAnimationComponent.skeletalInformation->GetAnimationBlend(t_animationNames[i]).ResetTimer();*/
                }
                else
                {
                    // Do nothing
                }
            }
        }

        void SkeletalAnimationCoreManager::STimer(const std::string& p_animationName, SkeletalAnimationComponent* o_skeletalAnimationComponent, float p_startTime)
        {
            auto& clip = (*o_skeletalAnimationComponent->animationTransitions).find(p_animationName);
            clip->second.startTime = p_startTime;
            clip->second.elapsedSinceStart = p_startTime;
            // cout << "Started timer: " << p_animationName << "StartTime : " << p_startTime << endl;
        }

        void SkeletalAnimationCoreManager::STimer(const std::string& p_animationName, LowerSkeletalAnimationComponent* o_skeletalAnimationComponent, float p_startTime)
        {
            auto& clip = (*o_skeletalAnimationComponent->animationTransitions).find(p_animationName);
            clip->second.startTime = p_startTime;
            clip->second.elapsedSinceStart = p_startTime;
        }

        std::string SkeletalAnimationCoreManager::CheckForTransitions(float& o_lowestTimeElapsed, LowerSkeletalAnimationComponent* p_lowerSkeletalAnimationComponent)
        {
            std::vector<std::string> t_animationNames = p_lowerSkeletalAnimationComponent->skeletalInformation->GetAnimationNames();
            size_t numberOfAnimations = t_animationNames.size();
            o_lowestTimeElapsed = 9999;
            int32_t t_index = -1;
            for(size_t i = 0; i < numberOfAnimations; i++)
            {
                auto& clip = (*p_lowerSkeletalAnimationComponent->animationTransitions).find(t_animationNames[i]);
                if(clip->second.startTime == 0.0f)
                {
                    continue;
                }
                else
                {
                    float t_clipElapsedTime = clip->second.elapsedSinceStart - clip->second.startTime;
                    if(o_lowestTimeElapsed > t_clipElapsedTime)
                    {
                        o_lowestTimeElapsed = t_clipElapsedTime;
                        t_index = i;
                    }
                }
            }
            if(t_index > -1)
            {
                return t_animationNames[t_index];
            }
            else
            {
                return "";
            }
        }
        std::string SkeletalAnimationCoreManager::CheckForTransitions(float& o_lowestTimeElapsed, SkeletalAnimationComponent* p_upperSkeletalAnimationComponent)
        {
            std::vector<std::string> t_animationNames = p_upperSkeletalAnimationComponent->skeletalInformation->GetAnimationNames();
            size_t numberOfAnimations = t_animationNames.size();
            o_lowestTimeElapsed = 9999;
            int32_t t_index = -1;
            for(size_t i = 0; i < numberOfAnimations; i++)
            {
                auto& clip = (*p_upperSkeletalAnimationComponent->animationTransitions).find(t_animationNames[i]);
                if(clip->second.startTime == 0.0f)
                {
                    continue;
                }
                else
                {
                    float t_clipElapsedTime = clip->second.elapsedSinceStart - clip->second.startTime;
                    if(o_lowestTimeElapsed > t_clipElapsedTime)
                    {
                        o_lowestTimeElapsed = t_clipElapsedTime;
                        t_index = i;
                    }
                }
            }
            if(t_index > -1)
            {
                return t_animationNames[t_index];
            }
            else
            {
                return "";
            }
        }

        /**
        Returns 0 if we are not on an elevator. Returns 1 if we are on elevator but are not moving. returns 2 if we are on an elevator and are moving.
        */
        int SkeletalAnimationCoreManager::CheckPlatformMovementAndRunPlayer(XMFLOAT3 p_origin, float p_epsilon, float p_movementLenghtVector, int p_bodyHit)
        {
            // Raycasta för att se om du är på en hiss.

            if(EntityHandler::GetInstance().HasComponents(p_bodyHit, (int)ComponentType::FrequencyAffected))
            {
                TransformComponentNext* t_platformTransformNext = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponentNext>(p_bodyHit);
                TransformComponentPrevious* t_platformTransformPrevious =
                    EntityHandler::GetInstance().GetComponentFromStorage<TransformComponentPrevious>(p_bodyHit);
                XMFLOAT3 t_platformMovementVector;
                // Skapa en vector mellan förra positionen och nya positionen
                XMStoreFloat3(&t_platformMovementVector, XMLoadFloat3(&t_platformTransformNext->position) - XMLoadFloat3(&t_platformTransformPrevious->position));
                XMFLOAT3 t_platformMovementLengthVector;
                // Calculate the length of this vector
                XMStoreFloat3(&t_platformMovementLengthVector, XMVector3Length(XMLoadFloat3(&t_platformMovementVector)));
                // See if the vector is very close to 0. That means we only move by following the platform and we should not "Run" animation
                if(t_platformMovementLengthVector.x - p_movementLenghtVector < p_epsilon && t_platformMovementLengthVector.x - p_movementLenghtVector > -p_epsilon)
                {
                    // cout << "False Du är på en platform. Din movement vectorLength: " << p_movementLenghtVector << "PlatformensMovementvectorLegth
                    // " << t_platformMovementLengthVector.x << endl;
                    return 1;
                }
                else
                {
                    // cout << "True Du är på en platform. Din movement vectorLength: " << p_movementLenghtVector << "PlatformensMovementvectorLegth "
                    // << t_platformMovementLengthVector.x << endl;
                    return 2;
                }
            }
            return 0;
        }
        void SkeletalAnimationCoreManager::CheckANDPerformAnimationTransitionPlayer(const size_t& p_entityID)
        {
            using namespace DirectX;
            // Hämta komponenter för animation och för rörelse
            LowerSkeletalAnimationComponent* t_lowerSkeletalAnimationComponent =
                EntityHandler::GetInstance().GetComponentFromStorage<LowerSkeletalAnimationComponent>(p_entityID);
            SkeletalAnimationComponent* t_upperSkeletalAnimationComponent =
                EntityHandler::GetInstance().GetComponentFromStorage<SkeletalAnimationComponent>(p_entityID);
            TransformComponentNext* t_transformComponentNext = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponentNext>(p_entityID);
            TransformComponentPrevious* t_transformComponentPrevious =
                EntityHandler::GetInstance().GetComponentFromStorage<TransformComponentPrevious>(p_entityID);
            TransformComponent* t_currentTransform = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(p_entityID);


            // epsilon för ett litet värde

            float t_epsilon = 0.05f;
            XMFLOAT3 t_movementVector;
            // Skapa en vector mellan förra positionen och nya positionen
            XMStoreFloat3(&t_movementVector, XMLoadFloat3(&t_transformComponentNext->position) - XMLoadFloat3(&t_transformComponentPrevious->position));
            XMFLOAT3 t_movementLengthVector;
            // Calculate the length of this vector
            XMStoreFloat3(&t_movementLengthVector, XMVector3Length(XMLoadFloat3(&t_movementVector)));
            float t_movementInYAxis = abs(t_movementVector.y);
            float t_movementInXAxis = abs(t_movementVector.x);
            float t_movementInZAxis = abs(t_movementVector.z);
            // Lower animation flow
            // Om vi inte redan springer så ska vi in å kolla ifall vi har rört på oss

            // Raycasta för att se om iv är på en platform
            int t_platformCheck = -1;
            int bodyHit = -1;
            if(t_movementLengthVector.x > t_epsilon)
            {
                XMFLOAT3 t_originOffsetted = t_currentTransform->position;
                t_originOffsetted.y -= 4;
                bodyHit = m_sharedContext.GetPhysicsModule().GetRayCastManager().CastRay(t_originOffsetted, XMFLOAT3(0, -1, 0), 100);
                if(bodyHit == -1)
                {
                    // cout << "RayCast returned -1 in animation" << endl;
                }
                else
                {
                    // cout << "Bodyhit : " << bodyHit << endl;
                }

                if(bodyHit > -1)
                {
                    t_platformCheck = CheckPlatformMovementAndRunPlayer(t_transformComponentNext->position, t_epsilon, t_movementLengthVector.x, bodyHit);
                }
            }

            if(t_lowerSkeletalAnimationComponent->clipName != "Run" || (*t_lowerSkeletalAnimationComponent->animationTransitions)["Idle"].startTime > 0.0f)
            {
                // Kolla om vi rört på oss och vi inte har börjat runtransition
                if(t_movementLengthVector.x > t_epsilon && (*t_lowerSkeletalAnimationComponent->animationTransitions)["Run"].startTime == 0.0f)
                {
                    // cout << "Du är i vi har rört på oss och inte börjat runtransition på lower" << endl;
                    // Kolla ifall vi har rört oss i Yled. Ifall inte bara spring och ifall jag inte står still poå en rörande platform
                    if(t_movementInYAxis < t_epsilon && t_lowerSkeletalAnimationComponent->clipName != "Jump" && t_platformCheck != 1 &&
                       (*t_lowerSkeletalAnimationComponent->animationTransitions)["Jump"].startTime == 0.0f)
                    {
                        if(t_upperSkeletalAnimationComponent->clipName == "Run")
                        {
                            // cout << "Satte lower till runtime uppertimePos" << endl;
                            STimer("Run", t_lowerSkeletalAnimationComponent, t_upperSkeletalAnimationComponent->timePosition);
                        }
                        else if((*t_upperSkeletalAnimationComponent->animationTransitions)["Run"].startTime > 0)
                        {
                            // cout << "Satte lower till runtime Upperelasped" << endl;
                            STimer("Run", t_lowerSkeletalAnimationComponent, (*t_upperSkeletalAnimationComponent->animationTransitions)["Run"].elapsedSinceStart);
                        }
                        else
                        {
                            // cout << "Satte lower till runtime 0.0001" << endl;
                            STimer("Run", t_lowerSkeletalAnimationComponent, 0.0001f);
                        }
                    }
                    // Har vi rört oss i Yled betyder det att vi faller eller hoppar. DÅ ska vi kolla om vi hoppat klart för efter hela hopanimationen
                    // så startas Idlemätarn för transition
                    else if((*t_lowerSkeletalAnimationComponent->animationTransitions)["Idle"].startTime > 0.0f &&
                            (t_movementInXAxis > 0 || t_movementInZAxis > 0) && (*t_lowerSkeletalAnimationComponent->animationTransitions)["Run"].startTime == 0 &&
                            t_platformCheck < 1 && (*t_lowerSkeletalAnimationComponent->animationTransitions)["Jump"].startTime == 0.0f)
                    {
                        if(t_upperSkeletalAnimationComponent->clipName == "Run")
                        {
                            // cout << "Satte lower till runtime uppertimePos" << endl;
                            STimer("Run", t_lowerSkeletalAnimationComponent, t_upperSkeletalAnimationComponent->timePosition);
                        }
                        else if((*t_upperSkeletalAnimationComponent->animationTransitions)["Run"].startTime > 0)
                        {
                            // cout << "Satte lower till runtime Upperelasped" << endl;
                            STimer("Run", t_lowerSkeletalAnimationComponent, (*t_upperSkeletalAnimationComponent->animationTransitions)["Run"].elapsedSinceStart);
                        }
                        else
                        {
                            // cout << "Satte lower till runtime 0.0001" << endl;
                            STimer("Run", t_lowerSkeletalAnimationComponent, 0.0001f);
                        }
                        // (*t_lowerSkeletalAnimationComponent->animationTransitions)["Idle"].startTime = 0.0f;
                    }
                }
            }
            // Om vi springer ska vi kolla om vi har stannat
            else if(t_lowerSkeletalAnimationComponent->clipName == "Run")
            {
                // Kolla om vi stannat
                // cout << "Du är i vi springer och ska kolla om vi stannar Lower" << endl;
                if((t_movementLengthVector.x < t_epsilon || t_platformCheck == 1) && t_upperSkeletalAnimationComponent->clipName != "Run" &&
                   (*t_lowerSkeletalAnimationComponent->animationTransitions)["Attack"].startTime == 0 &&
                   (*t_lowerSkeletalAnimationComponent->animationTransitions)["Idle"].startTime == 0 &&
                   (*t_lowerSkeletalAnimationComponent->animationTransitions)["Jump"].startTime == 0 &&
                   (*t_lowerSkeletalAnimationComponent->animationTransitions)["Hit"].startTime == 0)
                {
                    // Gör samma som överkroppen
                    std::vector<std::string> t_animationNames = t_upperSkeletalAnimationComponent->skeletalInformation->GetAnimationNames();
                    size_t numberOfAnimations = t_animationNames.size();
                    float t_lowestElapsedTime = 9999;
                    int32_t t_index = -1;
                    std::string t_newestClip;
                    t_newestClip = CheckForTransitions(t_lowestElapsedTime, t_upperSkeletalAnimationComponent);
                    if(t_newestClip != "")
                    {
                        if(t_newestClip == "RunAttack")
                        {
                            // cout << "Opps underkroppen fick RunAttack" << endl;
                        }
                        STimer(t_newestClip, t_lowerSkeletalAnimationComponent, t_lowestElapsedTime);
                    }
                    else
                    {
                        STimer(t_upperSkeletalAnimationComponent->clipName, t_lowerSkeletalAnimationComponent, t_upperSkeletalAnimationComponent->timePosition);
                    }
                }
            }
            // Överkroppens animationsflow
            // Om du har rört dig och din animation är idle. Så ska du sättas till run annars ska alla andra animationer få köra klart sitt

            /*if((t_movementInXAxis > t_epsilon || t_movementInZAxis > t_epsilon) && t_platformCheck != 1 &&
               (t_upperSkeletalAnimationComponent->clipName == "Idle" || (*t_upperSkeletalAnimationComponent->animationTransitions)["Idle"].startTime
               > 0.0f) &&
               (*t_upperSkeletalAnimationComponent->animationTransitions)["Run"].startTime == 0 &&
               (*t_upperSkeletalAnimationComponent->animationTransitions)["Jump"].startTime == 0 &&
               (*t_upperSkeletalAnimationComponent->animationTransitions)["Attack"].startTime == 0)*/
            if((t_movementInXAxis > t_epsilon || t_movementInZAxis > t_epsilon) && t_platformCheck != 1 && t_upperSkeletalAnimationComponent->clipName != "Run" &&
               (*t_upperSkeletalAnimationComponent->animationTransitions)["Run"].startTime == 0.0f && t_upperSkeletalAnimationComponent->clipName != "RunAttack" &&
               (*t_upperSkeletalAnimationComponent->animationTransitions)["RunAttack"].startTime == 0.0f && t_upperSkeletalAnimationComponent->clipName != "Jump" &&
               (*t_upperSkeletalAnimationComponent->animationTransitions)["Jump"].startTime == 0.0f && t_upperSkeletalAnimationComponent->clipName != "JumpAttack" &&
               (*t_upperSkeletalAnimationComponent->animationTransitions)["JumpAttack"].startTime == 0.0f)
            {
                // cout << "Du är i Starta run i upper" << endl;
                // Kolla om vi transitionar run
                std::string t_newestTransition;
                float t_lowestElapsedTime;
                t_newestTransition = CheckForTransitions(t_lowestElapsedTime, t_lowerSkeletalAnimationComponent);
                // cout << t_lowerSkeletalAnimationComponent->clipName << endl;
                if(t_lowerSkeletalAnimationComponent->clipName == "Run")
                {
                    // AttackANdRun
                    if(t_upperSkeletalAnimationComponent->clipName == "Attack" || (*t_upperSkeletalAnimationComponent->animationTransitions)["Attack"].startTime > 0.0f)
                    {
                        STimer("RunAttack", t_upperSkeletalAnimationComponent, t_lowerSkeletalAnimationComponent->timePosition);
                    }
                    else
                    {
                        STimer("Run", t_upperSkeletalAnimationComponent, t_lowerSkeletalAnimationComponent->timePosition);
                    }
                }
                else if(t_newestTransition != "")
                {
                    // cout << "Nyaste Transitionen: " << t_newestTransition << endl;
                    if(t_newestTransition == "Run")
                    {
                        // RUN AND ATTACK
                        if(t_upperSkeletalAnimationComponent->clipName == "Attack" ||
                           (*t_upperSkeletalAnimationComponent->animationTransitions)["Attack"].startTime > 0.0f)
                        {
                            STimer("RunAttack", t_upperSkeletalAnimationComponent,
                                   (*t_lowerSkeletalAnimationComponent->animationTransitions)[t_newestTransition].elapsedSinceStart);
                        }
                        else
                        {
                            STimer("Run", t_upperSkeletalAnimationComponent,
                                   (*t_lowerSkeletalAnimationComponent->animationTransitions)[t_newestTransition].elapsedSinceStart);
                        }
                    }
                    else
                    {
                        STimer(t_newestTransition, t_upperSkeletalAnimationComponent,
                               (*t_lowerSkeletalAnimationComponent->animationTransitions)[t_newestTransition].elapsedSinceStart);
                    }
                }
                else
                {
                    // cout << "Här bördu inte vara (?)" << endl;
                    STimer("Run", t_upperSkeletalAnimationComponent, 0.0001f);
                }
            }
            // Om överkroppen är i runmode men du har stannat ska du köra idle istället
            if((t_upperSkeletalAnimationComponent->clipName == "Run" || (*t_upperSkeletalAnimationComponent->animationTransitions)["Run"].startTime > 0.0f ||
                (t_upperSkeletalAnimationComponent->clipName == "RunAttack" || (*t_upperSkeletalAnimationComponent->animationTransitions)["RunAttack"].startTime > 0.0f)) &&
               (t_movementLengthVector.x < t_epsilon || t_platformCheck == 1) &&
               (*t_upperSkeletalAnimationComponent->animationTransitions)["Idle"].startTime == 0.0f &&
               (*t_upperSkeletalAnimationComponent->animationTransitions)["Attack"].startTime == 0.0f)
            {
                // cout << "Överkroppen är i runmode men du har stannat" << endl;
                if(t_upperSkeletalAnimationComponent->clipName == "RunAttack")
                {
                    STimer("Attack", t_upperSkeletalAnimationComponent, t_upperSkeletalAnimationComponent->timePosition);
                }
                else if((*t_upperSkeletalAnimationComponent->animationTransitions)["RunAttack"].elapsedSinceStart > 0)
                {
                    std::string t_newestClip;
                    float t_lowestClipTime;
                    t_newestClip = CheckForTransitions(t_lowestClipTime, t_upperSkeletalAnimationComponent);
                    if(t_newestClip == "RunAttack")
                    {
                        STimer("Attack", t_upperSkeletalAnimationComponent, (*t_upperSkeletalAnimationComponent->animationTransitions)["RunAttack"].elapsedSinceStart);
                    }
                }
                else
                {
                    STimer("Idle", t_upperSkeletalAnimationComponent, 0.001f);
                }
            }

            // Orientera underkroppenlogik
            if((t_lowerSkeletalAnimationComponent->clipName == "Run" || t_lowerSkeletalAnimationComponent->clipName == "Jump") && t_platformCheck != 1)
            {
                // Hämta orientationen för meshen
                XMVECTOR t_orientationQuater = XMLoadFloat4(&t_transformComponentNext->rotation);
                XMFLOAT3 t_frontVector;
                // Rotera en enhetsvector enligt orientationen på meshen
                XMStoreFloat3(&t_frontVector, XMVector3Transform(XMLoadFloat3(&XMFLOAT3(0, 0, 1)), XMMatrixRotationQuaternion(t_orientationQuater)));

                // Skapa två vectorer i 2dplanet för att undvika att hopp ska influera
                XMFLOAT2 t_frontVec2 = XMFLOAT2(t_frontVector.x, t_frontVector.z);
                if(t_platformCheck == 2)
                {
                    TransformComponentNext* t_platformTransformNext = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponentNext>(bodyHit);
                    TransformComponentPrevious* t_platformTransformPrevious =
                        EntityHandler::GetInstance().GetComponentFromStorage<TransformComponentPrevious>(bodyHit);
                    XMFLOAT3 t_platformMovement;
                    XMStoreFloat3(&t_platformMovement, XMLoadFloat3(&t_platformTransformNext->position) - (XMLoadFloat3(&t_platformTransformPrevious->position)));
                    XMStoreFloat3(&t_movementVector, XMLoadFloat3(&t_movementVector) - XMLoadFloat3(&t_platformMovement));
                }
                XMFLOAT2 t_moveVec2 = XMFLOAT2(t_movementVector.x, t_movementVector.z);
                XMFLOAT2 t_positive;
                // Ta reda på åt vilket håll spelaren springer åt. För att kunna sätta vinkeln positiv eller negativ
                XMStoreFloat2(&t_positive, XMVector2Cross(XMLoadFloat2(&t_frontVec2), XMLoadFloat2(&t_moveVec2)));
                XMFLOAT3 angleVec;
                // Calculate angle between the vectors
                XMStoreFloat3(&angleVec, XMVector2AngleBetweenVectors(XMLoadFloat2(&t_frontVec2), XMLoadFloat2(&t_moveVec2)));
                // Create lengthvector to get rid of values very close to 0 messing up when stopping.
                XMFLOAT2 t_XZMovementLenght;
                XMStoreFloat2(&t_XZMovementLenght, XMVector2Length(XMLoadFloat2(&t_moveVec2)));
                // Sign operation
                if(t_positive.y < 0 && t_XZMovementLenght.x > t_epsilon)
                {
                    t_positive.y = 1;
                }
                else if(t_positive.y > 0 && t_XZMovementLenght.x > t_epsilon)
                {
                    t_positive.y = -1;
                }
                else
                {
                    t_positive.y = 0;
                }
                // Change the sign of the angle and then create an orientation quaternion describing a rotation around yaxis with the angle we
                // calculated
                angleVec.x *= t_positive.y;
                if(t_positive.y == 0)
                {
                    t_lowerSkeletalAnimationComponent->orientation = XMFLOAT4(0, 0, 0, 1);
                }
                else
                {
                    XMStoreFloat4(&t_lowerSkeletalAnimationComponent->orientation, XMQuaternionRotationAxis(XMLoadFloat3(&XMFLOAT3(0, 1, 0)), angleVec.x));
                }
            }
        }

        void SkeletalAnimationCoreManager::CheckANDPerformAnimationTransitionEnemy(const size_t& p_entityID)
        {
            using namespace DirectX;
            // Hämta komponenter för animation och för rörelse
            LowerSkeletalAnimationComponent* t_lowerSkeletalAnimationComponent =
                EntityHandler::GetInstance().GetComponentFromStorage<LowerSkeletalAnimationComponent>(p_entityID);
            SkeletalAnimationComponent* t_upperSkeletalAnimationComponent =
                EntityHandler::GetInstance().GetComponentFromStorage<SkeletalAnimationComponent>(p_entityID);
            TransformComponentNext* t_transformComponentNext = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponentNext>(p_entityID);
            TransformComponentPrevious* t_transformComponentPrevious =
                EntityHandler::GetInstance().GetComponentFromStorage<TransformComponentPrevious>(p_entityID);


            // epsilon för ett litet värde

            float t_epsilon = 0.1f;
            XMFLOAT3 t_movementVector;
            // Skapa en vector mellan förra positionen och nya positionen
            XMStoreFloat3(&t_movementVector, XMLoadFloat3(&t_transformComponentNext->position) - XMLoadFloat3(&t_transformComponentPrevious->position));
            XMFLOAT3 t_movementLengthVector;
            // Calculate the length of this vector
            XMStoreFloat3(&t_movementLengthVector, XMVector3Length(XMLoadFloat3(&t_movementVector)));
            float t_movementInYAxis = abs(t_movementVector.y);
            float t_movementInXAxis = abs(t_movementVector.x);
            float t_movementInZAxis = abs(t_movementVector.z);
            // Lower animation flow
            // Om vi inte redan springer så ska vi in å kolla ifall vi har rört på oss

            if(t_lowerSkeletalAnimationComponent->clipName != "Run")
            {
                // Kolla om vi rört på oss och vi inte har börjat runtransition
                if(t_movementLengthVector.x > t_epsilon && (*t_lowerSkeletalAnimationComponent->animationTransitions)["Run"].startTime == 0)
                {
                    // Kolla ifall vi har rört oss i Yled. Ifall inte bara spring
                    if(t_movementInYAxis < t_epsilon && t_lowerSkeletalAnimationComponent->clipName != "Jump")
                    {
                        STimer("Run", t_lowerSkeletalAnimationComponent, 0.001f);
                    }
                    // Har vi rört oss i Yled betyder det att vi faller eller hoppar. DÅ ska vi kolla om vi hoppat klart för efter hela hopanimationen
                    // så startas Idlemätarn för transition
                    else if((*t_lowerSkeletalAnimationComponent->animationTransitions)["Idle"].startTime > 0.0f && (t_movementInXAxis > 0 || t_movementInZAxis > 0))
                    {
                        STimer("Run", t_lowerSkeletalAnimationComponent, 0.001f);
                        (*t_lowerSkeletalAnimationComponent->animationTransitions)["Idle"].elapsedSinceStart = 0.0f;
                        (*t_lowerSkeletalAnimationComponent->animationTransitions)["Idle"].startTime = 0.0f;
                    }
                }
            }
            // Om vi springer ska vi kolla om vi har stannat
            else if(t_lowerSkeletalAnimationComponent->clipName == "Run")
            {
                // Kolla om vi stannat
                if(t_movementLengthVector.x < t_epsilon && t_upperSkeletalAnimationComponent->clipName != "Run" &&
                   (*t_lowerSkeletalAnimationComponent->animationTransitions)[t_upperSkeletalAnimationComponent->clipName].startTime == 0)
                {
                    // Gör samma som överkroppen
                    // printf("3");
                    // t_lowerSkeletalAnimationComponent->skeletalInformation->GetAnimationBlend(t_upperSkeletalAnimationComponent->clipName).StartTimer();
                    STimer(t_upperSkeletalAnimationComponent->clipName, t_lowerSkeletalAnimationComponent, 0.001f);
                    // t_lowerSkeletalAnimationComponent->clipName = t_upperSkeletalAnimationComponent->clipName;
                    // t_lowerSkeletalAnimationComponent->timePosition = t_upperSkeletalAnimationComponent->timePosition;
                    // TODOLH se till så att blendingen fungerar för dessa också! alltså när underkroppen ska ta överkroppens animation och synkas...
                }
            }
            // Överkroppens animationsflow
            // Om du har rört dig och din animation är idle. Så ska du sättas till run annars ska alla andra animationer få köra klart sitt

            if((t_movementInXAxis > t_epsilon || t_movementInZAxis > t_epsilon) &&
               (t_upperSkeletalAnimationComponent->clipName == "Idle" || (*t_upperSkeletalAnimationComponent->animationTransitions)["Idle"].startTime > 0.0f) &&
               (*t_upperSkeletalAnimationComponent->animationTransitions)["Run"].startTime == 0 &&
               (*t_upperSkeletalAnimationComponent->animationTransitions)["Jump"].startTime == 0.0f)
            {
                // t_upperSkeletalAnimationComponent->skeletalInformation->GetAnimationBlend("Run").StartTimer();
                STimer("Run", t_upperSkeletalAnimationComponent, 0.001f);
                (*t_upperSkeletalAnimationComponent->animationTransitions)["Idle"].startTime = 0.0f;
                // t_upperSkeletalAnimationComponent->clipName = "Run";
                if(t_lowerSkeletalAnimationComponent->clipName == "Run")
                {
                    // t_upperSkeletalAnimationComponent->timePosition = t_lowerSkeletalAnimationComponent->timePosition;
                }
                else
                {
                    // t_upperSkeletalAnimationComponent->timePosition = 0.0f;
                }
            }
            // Om överkroppen är i runmode men du har stannat ska du köra idle istället
            if(t_upperSkeletalAnimationComponent->clipName == "Run" && t_movementLengthVector.x < t_epsilon &&
               (*t_upperSkeletalAnimationComponent->animationTransitions)["Idle"].startTime == 0 &&
               (*t_upperSkeletalAnimationComponent->animationTransitions)["Attack"].startTime == 0)
            {
                STimer("Idle", t_upperSkeletalAnimationComponent, 0.001f);
            }

            // Orientera underkroppenlogik
            if(t_lowerSkeletalAnimationComponent->clipName == "Run" || t_lowerSkeletalAnimationComponent->clipName == "Jump")
            {
                // Hämta orientationen för meshen
                XMVECTOR t_orientationQuater = XMLoadFloat4(&t_transformComponentNext->rotation);
                XMFLOAT3 t_frontVector;
                // Rotera en enhetsvector enligt orientationen på meshen
                XMStoreFloat3(&t_frontVector, XMVector3Transform(XMLoadFloat3(&XMFLOAT3(0, 0, 1)), XMMatrixRotationQuaternion(t_orientationQuater)));

                // Skapa två vectorer i 2dplanet för att undvika att hopp ska influera
                XMFLOAT2 t_frontVec2 = XMFLOAT2(t_frontVector.x, t_frontVector.z);
                XMFLOAT2 t_moveVec2 = XMFLOAT2(t_movementVector.x, t_movementVector.z);
                XMFLOAT2 t_positive;
                // Ta reda på åt vilket håll spelaren springer åt. För att kunna sätta vinkeln positiv eller negativ
                XMStoreFloat2(&t_positive, XMVector2Cross(XMLoadFloat2(&t_frontVec2), XMLoadFloat2(&t_moveVec2)));
                XMFLOAT3 angleVec;
                // Calculate angle between the vectors
                XMStoreFloat3(&angleVec, XMVector2AngleBetweenVectors(XMLoadFloat2(&t_frontVec2), XMLoadFloat2(&t_moveVec2)));
                // Sign operation
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
                // Change the sign of the angle and then create an orientation quaternion describing a rotation around yaxis with the angle we
                // calculated
                angleVec.x *= t_positive.y;
                if(t_positive.y == 0)
                {
                    t_lowerSkeletalAnimationComponent->orientation = XMFLOAT4(0, 0, 0, 1);
                }
                else
                {
                    XMStoreFloat4(&t_lowerSkeletalAnimationComponent->orientation, XMQuaternionRotationAxis(XMLoadFloat3(&XMFLOAT3(0, 1, 0)), angleVec.x));
                }
            }
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

                    if((*t_upperSkeletalAnimationComponent->animationTransitions).count("JumpAttack") < 1)
                    {
                        // ENEmy onevent
                        if(t_animationTransitionEvent->animation == Animation::ATTACK)
                        {
                            // t_upperSkeletalAnimationComponent->skeletalInformation->GetAnimationBlend("Attack").StartTimer();
                            STimer("Attack", t_upperSkeletalAnimationComponent, 0.001f);
                            // t_upperSkeletalAnimationComponent->clipName = "Attack";
                            // t_upperSkeletalAnimationComponent->timePosition = 0.0f;
                            if(t_lowerSkeletalAnimationComponent->clipName != "Run" &&
                               (*t_lowerSkeletalAnimationComponent->animationTransitions)["Run"].startTime == 0 &&
                               t_lowerSkeletalAnimationComponent->clipName != "Jump" &&
                               (*t_lowerSkeletalAnimationComponent->animationTransitions)["Jump"].startTime > 0)
                            {
                                // t_lowerSkeletalAnimationComponent->skeletalInformation->GetAnimationBlend("Attack").StartTimer();
                                STimer("Attack", t_lowerSkeletalAnimationComponent, 0.001f);
                                // t_lowerSkeletalAnimationComponent->clipName = "Attack";
                                // t_lowerSkeletalAnimationComponent->timePosition = 0.0f;
                            }
                        }
                        else if(t_animationTransitionEvent->animation == Animation::STOPATTACK)
                        {
                            if(t_upperSkeletalAnimationComponent->clipName == "Attack" ||
                               (*t_upperSkeletalAnimationComponent->animationTransitions)["Attack"].startTime > 0.0f)
                            {
                                if(t_lowerSkeletalAnimationComponent->clipName != "Attack")
                                {
                                    STimer(t_lowerSkeletalAnimationComponent->clipName, t_upperSkeletalAnimationComponent, 0.001f);
                                }
                                else
                                {
                                    STimer("Idle", t_upperSkeletalAnimationComponent, 0.001f);
                                }
                            }
                            if(t_lowerSkeletalAnimationComponent->clipName == "Attack" ||
                               (*t_lowerSkeletalAnimationComponent->animationTransitions)["Attack"].startTime > 0.0f)
                            {
                                STimer("Idle", t_lowerSkeletalAnimationComponent, 0.001f);
                            }
                        }
                        else if(t_animationTransitionEvent->animation == Animation::JUMP)
                        {
                            if(t_upperSkeletalAnimationComponent->clipName != "Attack" &&
                               (*t_upperSkeletalAnimationComponent->animationTransitions)["Jump"].startTime == 0.0f &&
                               t_upperSkeletalAnimationComponent->clipName != "Jump")
                            {
                                STimer("Jump", t_upperSkeletalAnimationComponent, 0.001f);
                            }
                            if((*t_lowerSkeletalAnimationComponent->animationTransitions)["Jump"].startTime == 0.0f &&
                               t_upperSkeletalAnimationComponent->clipName != "Jump")
                            {
                                STimer("Jump", t_lowerSkeletalAnimationComponent, 0.001f);
                            }
                        }
                        break;
                    }
                    else
                    {
                        // Player onevent
                        if(t_animationTransitionEvent->animation == Animation::ATTACK)
                        {

                            float t_lowestTime = 9999;
                            std::string t_newestClip = CheckForTransitions(t_lowestTime, t_lowerSkeletalAnimationComponent);

                            if(t_newestClip == "Jump")
                            {
                                STimer("JumpAttack", t_upperSkeletalAnimationComponent,
                                       (*t_lowerSkeletalAnimationComponent->animationTransitions)["Jump"].elapsedSinceStart);
                            }
                            else if(t_newestClip == "Run")
                            {
                                STimer("RunAttack", t_upperSkeletalAnimationComponent,
                                       (*t_lowerSkeletalAnimationComponent->animationTransitions)["Run"].elapsedSinceStart);
                            }
                            else if(t_lowerSkeletalAnimationComponent->clipName == "Run")
                            {
                                STimer("RunAttack", t_upperSkeletalAnimationComponent, t_lowerSkeletalAnimationComponent->timePosition);
                            }
                            else if(t_lowerSkeletalAnimationComponent->clipName == "Jump")
                            {
                                STimer("JumpAttack", t_upperSkeletalAnimationComponent, t_lowerSkeletalAnimationComponent->timePosition);
                            }
                            else
                            {
                                STimer("Attack", t_upperSkeletalAnimationComponent, 0.001f);
                            }

                            /*if(t_lowerSkeletalAnimationComponent->clipName != "Run" &&
                               (*t_lowerSkeletalAnimationComponent->animationTransitions)["Run"].startTime == 0.0f &&
                               t_lowerSkeletalAnimationComponent->clipName != "Jump" &&
                               (*t_lowerSkeletalAnimationComponent->animationTransitions)["Jump"].startTime > 0.0f)*/
                            if(t_lowerSkeletalAnimationComponent->clipName == "Idle" &&
                               (*t_lowerSkeletalAnimationComponent->animationTransitions)["Idle"].startTime == 0.0f &&
                               (*t_lowerSkeletalAnimationComponent->animationTransitions)["Jump"].startTime == 0.0f &&
                               (*t_lowerSkeletalAnimationComponent->animationTransitions)["Run"].startTime == 0.0f)
                            {
                                STimer("Attack", t_lowerSkeletalAnimationComponent, 0.001f);
                            }
                        }
                        else if(t_animationTransitionEvent->animation == Animation::STOPATTACK)
                        {
                            // Gör lower först eftersom upper gärna tar av den

                            if(t_lowerSkeletalAnimationComponent->clipName == "Attack" ||
                               (*t_lowerSkeletalAnimationComponent->animationTransitions)["Attack"].startTime > 0.0f)
                            {
                                std::string t_newestClip = "";
                                float t_lowestTimeElapsed = 9999;
                                t_newestClip = CheckForTransitions(t_lowestTimeElapsed, t_lowerSkeletalAnimationComponent);
                                if(t_newestClip != "" && t_newestClip != "Attack")
                                {
                                    // DO nothing
                                }
                                else
                                {
                                    STimer("Idle", t_lowerSkeletalAnimationComponent, 0.001f);
                                }
                                (*t_lowerSkeletalAnimationComponent->animationTransitions)["Attack"].startTime = 0.0f;
                                (*t_lowerSkeletalAnimationComponent->animationTransitions)["Attack"].elapsedSinceStart = 0.0f;
                            }

                            // Om jag håller på att transitiona till attack eller är i attack
                            if(t_upperSkeletalAnimationComponent->clipName == "Attack" ||
                               (*t_upperSkeletalAnimationComponent->animationTransitions)["Attack"].startTime > 0.0f ||
                               t_upperSkeletalAnimationComponent->clipName == "RunAttack" ||
                               (*t_upperSkeletalAnimationComponent->animationTransitions)["RunAttack"].startTime > 0.0f ||
                               t_upperSkeletalAnimationComponent->clipName == "JumpAttack" ||
                               (*t_upperSkeletalAnimationComponent->animationTransitions)["JumpAttack"].startTime > 0.0f)
                            {
                                // Om underkroppen inte är i attackläge börja göra dens animation
                                if(true ||
                                   t_lowerSkeletalAnimationComponent->clipName != "Attack" &&
                                       (*t_lowerSkeletalAnimationComponent->animationTransitions)["Attack"].startTime == 0.0f)
                                {
                                    std::string t_newestClip = "";
                                    float t_lowestTimeElapsed = 9999;
                                    t_newestClip = CheckForTransitions(t_lowestTimeElapsed, t_lowerSkeletalAnimationComponent);
                                    if(t_newestClip != "")
                                    {
                                        float t_elapsedTime = (*t_lowerSkeletalAnimationComponent->animationTransitions)[t_newestClip].elapsedSinceStart -
                                                              (*t_lowerSkeletalAnimationComponent->animationTransitions)[t_newestClip].startTime;
                                        if(t_elapsedTime == 0.0f)
                                        {
                                            t_elapsedTime = 0.001f;
                                        }
                                        STimer(t_newestClip, t_upperSkeletalAnimationComponent, t_elapsedTime);
                                        // cout << t_newestClip << " Var det nyaste!" << endl;
                                    }
                                    else
                                    {
                                        STimer(t_lowerSkeletalAnimationComponent->clipName, t_upperSkeletalAnimationComponent,
                                               t_lowerSkeletalAnimationComponent->timePosition);
                                    }
                                }
                                // (*t_upperSkeletalAnimationComponent->animationTransitions)["Attack"].startTime = 0.0f;
                                // (*t_upperSkeletalAnimationComponent->animationTransitions)["Attack"].elapsedSinceStart = 0.0f;
                            }

                            (*t_lowerSkeletalAnimationComponent->animationTransitions)["Attack"].startTime = 0.0f;
                            (*t_lowerSkeletalAnimationComponent->animationTransitions)["Attack"].elapsedSinceStart = 0.0f;
                            (*t_upperSkeletalAnimationComponent->animationTransitions)["Attack"].startTime = 0.0f;
                            (*t_upperSkeletalAnimationComponent->animationTransitions)["Attack"].elapsedSinceStart = 0.0f;
                            (*t_upperSkeletalAnimationComponent->animationTransitions)["RunAttack"].startTime = 0.0f;
                            (*t_upperSkeletalAnimationComponent->animationTransitions)["RunAttack"].elapsedSinceStart = 0.0f;
                            (*t_upperSkeletalAnimationComponent->animationTransitions)["JumpAttack"].startTime = 0.0f;
                            (*t_upperSkeletalAnimationComponent->animationTransitions)["JumpAttack"].elapsedSinceStart = 0.0f;
                        }
                        else if(t_animationTransitionEvent->animation == Animation::JUMP)
                        {
                            if((*t_upperSkeletalAnimationComponent->animationTransitions)["Jump"].startTime == 0.0f &&
                               t_upperSkeletalAnimationComponent->clipName != "Jump")
                            {
                                if((*t_upperSkeletalAnimationComponent->animationTransitions)["Attack"].startTime > 0.0f ||
                                   t_upperSkeletalAnimationComponent->clipName == "Attack" ||
                                   (*t_upperSkeletalAnimationComponent->animationTransitions)["RunAttack"].startTime > 0.0f ||
                                   t_upperSkeletalAnimationComponent->clipName == "RunAttack")
                                {
                                    // JUMPATTACK
                                    if(t_upperSkeletalAnimationComponent->clipName == "Attack" ||
                                       t_upperSkeletalAnimationComponent->clipName == "RunAttack")
                                    {
                                        STimer("JumpAttack", t_upperSkeletalAnimationComponent, 0.001f);
                                    }
                                    else if((*t_upperSkeletalAnimationComponent->animationTransitions)["Attack"].startTime > 0.0f ||
                                            (*t_upperSkeletalAnimationComponent->animationTransitions)["RunAttack"].startTime > 0.0f)
                                    {
                                        std::string t_newestClip;
                                        float t_lowestTime = 9999;
                                        t_newestClip = CheckForTransitions(t_lowestTime, t_upperSkeletalAnimationComponent);
                                        if(t_newestClip == "Attack" || t_newestClip == "RunAttack")
                                        {
                                            STimer("JumpAttack", t_upperSkeletalAnimationComponent, 0.001f);
                                        }
                                        else
                                        {
                                            STimer("Jump", t_upperSkeletalAnimationComponent, 0.001f);
                                        }
                                    }
                                }
                                else
                                {
                                    STimer("Jump", t_upperSkeletalAnimationComponent, 0.001f);
                                }
                            }
                            // Om jag är attack eller har startat attack men håller på att transitiona ifrån attack ska jag hoppa istället!
                            /*if(t_upperSkeletalAnimationComponent->clipName == "Attack" ||
                               (*t_upperSkeletalAnimationComponent->animationTransitions)["Attack"].startTime > 0)
                            {
                                std::string t_newestClip = "";
                                float t_lowestTimeElapsed = 0;
                                t_newestClip = CheckForTransitions(t_lowestTimeElapsed, t_upperSkeletalAnimationComponent);
                                if (t_newestClip != "" && t_newestClip != "Attack")
                                {
                                    STimer("Jump", t_upperSkeletalAnimationComponent, 0.001f);
                                }
                            }*/
                            if((*t_lowerSkeletalAnimationComponent->animationTransitions)["Jump"].startTime == 0 &&
                               t_lowerSkeletalAnimationComponent->clipName != "Jump")
                            {
                                STimer("Jump", t_lowerSkeletalAnimationComponent, 0.001f);
                            }
                        }
                    }
                    break;
                }

                case Doremi::Core::EventType::DamageTaken:
                {
                    DamageTakenEvent* t_event = static_cast<DamageTakenEvent*>(p_event);
                    LowerSkeletalAnimationComponent* t_lowerSkeletalAnimationComponent =
                        EntityHandler::GetInstance().GetComponentFromStorage<LowerSkeletalAnimationComponent>(t_event->entityId);
                    SkeletalAnimationComponent* t_upperSkeletalAnimationComponent =
                        EntityHandler::GetInstance().GetComponentFromStorage<SkeletalAnimationComponent>(t_event->entityId);
                    STimer("Hit", t_upperSkeletalAnimationComponent, 0.001f);
                    if(t_lowerSkeletalAnimationComponent->clipName != "Run")
                    {
                        STimer("Hit", t_lowerSkeletalAnimationComponent, 0.001f);
                    }
                    break;
                }
                break;
            }
        }

        void SkeletalAnimationCoreManager::UpdatePlayer(double p_dt, SkeletalAnimationComponent* t_skeletalAnimationComponent,
                                                        LowerSkeletalAnimationComponent* t_lowerSkeletalAnimationComponent, const size_t& j)
        {
            // Uppdatera tiderna
            t_skeletalAnimationComponent->timePosition += p_dt;
            t_lowerSkeletalAnimationComponent->timePosition += p_dt;
            // Uppdatera alla transitions
            UpdateTransitionsPlayer(*t_skeletalAnimationComponent, *t_lowerSkeletalAnimationComponent, p_dt);
            // Check if animationtimeelapsed is more than the cliplength. If so reset cliptime
            float t_upperBodyClipTime = t_skeletalAnimationComponent->skeletalInformation->GetClipEndTime(t_skeletalAnimationComponent->clipName);
            float t_lowerBodyClipTime = t_lowerSkeletalAnimationComponent->skeletalInformation->GetClipEndTime(t_lowerSkeletalAnimationComponent->clipName);
            if(t_skeletalAnimationComponent->timePosition > t_upperBodyClipTime)
            {
                // Check if this clip is supposed to be looped. If not start playing idle or if we are transitioning just let it transition
                if(!t_skeletalAnimationComponent->skeletalInformation->GetAnimationClip(t_skeletalAnimationComponent->clipName).loop)
                {
                    t_skeletalAnimationComponent->timePosition -= t_upperBodyClipTime;
                    float t_lowestTime = 0;
                    std::string t_string = CheckForTransitions(t_lowestTime, t_skeletalAnimationComponent);
                    if(t_string == "" && t_skeletalAnimationComponent->clipName != "JumpAttack")
                    {
                        STimer("Idle", t_skeletalAnimationComponent, t_skeletalAnimationComponent->timePosition);
                    }
                    else if(t_skeletalAnimationComponent->clipName == "JumpAttack")
                    {
                        float t_epsilon = 0.05f;
                        TransformComponentNext* t_transformComponentNext = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponentNext>(j);
                        TransformComponentPrevious* t_transformComponentPrevious =
                            EntityHandler::GetInstance().GetComponentFromStorage<TransformComponentPrevious>(j);
                        XMFLOAT3 t_movementVector;
                        XMStoreFloat3(&t_movementVector, XMLoadFloat3(&t_transformComponentNext->position) - XMLoadFloat3(&t_transformComponentPrevious->position));
                        XMFLOAT3 t_movementLengthVector;
                        // Calculate the length of this vector
                        XMStoreFloat3(&t_movementLengthVector, XMVector3Length(XMLoadFloat3(&t_movementVector)));
                        if(t_movementLengthVector.x > t_epsilon)
                        {
                            STimer("RunAttack", t_skeletalAnimationComponent, 0.001f);
                        }
                        else
                        {
                            // cout << "WTF" << endl;
                            STimer("Attack", t_skeletalAnimationComponent, 0.001f);
                        }
                    }
                }
                else
                {
                    t_skeletalAnimationComponent->timePosition -= t_upperBodyClipTime;
                }
            }
            if(t_lowerSkeletalAnimationComponent->timePosition > t_lowerBodyClipTime)
            {
                // Check if this clip is supposed to be looped. If not start playing idle or if we are transitioning just let it transition
                if(!t_lowerSkeletalAnimationComponent->skeletalInformation->GetAnimationClip(t_lowerSkeletalAnimationComponent->clipName).loop)
                {
                    t_lowerSkeletalAnimationComponent->timePosition -= t_lowerBodyClipTime;
                    float t_lowestTime = 0;
                    std::string t_string = CheckForTransitions(t_lowestTime, t_lowerSkeletalAnimationComponent);
                    if(t_string == "" && t_skeletalAnimationComponent->clipName != "JumpAttack")
                    {
                        STimer("Idle", t_lowerSkeletalAnimationComponent, t_lowerSkeletalAnimationComponent->timePosition);
                    }
                    else if(t_skeletalAnimationComponent->clipName == "JumpAttack")
                    {
                        float t_epsilon = 0.05f;
                        TransformComponentNext* t_transformComponentNext = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponentNext>(j);
                        TransformComponentPrevious* t_transformComponentPrevious =
                            EntityHandler::GetInstance().GetComponentFromStorage<TransformComponentPrevious>(j);
                        XMFLOAT3 t_movementVector;
                        XMStoreFloat3(&t_movementVector, XMLoadFloat3(&t_transformComponentNext->position) - XMLoadFloat3(&t_transformComponentPrevious->position));
                        XMFLOAT3 t_movementLengthVector;
                        // Calculate the length of this vector
                        XMStoreFloat3(&t_movementLengthVector, XMVector3Length(XMLoadFloat3(&t_movementVector)));
                        if(t_movementLengthVector.x > t_epsilon)
                        {
                            STimer("Run", t_lowerSkeletalAnimationComponent, 0.001f);
                        }
                        else
                        {
                            STimer("Attack", t_lowerSkeletalAnimationComponent, 0.001f);
                        }
                    }
                }
                else
                {
                    t_lowerSkeletalAnimationComponent->timePosition -= t_lowerBodyClipTime;
                }
            }
            // Check if this enity should change animation and start the transition
            CheckANDPerformAnimationTransitionPlayer(j);
            // Make a transformationmatrix per bone
            int t_numberOfTransformationMatrices = t_skeletalAnimationComponent->skeletalInformation->GetBoneCount();
            int t_numberOfLowerBodyTransforms = t_lowerSkeletalAnimationComponent->skeletalInformation->GetBoneCount();
            // Declare and define a vector to the length of the number of joints/bones
            std::vector<DirectX::XMFLOAT4X4> t_finalTransformations(t_numberOfTransformationMatrices);
            std::vector<DirectX::XMFLOAT4X4> t_lowerBodyFinalTransformations(t_numberOfLowerBodyTransforms);
            // Send to graphicmodule to compute the final transformationmatrices for this timeinstant.
            /*m_sharedContext.GetGraphicModule().GetSubModuleManager().GetSkeletalAnimationManager().GetFinalTransforms(
            t_skeletalAnimationComponent->clipName, (float)t_skeletalAnimationComponent->timePosition, t_finalTransformations,
            t_skeletalAnimationComponent->skeletalInformation);*/
            m_sharedContext.GetGraphicModule().GetSubModuleManager().GetSkeletalAnimationManager().GetInterpolatedAnimationsMatrices(
                t_skeletalAnimationComponent->skeletalInformation, t_skeletalAnimationComponent->clipName,
                (float)t_skeletalAnimationComponent->timePosition, t_finalTransformations, *t_skeletalAnimationComponent->animationTransitions);
            // Do the same for lowerbody
            /*m_sharedContext.GetGraphicModule().GetSubModuleManager().GetSkeletalAnimationManager().GetFinalTransforms(
            t_lowerSkeletalAnimationComponent->clipName, (float)t_lowerSkeletalAnimationComponent->timePosition,
            t_lowerBodyFinalTransformations, t_lowerSkeletalAnimationComponent->skeletalInformation);*/
            m_sharedContext.GetGraphicModule().GetSubModuleManager().GetSkeletalAnimationManager().GetInterpolatedAnimationsMatrices(
                t_lowerSkeletalAnimationComponent->skeletalInformation, t_lowerSkeletalAnimationComponent->clipName,
                (float)t_lowerSkeletalAnimationComponent->timePosition, t_lowerBodyFinalTransformations, *t_lowerSkeletalAnimationComponent->animationTransitions);
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
            XMFLOAT3 offsetPosition = XMFLOAT3(orientationComp->position.x, orientationComp->position.y + renderComp->offsetY, orientationComp->position.z);
            DirectX::XMVECTOR quaternion = DirectX::XMLoadFloat4(&orientationComp->rotation);
            DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&offsetPosition);
            DirectX::XMMATRIX tempTransMat;
            if(renderComp->lockedRotationX)
            {
                XMVECTOR right = XMLoadFloat3(&XMFLOAT3(1, 0, 0));
                right = XMVector3Rotate(right, quaternion);
                XMVECTOR up = XMLoadFloat3(&XMFLOAT3(0, 1, 0));
                XMVECTOR forward = XMVector3Cross(right, up);
                tempTransMat =
                    DirectX::XMMatrixTranspose(DirectX::XMMatrixScaling(orientationComp->scale.x, orientationComp->scale.y, orientationComp->scale.z) *
                                               XMMatrixInverse(nullptr, XMMatrixLookAtLH(position, position + forward, up)));
            }
            else
            {
                tempTransMat =
                    DirectX::XMMatrixTranspose(DirectX::XMMatrixScaling(orientationComp->scale.x, orientationComp->scale.y, orientationComp->scale.z) *
                                               DirectX::XMMatrixRotationQuaternion(quaternion) * DirectX::XMMatrixTranslationFromVector(position));
            }

            DoremiEngine::Graphic::SubModuleManager& submoduleManager = m_sharedContext.GetGraphicModule().GetSubModuleManager();
            DirectX::XMStoreFloat4x4(&transMat, tempTransMat);
            submoduleManager.GetMeshManager().AddToRenderList(*renderComp->mesh, *renderComp->material, transMat);
            // Set the Raster and depthstencilstate
            m_rasterizerState->GetRasterizerState();
            m_depthStencilState->GetDepthStencilState();
            // Draw the skeletalmesh uses another drawmethod than the common one. Since now we have more information in the vertex

            submoduleManager.GetDirectXManager().DrawCurrentRenderListSkeletal(m_rasterizerState->GetRasterizerState(),
                                                                               m_depthStencilState->GetDepthStencilState());
        }

        void SkeletalAnimationCoreManager::UpdateEnemy(double p_dt, SkeletalAnimationComponent* t_skeletalAnimationComponent,
                                                       LowerSkeletalAnimationComponent* t_lowerSkeletalAnimationComponent, const size_t& j)
        {
            // Uppdatera tiderna
            t_skeletalAnimationComponent->timePosition += p_dt;
            t_lowerSkeletalAnimationComponent->timePosition += p_dt;
            // Uppdatera alla transitions
            UpdateTransitionsEnemy(*t_skeletalAnimationComponent, *t_lowerSkeletalAnimationComponent, p_dt);
            // Check if animationtimeelapsed is more than the cliplength. If so reset cliptime
            float t_upperBodyClipTime = t_skeletalAnimationComponent->skeletalInformation->GetClipEndTime(t_skeletalAnimationComponent->clipName);
            float t_lowerBodyClipTime = t_lowerSkeletalAnimationComponent->skeletalInformation->GetClipEndTime(t_lowerSkeletalAnimationComponent->clipName);
            if(t_skeletalAnimationComponent->timePosition > t_upperBodyClipTime)
            {
                // Check if this clip is supposed to be looped. If not start playing idle
                if(!t_skeletalAnimationComponent->skeletalInformation->GetAnimationClip(t_skeletalAnimationComponent->clipName).loop)
                {
                    // t_skeletalAnimationComponent->clipName = "Idle";
                    // t_skeletalAnimationComponent->skeletalInformation->GetAnimationBlend("Idle").StartTimer();
                    STimer("Idle", t_skeletalAnimationComponent, 0.001f);
                    t_skeletalAnimationComponent->timePosition -= t_upperBodyClipTime;
                    // t_skeletalAnimationComponent->timePosition = 0.0f;
                }
                else
                {
                    t_skeletalAnimationComponent->timePosition -= t_upperBodyClipTime;
                }
            }
            if(t_lowerSkeletalAnimationComponent->timePosition > t_lowerBodyClipTime)
            {
                // Check if this clip is supposed to be looped. If not start playing idle
                if(!t_lowerSkeletalAnimationComponent->skeletalInformation->GetAnimationClip(t_lowerSkeletalAnimationComponent->clipName).loop)
                {
                    // t_lowerSkeletalAnimationComponent->skeletalInformation->GetAnimationBlend("Idle").StartTimer();
                    STimer("Idle", t_lowerSkeletalAnimationComponent, 0.001f);
                    t_lowerSkeletalAnimationComponent->timePosition -= t_lowerBodyClipTime;
                    /*t_lowerSkeletalAnimationComponent->clipName = "Idle";
                    t_lowerSkeletalAnimationComponent->timePosition = 0.0f;*/
                }
                else
                {
                    t_lowerSkeletalAnimationComponent->timePosition -= t_lowerBodyClipTime;
                }
            }
            // Check if this enity should change animation and start the transition
            CheckANDPerformAnimationTransitionEnemy(j);
            // Make a transformationmatrix per bone
            int t_numberOfTransformationMatrices = t_skeletalAnimationComponent->skeletalInformation->GetBoneCount();
            int t_numberOfLowerBodyTransforms = t_lowerSkeletalAnimationComponent->skeletalInformation->GetBoneCount();
            // Declare and define a vector to the length of the number of joints/bones
            std::vector<DirectX::XMFLOAT4X4> t_finalTransformations(t_numberOfTransformationMatrices);
            std::vector<DirectX::XMFLOAT4X4> t_lowerBodyFinalTransformations(t_numberOfLowerBodyTransforms);
            // Send to graphicmodule to compute the final transformationmatrices for this timeinstant.
            /*m_sharedContext.GetGraphicModule().GetSubModuleManager().GetSkeletalAnimationManager().GetFinalTransforms(
            t_skeletalAnimationComponent->clipName, (float)t_skeletalAnimationComponent->timePosition, t_finalTransformations,
            t_skeletalAnimationComponent->skeletalInformation);*/
            m_sharedContext.GetGraphicModule().GetSubModuleManager().GetSkeletalAnimationManager().GetInterpolatedAnimationsMatrices(
                t_skeletalAnimationComponent->skeletalInformation, t_skeletalAnimationComponent->clipName,
                (float)t_skeletalAnimationComponent->timePosition, t_finalTransformations, *t_skeletalAnimationComponent->animationTransitions);
            // Do the same for lowerbody
            /*m_sharedContext.GetGraphicModule().GetSubModuleManager().GetSkeletalAnimationManager().GetFinalTransforms(
            t_lowerSkeletalAnimationComponent->clipName, (float)t_lowerSkeletalAnimationComponent->timePosition,
            t_lowerBodyFinalTransformations, t_lowerSkeletalAnimationComponent->skeletalInformation);*/
            m_sharedContext.GetGraphicModule().GetSubModuleManager().GetSkeletalAnimationManager().GetInterpolatedAnimationsMatrices(
                t_lowerSkeletalAnimationComponent->skeletalInformation, t_lowerSkeletalAnimationComponent->clipName,
                (float)t_lowerSkeletalAnimationComponent->timePosition, t_lowerBodyFinalTransformations, *t_lowerSkeletalAnimationComponent->animationTransitions);
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
            XMFLOAT3 offsetPosition = XMFLOAT3(orientationComp->position.x, orientationComp->position.y + renderComp->offsetY, orientationComp->position.z);
            DirectX::XMVECTOR quaternion = DirectX::XMLoadFloat4(&orientationComp->rotation);
            DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&offsetPosition);
            DirectX::XMMATRIX tempTransMat;
            if(renderComp->lockedRotationX)
            {
                XMVECTOR right = XMLoadFloat3(&XMFLOAT3(1, 0, 0));
                right = XMVector3Rotate(right, quaternion);
                XMVECTOR up = XMLoadFloat3(&XMFLOAT3(0, 1, 0));
                XMVECTOR forward = XMVector3Cross(right, up);
                tempTransMat =
                    DirectX::XMMatrixTranspose(DirectX::XMMatrixScaling(orientationComp->scale.x, orientationComp->scale.y, orientationComp->scale.z) *
                                               XMMatrixInverse(nullptr, XMMatrixLookAtLH(position, position + forward, up)));
            }
            else
            {
                tempTransMat =
                    DirectX::XMMatrixTranspose(DirectX::XMMatrixScaling(orientationComp->scale.x, orientationComp->scale.y, orientationComp->scale.z) *
                                               DirectX::XMMatrixRotationQuaternion(quaternion) * DirectX::XMMatrixTranslationFromVector(position));
            }

            DirectX::XMStoreFloat4x4(&transMat, tempTransMat);
            DoremiEngine::Graphic::SubModuleManager& submoduleManager = m_sharedContext.GetGraphicModule().GetSubModuleManager();
            submoduleManager.GetMeshManager().AddToRenderList(*renderComp->mesh, *renderComp->material, transMat);
            // Set the Raster and depthstencilstate
            m_rasterizerState->GetRasterizerState();
            m_depthStencilState->GetDepthStencilState();
            // Draw the skeletalmesh uses another drawmethod than the common one. Since now we have more information in the vertex

            submoduleManager.GetDirectXManager().DrawCurrentRenderListSkeletal(m_rasterizerState->GetRasterizerState(),
                                                                               m_depthStencilState->GetDepthStencilState());
        }

        void SkeletalAnimationCoreManager::Update(double p_dt)
        {
            // Loop through all entities
            const size_t t_length = EntityHandler::GetInstance().GetLastEntityIndex();
            // Loop over all entities to perform various functions on enteties that have skeletal animation
            int t_mask = (int)ComponentType::Render | (int)ComponentType::Transform | (int)ComponentType::UpperBodySkeletalAnimation |
                         (int)ComponentType::LowerBodySkeletalAnimation;
            // Set shaders

            DoremiEngine::Graphic::SubModuleManager& submoduleManager = m_sharedContext.GetGraphicModule().GetSubModuleManager();
            submoduleManager.GetShaderManager().SetActiveVertexShader(m_vertexShader);
            submoduleManager.GetShaderManager().SetActivePixelShader(m_pixelShader);
            int counter = 0;
            for(size_t j = 0; j < t_length; j++)
            {
                // Check if entity has skeletalanimation
                if(EntityHandler::GetInstance().HasComponents(j, t_mask))
                {
                    PlayerHandlerClient* t_playerHandler = static_cast<PlayerHandlerClient*>(PlayerHandler::GetInstance());

                    if(!t_playerHandler->PlayerExists())
                    {
                        continue;
                    }

                    EntityID t_playerEntityID = t_playerHandler->GetPlayerEntityID();

                    TransformComponentNext* t_transformComponentNext = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponentNext>(j);
                    TransformComponentNext* t_playerTransform = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponentNext>(t_playerEntityID);
                    XMFLOAT3 t_playervector;
                    XMStoreFloat3(&t_playervector, XMLoadFloat3(&t_transformComponentNext->position) - XMLoadFloat3(&t_playerTransform->position));
                    XMFLOAT3 t_playerLengthVector;
                    // Calculate the length of this vector
                    XMStoreFloat3(&t_playerLengthVector, XMVector3Length(XMLoadFloat3(&t_playervector)));
                    if(t_playerLengthVector.x > 500)
                    {
                        continue;
                    }

                    // Get component and update time that the animation has been active
                    SkeletalAnimationComponent* t_skeletalAnimationComponent =
                        EntityHandler::GetInstance().GetComponentFromStorage<SkeletalAnimationComponent>(j);
                    LowerSkeletalAnimationComponent* t_lowerSkeletalAnimationComponent =
                        EntityHandler::GetInstance().GetComponentFromStorage<LowerSkeletalAnimationComponent>(j);

                    if((*t_skeletalAnimationComponent->animationTransitions).count("JumpAttack") < 1)
                    {
                        UpdateEnemy(p_dt, t_skeletalAnimationComponent, t_lowerSkeletalAnimationComponent, j);
                    }
                    else
                    {
                        UpdatePlayer(p_dt, t_skeletalAnimationComponent, t_lowerSkeletalAnimationComponent, j);
                    }
                }
                else
                {
                    // do nothing
                }
            }
        }
    }
}
