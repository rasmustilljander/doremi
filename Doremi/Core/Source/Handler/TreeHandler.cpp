#include <Doremi/Core/Include/Handler/TreeHandler.hpp>
#include <DoremiEngine/Physics/Include/RigidBodyManager.hpp>
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/PhysicsMaterialManager.hpp>
#include <DoremiEngine/Physics/Include/RayCastManager.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <Doremi/Core/Include/CameraHandler.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Camera/Camera.hpp>
#include <DoremiEngine/Configuration/Include/ConfigurationModule.hpp>
#include <DirectXMath.h>
#include <DoremiEngine/Graphic/Include/VertexStruct.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Mesh/MeshInfo.hpp>
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/MeshManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Mesh/MaterialInfo.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/DirectXManager.hpp>
#include <iostream>
#include <algorithm>
#include <Windows.h>

#include <Doremi/Core/Include/PlayerHandlerClient.hpp>
// SKITKOD TODOEA
// Project specific
#include <Manager/GraphicManager.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <EntityComponent/Components/RenderComponent.hpp>
#include <EntityComponent/Components/TransformComponent.hpp>
#include <Doremi/Core/Include/Handler/TreeHandler.hpp>
// Engine
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/MeshManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/DirectXManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/SubModuleManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/ShaderManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Mesh/MeshInfo.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Mesh/MaterialInfo.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Shader/PixelShader.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Shader/VertexShader.hpp>
#include <DoremiEngine/Graphic/Include/Interface/State/DepthStencilState.hpp>
#include <DoremiEngine/Graphic/Include/Interface/State/RasterizerState.hpp>
#include <dxgi.h>
#include <d3d11_1.h>

// TIMER
#include <Doremi/Core/Include/Timing/NamedTimer.hpp>
#include <Doremi/Core/Include/Timing/FunctionTimer.hpp>
//
namespace Doremi
{
    namespace Core
    {
        TreeHandler* TreeHandler::m_singleton = nullptr;

        TreeHandler::TreeHandler(const DoremiEngine::Core::SharedContext& p_sharedContext) : m_sharedContext(p_sharedContext)
        {
            m_treeCreator = new TreeCreator(p_sharedContext);
            m_viewDist = m_sharedContext.GetConfigurationModule().GetAllConfigurationValues().CameraViewDistance;
            m_fov = m_sharedContext.GetConfigurationModule().GetAllConfigurationValues().CameraFieldOfView;
            m_nearDist = 0.1f;
            m_heighWidthNear = 2 * tan(m_fov * 0.5f) * m_nearDist;
            m_heighWidthFar = 2 * tan(m_fov * 0.5f) * m_viewDist;

            //// SKITKOD TODOEA
            // TODOKO Should not be here!! or should it? For standard shaders? Maybee in shadermanager
            // TODOLH Maybe shouldnt be here either. Moved it from shadermodulemanagerImplementation cos this guy needs to be able to switch shader
            // before drawing
            // D3D11_INPUT_ELEMENT_DESC ied[] = {
            //    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            //    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            //    { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            //};
            // m_vertexShader =
            // m_sharedContext.GetGraphicModule().GetSubModuleManager().GetShaderManager().BuildVertexShader("BasicVertexShader.hlsl",
            //    ied, ARRAYSIZE(ied));
            // m_pixelShader = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetShaderManager().BuildPixelShader("BasicPixelShader.hlsl");

            // D3D11_RASTERIZER_DESC rastDesc;
            // ZeroMemory(&rastDesc, sizeof(rastDesc));
            // rastDesc.FillMode = D3D11_FILL_SOLID;
            // rastDesc.CullMode = D3D11_CULL_NONE;
            // rastDesc.FrontCounterClockwise = false;
            // rastDesc.DepthBias = 0;
            // rastDesc.DepthBiasClamp = 0.0f;
            // rastDesc.SlopeScaledDepthBias = 0.0f;
            // rastDesc.DepthClipEnable = false;
            // rastDesc.ScissorEnable = false;
            // rastDesc.MultisampleEnable = true;
            // rastDesc.AntialiasedLineEnable = false;
            // m_rasterizerState = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager().CreateRasterizerState(rastDesc);
            // D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
            // ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
            // depthStencilDesc.DepthEnable = true;
            // depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
            // depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
            // m_depthStencilState =
            // m_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager().CreateDepthStencilState(depthStencilDesc);
        }

        TreeHandler* TreeHandler::GetInstance()
        {
            if(m_singleton == nullptr)
            {
                std::cout << "Called GetInstance before TreeHandler was called" << std::endl;
            }
            return m_singleton;
        }
        void TreeHandler::StartupTreeHandler(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            if(m_singleton != nullptr)
            {
                std::runtime_error("StartupTreeHandler was called");
            }
            m_singleton = new TreeHandler(p_sharedContext);
        }
        void TreeHandler::BuildTheTree() { m_treeCreator->CreateTree(); }
        TreeHandler::~TreeHandler() {}
        bool TreeHandler::CollisionCheckForBox(DirectX::XMFLOAT3 p_center, DirectX::XMFLOAT3 p_dimensions)
        {
            FUNCTION_TIMER;
            // std::cout << "X:" << m_playerPos.x << " Y:" << m_playerPos.y << " Z:" << m_playerPos.z << std::endl;
            // Check if any one point of the cube is in the view frustum.
            for(int i = 0; i < 6; ++i)
            {
                // for circle
                //DirectX::XMFLOAT3 t_collideCheckFloats;
                //DirectX::XMStoreFloat3(&t_collideCheckFloats, DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_planes[i].x,
                //m_planes[i].y, m_planes[i].z)), DirectX::XMLoadFloat3(&DirectX::XMFLOAT3((p_center)))));
                // if (t_collideCheckFloats.x - m_planes[i].w >= 0)
                //{
                //    return false;
                //}
                //Cirkel
                //if(abs(p_center.x - m_playerPos.x) + abs(p_center.y - m_playerPos.y) + abs(p_center.z - m_playerPos.z) > 10)
                //{
                //    return false;
                //}
                // cirkelslut
                // if (/*t_collideCheckFloats.x + m_planes[i].w + p_dimensions.x*/p_center.x > 200)
                //{
                //    return false;
                //}
                // if (/*t_collideCheckFloats.x + m_planes[i].w + p_dimensions.x*/p_center.y > 200)
                //{
                //    return false;
                //}
                 ////För box PLAN
                 DirectX::XMFLOAT3 t_collideCheckFloats;

                 DirectX::XMStoreFloat3(&t_collideCheckFloats, DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_planes[i].x,
                 m_planes[i].y, m_planes[i].z)), DirectX::XMLoadFloat3(&DirectX::XMFLOAT3((p_center.x - p_dimensions.x), (p_center.y -
                 p_dimensions.y), (p_center.z - p_dimensions.z)))));// +m_planes[i].w;)
                 if (t_collideCheckFloats.x - m_planes[i].w <= 0.0f)
                {
                    continue;
                }
                 DirectX::XMStoreFloat3(&t_collideCheckFloats, DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_planes[i].x,
                 m_planes[i].y, m_planes[i].z)), DirectX::XMLoadFloat3(&DirectX::XMFLOAT3((p_center.x + p_dimensions.x), (p_center.y -
                 p_dimensions.y), (p_center.z - p_dimensions.z)))));// +m_planes[i].w;)
                 if (t_collideCheckFloats.x - m_planes[i].w <= 0.0f)
                {
                    continue;
                }
                 DirectX::XMStoreFloat3(&t_collideCheckFloats, DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_planes[i].x,
                 m_planes[i].y, m_planes[i].z)), DirectX::XMLoadFloat3(&DirectX::XMFLOAT3((p_center.x - p_dimensions.x), (p_center.y +
                 p_dimensions.y), (p_center.z - p_dimensions.z)))));// +m_planes[i].w;)
                 if (t_collideCheckFloats.x - m_planes[i].w <= 0.0f)
                {
                    continue;
                }
                 DirectX::XMStoreFloat3(&t_collideCheckFloats, DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_planes[i].x,
                 m_planes[i].y, m_planes[i].z)), DirectX::XMLoadFloat3(&DirectX::XMFLOAT3((p_center.x - p_dimensions.x), (p_center.y -
                 p_dimensions.y), (p_center.z + p_dimensions.z)))));// +m_planes[i].w;)
                 if (t_collideCheckFloats.x - m_planes[i].w <= 0.0f)
                {
                    continue;
                }
                 DirectX::XMStoreFloat3(&t_collideCheckFloats, DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_planes[i].x,
                 m_planes[i].y, m_planes[i].z)), DirectX::XMLoadFloat3(&DirectX::XMFLOAT3((p_center.x + p_dimensions.x), (p_center.y +
                 p_dimensions.y), (p_center.z - p_dimensions.z)))));// +m_planes[i].w;)
                 if (t_collideCheckFloats.x - m_planes[i].w <= 0.0f)
                {
                    continue;
                }
                 DirectX::XMStoreFloat3(&t_collideCheckFloats, DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_planes[i].x,
                 m_planes[i].y, m_planes[i].z)), DirectX::XMLoadFloat3(&DirectX::XMFLOAT3((p_center.x + p_dimensions.x), (p_center.y -
                 p_dimensions.y), (p_center.z + p_dimensions.z)))));// +m_planes[i].w;)
                 if (t_collideCheckFloats.x - m_planes[i].w <= 0.0f)
                {
                    continue;
                }
                 DirectX::XMStoreFloat3(&t_collideCheckFloats, DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_planes[i].x,
                 m_planes[i].y, m_planes[i].z)), DirectX::XMLoadFloat3(&DirectX::XMFLOAT3((p_center.x - p_dimensions.x), (p_center.y +
                 p_dimensions.y), (p_center.z + p_dimensions.z)))));// +m_planes[i].w;)
                 if (t_collideCheckFloats.x - m_planes[i].w <= 0.0f)
                {
                    continue;
                }
                 DirectX::XMStoreFloat3(&t_collideCheckFloats, DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_planes[i].x,
                 m_planes[i].y, m_planes[i].z)), DirectX::XMLoadFloat3(&DirectX::XMFLOAT3((p_center.x + p_dimensions.x), (p_center.y +
                 p_dimensions.y), (p_center.z + p_dimensions.z)))));// +m_planes[i].w;)
                 if (t_collideCheckFloats.x - m_planes[i].w <= 0.0f)
                {
                    continue;
                }
                 return false;
                //////OriginalBOXEN
                // DirectX::XMFLOAT3 t_collideCheckFloats;
                // DirectX::XMStoreFloat3(&t_collideCheckFloats, DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_planes[i].x,
                // m_planes[i].y, m_planes[i].z)), DirectX::XMLoadFloat3(&DirectX::XMFLOAT3((p_center.x - p_dimensions.x), (p_center.y -
                // p_dimensions.y), (p_center.z - p_dimensions.z)))));// +m_planes[i].w;)
                // if (t_collideCheckFloats.x + m_planes[i].w >= 0.0f)
                //{
                //    continue;
                //}
                // DirectX::XMStoreFloat3(&t_collideCheckFloats, DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_planes[i].x,
                // m_planes[i].y, m_planes[i].z)), DirectX::XMLoadFloat3(&DirectX::XMFLOAT3((p_center.x + p_dimensions.x), (p_center.y -
                // p_dimensions.y), (p_center.z - p_dimensions.z)))));// +m_planes[i].w;)
                // if (t_collideCheckFloats.x + m_planes[i].w >= 0.0f)
                //{
                //    continue;
                //}
                // DirectX::XMStoreFloat3(&t_collideCheckFloats, DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_planes[i].x,
                // m_planes[i].y, m_planes[i].z)), DirectX::XMLoadFloat3(&DirectX::XMFLOAT3((p_center.x - p_dimensions.x), (p_center.y +
                // p_dimensions.y), (p_center.z - p_dimensions.z)))));// +m_planes[i].w;)
                // if (t_collideCheckFloats.x + m_planes[i].w >= 0.0f)
                //{
                //    continue;
                //}
                // DirectX::XMStoreFloat3(&t_collideCheckFloats, DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_planes[i].x,
                // m_planes[i].y, m_planes[i].z)), DirectX::XMLoadFloat3(&DirectX::XMFLOAT3((p_center.x - p_dimensions.x), (p_center.y -
                // p_dimensions.y), (p_center.z + p_dimensions.z)))));// +m_planes[i].w;)
                // if (t_collideCheckFloats.x + m_planes[i].w >= 0.0f)
                //{
                //    continue;
                //}
                // DirectX::XMStoreFloat3(&t_collideCheckFloats, DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_planes[i].x,
                // m_planes[i].y, m_planes[i].z)), DirectX::XMLoadFloat3(&DirectX::XMFLOAT3((p_center.x + p_dimensions.x), (p_center.y +
                // p_dimensions.y), (p_center.z - p_dimensions.z)))));// +m_planes[i].w;)
                // if (t_collideCheckFloats.x + m_planes[i].w >= 0.0f)
                //{
                //    continue;
                //}
                // DirectX::XMStoreFloat3(&t_collideCheckFloats, DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_planes[i].x,
                // m_planes[i].y, m_planes[i].z)), DirectX::XMLoadFloat3(&DirectX::XMFLOAT3((p_center.x + p_dimensions.x), (p_center.y -
                // p_dimensions.y), (p_center.z + p_dimensions.z)))));// +m_planes[i].w;)
                // if (t_collideCheckFloats.x + m_planes[i].w >= 0.0f)
                //{
                //    continue;
                //}
                // DirectX::XMStoreFloat3(&t_collideCheckFloats, DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_planes[i].x,
                // m_planes[i].y, m_planes[i].z)), DirectX::XMLoadFloat3(&DirectX::XMFLOAT3((p_center.x - p_dimensions.x), (p_center.y +
                // p_dimensions.y), (p_center.z + p_dimensions.z)))));// +m_planes[i].w;)
                // if (t_collideCheckFloats.x + m_planes[i].w >= 0.0f)
                //{
                //    continue;
                //}
                // DirectX::XMStoreFloat3(&t_collideCheckFloats, DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_planes[i].x,
                // m_planes[i].y, m_planes[i].z)), DirectX::XMLoadFloat3(&DirectX::XMFLOAT3((p_center.x + p_dimensions.x), (p_center.y +
                // p_dimensions.y), (p_center.z + p_dimensions.z)))));// +m_planes[i].w;)
                // if (t_collideCheckFloats.x + m_planes[i].w >= 0.0f)
                //{
                //    continue;
                //}
                // return false;
                 // Originalboxxen slut
            }
            return true;
        }
        std::vector<uint32_t> TreeHandler::Update()
        {
            FUNCTION_TIMER
            bool t_isDone = false;
            int t_whatChild = 0;
            TreeCreator::OctNode* t_currentNode = &m_treeCreator->treeRoot;

            ////////// COLLISION test
            // std::vector<int> t_sweepHits;
            // size_t numberOfHits = 0;
            // DirectX::XMFLOAT3 physicsCollideFloat;
            //////////

            // Getting the camera
            const DoremiEngine::Graphic::Camera& t_playerCamera = CameraHandler::GetInstance()->GetThirdPersonCamera();
            // Getting the matrices so we can build our frustum
            const DoremiEngine::Graphic::CameraMatrices t_camMatrices = t_playerCamera.GetCameraMatrices();
            // Needed variables
            // DirectX::XMFLOAT4X4 t_projection4x4 = t_camMatrices.ProjectionMatrix;
            //// DirectX::XMFLOAT4X4 t_view4x4 = t_camMatrices.ViewMatrix;
            // float t_zMin;
            // float r;
            // int32_t playerentityID = -13;
            // PlayerHandlerClient* playerHandlerClient = (PlayerHandlerClient*)PlayerHandlerClient::GetInstance();
            // TransformComponent* playerPos;
            // if (playerHandlerClient->PlayerExists())
            //{
            //    playerentityID = playerHandlerClient->GetPlayerEntityID();
            //    //TransformComponent* playerPos;
            //    playerPos = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(playerentityID);
            //    //playerPos = EntityHandler::GetInstance().GetComponentFromStorage<RigidBod>(playerentityID);
            //    // std::cout << "X:" << playerPos->position.x << " Y:" << playerPos->position.y << " Z:" << playerPos->position.z << std::endl;
            //    m_playerPos = playerPos->position;
            //}

            /// Ett nytt försök!

            DirectX::XMMATRIX directionMatrix = DirectX::XMLoadFloat4x4(&t_camMatrices.ViewMatrix); // *DirectX::XMVECTOR(0, 0, 1);
            DirectX::XMVECTOR forward = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(0, 0, 1));
            DirectX::XMVECTOR up = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(0, 1, 0));
            DirectX::XMVECTOR right = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(1, 0, 0));
            DirectX::XMVECTOR quaternion = DirectX::XMQuaternionRotationMatrix(directionMatrix);
            forward = DirectX::XMVector3Rotate(forward, quaternion);
            right = DirectX::XMVector3Rotate(right, quaternion);
            up = DirectX::XMVector3Rotate(up, quaternion);
            

            DirectX::XMVECTOR upDimensions = (up * m_heighWidthFar * 0.5f);
            DirectX::XMVECTOR rightDimensions = (right * m_heighWidthFar * 0.5f);
            DirectX::XMVECTOR forwardFar = DirectX::XMLoadFloat3(&t_camMatrices.CameraPosition) + forward * m_viewDist;
            DirectX::XMVECTOR farTopLeft = forwardFar + upDimensions - rightDimensions;
            DirectX::XMVECTOR farTopRight = forwardFar + upDimensions + rightDimensions;
            DirectX::XMVECTOR farBottomLeft = forwardFar - upDimensions - rightDimensions;
            DirectX::XMVECTOR farBottomRight = forwardFar - upDimensions + rightDimensions;

            DirectX::XMVECTOR upDimensionsNear = (up * m_heighWidthNear * 0.5f);
            DirectX::XMVECTOR rightDimensionsNear = (right * m_heighWidthNear * 0.5f);;
            DirectX::XMVECTOR forwardNear = DirectX::XMLoadFloat3(&t_camMatrices.CameraPosition) + forward * m_nearDist;
            DirectX::XMVECTOR nearTopLeft = forwardNear + upDimensionsNear - rightDimensionsNear;
            DirectX::XMVECTOR nearTopRight = forwardNear + upDimensionsNear + rightDimensionsNear;
            DirectX::XMVECTOR nearBottomLeft = forwardNear - upDimensionsNear - rightDimensionsNear;
            DirectX::XMVECTOR nearBottomRight = forwardNear - upDimensionsNear + rightDimensionsNear;




            // Near PLANE!!
            DirectX::XMVECTOR planePoint1 = nearBottomLeft;
            DirectX::XMVECTOR planePoint2 = nearTopLeft;
            DirectX::XMVECTOR planePoint3 = nearBottomRight;
            planePoint1 = planePoint1 - planePoint3;
            planePoint2 = planePoint2 - planePoint3;
            // normal vector of the plane
            planePoint1 = DirectX::XMVector3Cross( planePoint1, planePoint2);
            DirectX::XMFLOAT3 planeData;
            DirectX::XMStoreFloat3(&planeData, planePoint1);
            m_planes[0].x = planeData.x;
            m_planes[0].y = planeData.y;
            m_planes[0].z = planeData.z;
            planePoint1 = DirectX::XMVector3Dot(planePoint1, nearBottomLeft);
            DirectX::XMFLOAT3 dotProduct;
            DirectX::XMStoreFloat3(&dotProduct, planePoint1);
            m_planes[0].w = dotProduct.x;

            // Far Plane!
            planePoint1 = farBottomLeft;
            planePoint2 = farTopLeft;
            planePoint3 = farBottomRight;
            planePoint1 = planePoint1 - planePoint3;
            planePoint2 = planePoint2 - planePoint3;
            // normal vector of the plane
            planePoint1 = DirectX::XMVector3Cross(planePoint2, planePoint1);
            DirectX::XMStoreFloat3(&planeData, planePoint1);
            m_planes[1].x = planeData.x;
            m_planes[1].y = planeData.y;
            m_planes[1].z = planeData.z;
            planePoint1 = DirectX::XMVector3Dot(planePoint1, farBottomLeft);
            DirectX::XMStoreFloat3(&dotProduct, planePoint1);
            m_planes[1].w = dotProduct.x;

            //left plane
            planePoint1 = nearBottomLeft;
            planePoint2 = farBottomLeft;
            planePoint3 = nearTopLeft;
            planePoint1 = planePoint1 - planePoint3;
            planePoint2 = planePoint2 - planePoint3;
            // normal vector of the plane
            planePoint1 = DirectX::XMVector3Cross(planePoint1, planePoint2);
            DirectX::XMStoreFloat3(&planeData, planePoint1);
            m_planes[2].x = planeData.x;
            m_planes[2].y = planeData.y;
            m_planes[2].z = planeData.z;
            planePoint1 = DirectX::XMVector3Dot(planePoint1, nearBottomLeft);
            DirectX::XMStoreFloat3(&dotProduct, planePoint1);
            m_planes[2].w = dotProduct.x;

            //right plane
            planePoint1 = nearTopRight;
            planePoint2 = farTopRight;
            planePoint3 = nearBottomRight;
            planePoint1 = planePoint1 - planePoint3;
            planePoint2 = planePoint2 - planePoint3;
            // normal vector of the plane
            planePoint1 = DirectX::XMVector3Cross(planePoint1, planePoint2);
            DirectX::XMStoreFloat3(&planeData, planePoint1);
            m_planes[3].x = planeData.x;
            m_planes[3].y = planeData.y;
            m_planes[3].z = planeData.z;
            planePoint1 = DirectX::XMVector3Dot(planePoint1, nearTopRight);
            DirectX::XMStoreFloat3(&dotProduct, planePoint1);
            m_planes[3].w = dotProduct.x;

            //ABOVE!
            planePoint1 = nearTopLeft;
            planePoint2 = farTopLeft;
            planePoint3 = farTopRight;
            planePoint1 = planePoint1 - planePoint3;
            planePoint2 = planePoint2 - planePoint3;
            // normal vector of the plane
            planePoint1 = DirectX::XMVector3Cross(planePoint1, planePoint2);
            DirectX::XMStoreFloat3(&planeData, planePoint1);
            m_planes[4].x = planeData.x;
            m_planes[4].y = planeData.y;
            m_planes[4].z = planeData.z;
            planePoint1 = DirectX::XMVector3Dot(planePoint1, nearTopLeft);
            DirectX::XMStoreFloat3(&dotProduct, planePoint1);
            m_planes[4].w = dotProduct.x;

            //lower plane
            planePoint1 = nearBottomLeft;
            planePoint2 = farBottomLeft;
            planePoint3 = farBottomRight;
            planePoint1 = planePoint1 - planePoint3;
            planePoint2 = planePoint2 - planePoint3;
            // normal vector of the plane
            planePoint1 = DirectX::XMVector3Cross(planePoint2, planePoint1);
            DirectX::XMStoreFloat3(&planeData, planePoint1);
            m_planes[5].x = planeData.x;
            m_planes[5].y = planeData.y;
            m_planes[5].z = planeData.z;
            planePoint1 = DirectX::XMVector3Dot(planePoint1, nearBottomLeft);
            DirectX::XMStoreFloat3(&dotProduct, planePoint1);
            m_planes[5].w = dotProduct.x;


            // DoremiEngine::Graphic::VertexBasic tQuad[] = {
            //    { -1.0f, 1.0f, -0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f }, // 1 //Lilla boxen
            //    { -1.0f, -1.0f, -0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f }, // 2//Framsidan
            //    { 1.0f, -1.0f, -0.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f }, // 3
            //    { -1.0f, 1.0f, -0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f }, // 1//Framsidan
            //    { 1.0f, 1.0f, -0.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f }, // 4
            //    { 1.0f, -1.0f, -0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f }, // 3
            //
            //    { -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f }, // 4
            //    { -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f }, // 5  Baksidan
            //    { 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f }, // 6
            //    { -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f }, // 4  Baksidan
            //    { 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f }, // 7
            //    { 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f }, // 6
            //
            //
            //    { -1.0f, 1.0f, -0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f }, // ovanpå 8
            //    { -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f }, //// 9
            //    { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f }, ////10
            //    { -1.0f, 1.0f, -0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f }, // ovanpå 8
            //    { 1.0f, 1.0f, -0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f }, // 11
            //    { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f }, ////10
            //
            //
            //    { -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f }, // under 12
            //    { -1.0f, -1.0f, -0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f }, //// 13
            //    { 1.0f, -1.0f, -0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f }, ////14
            //    { -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f }, // under 12
            //    { 1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f }, // 15
            //    { 1.0f, -1.0f, -0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f }, ////14
            //
            //    { -1.0f, 1.0f, -0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f }, // vänster 16
            //    { -1.0f, -1.0f, -0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f }, //// 17
            //    { -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f }, ////18
            //    { -1.0f, 1.0f, -0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f }, // vänster 16
            //    { -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f }, // 19
            //    { -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f }, ////18
            //
            //    { 1.0f, 1.0f,1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f }, // höger 20
            //    { 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f }, //// 21
            //    { 1.0f, -1.0f, -0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f }, ////22
            //    { 1.0f, 1.0f,1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f }, // höger 20
            //    { 1.0f, 1.0f, -0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f }, // 23
            //    { 1.0f, -1.0f, -0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f }, ////22
            //};


            // int hej = ARRAYSIZE(tQuad);
            // std::vector<DoremiEngine::Graphic::Vertex> bufferList;
            // for (size_t i = 0; i < hej; i++)
            //{
            //    DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(tQuad[i].x, tQuad[i].y, tQuad[i].z));
            //    pos = DirectX::XMVector3Transform(pos, (DirectX::XMLoadFloat4x4( &t_camMatrices.ProjectionMatrix)));
            //    DirectX::XMFLOAT3 posFloat;
            //    DirectX::XMStoreFloat3(&posFloat, pos);
            //    DoremiEngine::Graphic::Vertex intoListVertex;
            //    intoListVertex.normal = DirectX::XMFLOAT3(tQuad[i].nx, tQuad[i].ny, tQuad[i].nz);
            //    intoListVertex.position = posFloat;
            //    intoListVertex.textureCoordinate.x = tQuad[i].texcoord[0];
            //    intoListVertex.textureCoordinate.y = tQuad[i].texcoord[1];
            //    bufferList.push_back(intoListVertex);
            //    // tQuad[i].x = posFloat.x;
            //    // tQuad[i].y = posFloat.y;
            //    // tQuad[i].z = posFloat.z;
            //}
            // static int meshSHit = 0;
            // DoremiEngine::Graphic::MaterialInfo* materialInfo =
            // m_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().BuildMaterialInfo("AngryFace.dds");
            // DoremiEngine::Graphic::MeshInfo* meshInfoFrustum =
            // m_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().BuildMeshInfoFromBuffer(bufferList, "FrustumMesasdasdh" +
            // std::to_string(meshSHit));
            // DirectX::XMFLOAT4X4A orianteiionFLoat;
            // DirectX::XMStoreFloat4x4(&orianteiionFLoat, DirectX::XMMatrixIdentity());
            // m_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().AddToRenderList(*meshInfoFrustum, *materialInfo,
            // orianteiionFLoat);
            // meshSHit++;


            // DirectX::XMMATRIX t_viewAndProjectionMatrix;
            ////t_zMin = -t_projection4x4._43 / t_projection4x4._33;
            ////r = m_viewDist / (m_viewDist - t_zMin);
            ////t_projection4x4._33 = r;
            ////t_projection4x4._43 = -r * t_zMin;
            // t_viewAndProjectionMatrix = DirectX::XMMatrixMultiply(XMLoadFloat4x4(&t_projection4x4), XMLoadFloat4x4(&t_camMatrices.ViewMatrix));
            ///////////t_viewAndProjectionMatrix = DirectX::XMMatrixMultiply(DirectX::XMMatrixInverse(nullptr
            // DirectX::XMMatrixTranspose(XMLoadFloat4x4(&t_projection4x4))), XMLoadFloat4x4(&t_camMatrices.ViewMatrix));
            // DirectX::XMFLOAT4X4 t_viewAndProjection4x4;
            // DirectX::XMStoreFloat4x4(&t_viewAndProjection4x4, t_viewAndProjectionMatrix);


            // m_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager().DrawCurrentRenderList(m_rasterizerState->GetRasterizerState(),
            // m_depthStencilState->GetDepthStencilState());


            // uint32_t cullingDimensions = 10;
            //// Near plane!
            // DirectX::XMVECTOR planePoint1 = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_playerPos.x - cullingDimensions, m_playerPos.y -
            // cullingDimensions, m_playerPos.z - cullingDimensions));
            // DirectX::XMVECTOR planePoint2 = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_playerPos.x + cullingDimensions, m_playerPos.y -
            // cullingDimensions, m_playerPos.z - cullingDimensions));
            // DirectX::XMVECTOR planePoint3 = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_playerPos.x - cullingDimensions, m_playerPos.y +
            // cullingDimensions, m_playerPos.z - cullingDimensions));
            // planePoint1 = planePoint1 - planePoint3;
            // planePoint2 = planePoint2 - planePoint3;
            //// normal vector of the plane
            // planePoint1 = DirectX::XMVector3Cross(planePoint2, planePoint1);
            // DirectX::XMFLOAT3 planeData;
            // DirectX::XMStoreFloat3(&planeData, planePoint1);
            // m_planes[0].x = planeData.x;
            // m_planes[0].y = planeData.y;
            // m_planes[0].z = planeData.z;
            // m_planes[0].w = planeData.x * (m_playerPos.x - cullingDimensions) + planeData.y * (m_playerPos.y - cullingDimensions) + planeData.z *
            // (m_playerPos.z - cullingDimensions);

            // ////////DEBUGS
            ////////////////std::cout << "X:" << m_planes[0].x << " Y:" << m_planes[0].y << " Z:" << m_planes[0].z << " W:" << m_planes[0].w <<
            /// std::endl;
            //////////////Far plane
            // planePoint1 = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_playerPos.x - cullingDimensions, m_playerPos.y - cullingDimensions,
            // m_playerPos.z + cullingDimensions));
            // planePoint2 = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_playerPos.x + cullingDimensions, m_playerPos.y - cullingDimensions,
            // m_playerPos.z + cullingDimensions));
            // planePoint3 = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_playerPos.x - cullingDimensions, m_playerPos.y + cullingDimensions,
            // m_playerPos.z + cullingDimensions));
            // planePoint1 = planePoint1 - planePoint3;
            // planePoint2 = planePoint2 - planePoint3;
            //// normal vector of the plane
            // planePoint1 = DirectX::XMVector3Cross(planePoint1, planePoint2);
            // DirectX::XMStoreFloat3(&planeData, planePoint1);
            // m_planes[1].x = planeData.x;
            // m_planes[1].y = planeData.y;
            // m_planes[1].z = planeData.z;
            // m_planes[1].w = planeData.x * (m_playerPos.x - cullingDimensions) + planeData.y * (m_playerPos.y - cullingDimensions) + planeData.z *
            // (m_playerPos.z + cullingDimensions);

            ////left plane
            // planePoint1 = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_playerPos.x - cullingDimensions, m_playerPos.y - cullingDimensions,
            // m_playerPos.z - cullingDimensions));
            // planePoint2 = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_playerPos.x - cullingDimensions, m_playerPos.y - cullingDimensions,
            // m_playerPos.z + cullingDimensions));
            // planePoint3 = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_playerPos.x - cullingDimensions, m_playerPos.y + cullingDimensions,
            // m_playerPos.z - cullingDimensions));
            // planePoint1 = planePoint1 - planePoint3;
            // planePoint2 = planePoint2 - planePoint3;
            //// normal vector of the plane
            // planePoint1 = DirectX::XMVector3Cross(planePoint1, planePoint2);
            // DirectX::XMStoreFloat3(&planeData, planePoint1);
            // m_planes[2].x = planeData.x;
            // m_planes[2].y = planeData.y;
            // m_planes[2].z = planeData.z;
            // m_planes[2].w = planeData.x * (m_playerPos.x - cullingDimensions) + planeData.y * (m_playerPos.y - cullingDimensions) + planeData.z *
            // (m_playerPos.z - cullingDimensions);

            ////right plane
            // planePoint1 = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_playerPos.x + cullingDimensions, m_playerPos.y - cullingDimensions,
            // m_playerPos.z - cullingDimensions));
            // planePoint2 = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_playerPos.x + cullingDimensions, m_playerPos.y - cullingDimensions,
            // m_playerPos.z + cullingDimensions));
            // planePoint3 = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_playerPos.x + cullingDimensions, m_playerPos.y + cullingDimensions,
            // m_playerPos.z - cullingDimensions));
            // planePoint1 = planePoint1 - planePoint3;
            // planePoint2 = planePoint2 - planePoint3;
            //// normal vector of the plane
            // planePoint1 = DirectX::XMVector3Cross(planePoint2, planePoint1);
            // DirectX::XMStoreFloat3(&planeData, planePoint1);
            // m_planes[3].x = planeData.x;
            // m_planes[3].y = planeData.y;
            // m_planes[3].z = planeData.z;
            // m_planes[3].w = planeData.x * (m_playerPos.x + cullingDimensions) + planeData.y * (m_playerPos.y - cullingDimensions) + planeData.z *
            // (m_playerPos.z - cullingDimensions);

            ////above plane
            // planePoint1 = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_playerPos.x - cullingDimensions, m_playerPos.y + cullingDimensions,
            // m_playerPos.z - cullingDimensions));
            // planePoint2 = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_playerPos.x - cullingDimensions, m_playerPos.y + cullingDimensions,
            // m_playerPos.z + cullingDimensions));
            // planePoint3 = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_playerPos.x + cullingDimensions, m_playerPos.y + cullingDimensions,
            // m_playerPos.z + cullingDimensions));
            // planePoint1 = planePoint1 - planePoint3;
            // planePoint2 = planePoint2 - planePoint3;
            //// normal vector of the plane
            // planePoint1 = DirectX::XMVector3Cross(planePoint1, planePoint2);
            // DirectX::XMStoreFloat3(&planeData, planePoint1);
            // m_planes[4].x = planeData.x;
            // m_planes[4].y = planeData.y;
            // m_planes[4].z = planeData.z;
            // m_planes[4].w = planeData.x * (m_playerPos.x - cullingDimensions) + planeData.y * (m_playerPos.y + cullingDimensions) + planeData.z *
            // (m_playerPos.z - cullingDimensions);

            ////lower plane
            // planePoint1 = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_playerPos.x - cullingDimensions, m_playerPos.y - cullingDimensions,
            // m_playerPos.z - cullingDimensions));
            // planePoint2 = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_playerPos.x - cullingDimensions, m_playerPos.y - cullingDimensions,
            // m_playerPos.z + cullingDimensions));
            // planePoint3 = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_playerPos.x + cullingDimensions, m_playerPos.y - cullingDimensions,
            // m_playerPos.z + cullingDimensions));
            // planePoint1 = planePoint1 - planePoint3;
            // planePoint2 = planePoint2 - planePoint3;
            //// normal vector of the plane
            // planePoint1 = DirectX::XMVector3Cross(planePoint2, planePoint1);
            // DirectX::XMStoreFloat3(&planeData, planePoint1);
            // m_planes[5].x = planeData.x;
            // m_planes[5].y = planeData.y;
            // m_planes[5].z = planeData.z;
            // m_planes[5].w = planeData.x * (m_playerPos.x - cullingDimensions) + planeData.y * (m_playerPos.y - cullingDimensions) + planeData.z *
            // (m_playerPos.z - cullingDimensions);
            ////
            //////// From tutorial!
            ////// Calculate near plane of frustum.
            ////m_planes[0].x = t_viewAndProjection4x4._14 + t_viewAndProjection4x4._13;
            ////m_planes[0].y = t_viewAndProjection4x4._24 + t_viewAndProjection4x4._23;
            ////m_planes[0].z = t_viewAndProjection4x4._34 + t_viewAndProjection4x4._33;
            // m_planes[0].w = t_viewAndProjection4x4._44 + t_viewAndProjection4x4._43;
            // Trying new near plane!!
            // m_planes[0].x = t_viewAndProjection4x4._13;
            // m_planes[0].y = t_viewAndProjection4x4._23;
            // m_planes[0].z = t_viewAndProjection4x4._33;
            // m_planes[0].w = t_viewAndProjection4x4._43;
            // DirectX::XMStoreFloat4(&m_planes[0], DirectX::XMVector4Normalize(DirectX::XMLoadFloat4(&m_planes[0])));

            //////// Calculate far plane of frustum.
            // m_planes[1].x = t_viewAndProjection4x4._14 - t_viewAndProjection4x4._13;
            // m_planes[1].y = t_viewAndProjection4x4._24 - t_viewAndProjection4x4._23;
            // m_planes[1].z = t_viewAndProjection4x4._34 - t_viewAndProjection4x4._33;
            // m_planes[1].w = t_viewAndProjection4x4._44 - t_viewAndProjection4x4._43;
            // DirectX::XMStoreFloat4(&m_planes[1], DirectX::XMVector4Normalize(DirectX::XMLoadFloat4(&m_planes[1])));

            //// Calculate left plane of frustum.
            // m_planes[2].x = t_viewAndProjection4x4._14 + t_viewAndProjection4x4._11;
            // m_planes[2].y = t_viewAndProjection4x4._24 + t_viewAndProjection4x4._21;
            // m_planes[2].z = t_viewAndProjection4x4._34 + t_viewAndProjection4x4._31;
            // m_planes[2].w = t_viewAndProjection4x4._44 + t_viewAndProjection4x4._41;
            // DirectX::XMStoreFloat4(&m_planes[2], DirectX::XMVector4Normalize(DirectX::XMLoadFloat4(&m_planes[2])));

            //// Calculate right plane of frustum.
            // m_planes[3].x = t_viewAndProjection4x4._14 - t_viewAndProjection4x4._11;
            // m_planes[3].y = t_viewAndProjection4x4._24 - t_viewAndProjection4x4._21;
            // m_planes[3].z = t_viewAndProjection4x4._34 - t_viewAndProjection4x4._31;
            // m_planes[3].w = t_viewAndProjection4x4._44 - t_viewAndProjection4x4._41;
            // DirectX::XMStoreFloat4(&m_planes[3], DirectX::XMVector4Normalize(DirectX::XMLoadFloat4(&m_planes[3])));

            //// Calculate top plane of frustum.
            // m_planes[4].x = t_viewAndProjection4x4._14 - t_viewAndProjection4x4._12;
            // m_planes[4].y = t_viewAndProjection4x4._24 - t_viewAndProjection4x4._22;
            // m_planes[4].z = t_viewAndProjection4x4._34 - t_viewAndProjection4x4._32;
            // m_planes[4].w = t_viewAndProjection4x4._44 - t_viewAndProjection4x4._42;
            // DirectX::XMStoreFloat4(&m_planes[4], DirectX::XMVector4Normalize(DirectX::XMLoadFloat4(&m_planes[4])));

            //// Calculate bottom plane of frustum.
            // m_planes[5].x = t_viewAndProjection4x4._14 + t_viewAndProjection4x4._12;
            // m_planes[5].y = t_viewAndProjection4x4._24 + t_viewAndProjection4x4._22;
            // m_planes[5].z = t_viewAndProjection4x4._34 + t_viewAndProjection4x4._32;
            // m_planes[5].w = t_viewAndProjection4x4._44 + t_viewAndProjection4x4._42;
            // DirectX::XMStoreFloat4(&m_planes[5], DirectX::XMVector4Normalize(DirectX::XMLoadFloat4(&m_planes[5])));
            int hejsan = 0;
            int elseSatsen = 0;
            int ifSatsen = 0;
            NAMED_TIMER("TreeHandlerAfterBuildingTheFrustum")
            while(!t_isDone)
            {
                // if(!t_currentNode->empty)
                //{

                //////////////// COLLISION test
                //////// Loading the float to a vector so we can manipulate it
                //////DirectX::XMVECTOR t_halfDimensions = DirectX::XMLoadFloat3(&t_currentNode->boxDimensions);


                //////// Dividing it by two so we can use it as half extents as needed in the physicsfunction
                //////DirectX::XMStoreFloat3(&physicsCollideFloat, t_halfDimensions * 0.5f);
                //////t_sweepHits = m_sharedContext.GetPhysicsModule().GetRayCastManager().OverlapBoxMultipleHits(t_currentNode->center,
                /// physicsCollideFloat);
                //////// For the loop
                //////numberOfHits = t_sweepHits.size();
                /////////////////

                // Collision with the frustum planes.
                if(CollisionCheckForBox(t_currentNode->center, t_currentNode->boxDimensions) || t_currentNode->depth == 0)
                {
                    NAMED_TIMER("IfSatsen")
                    // If max depth isnt reached , minus one is needed to get the depth wanted
                    if(/*t_currentNode->depth < m_treeCreator->m_treeDepth - 1 &&*/ t_currentNode->leaf == false)
                    {
                        // Max depth wasn't reached

                        // Where to start next, if we reached t_maxdepth last loop we have to
                        if(t_currentNode->loopInfo <= 7)
                        {
                            // Adding the value before and subtracting it in the array because we want to keep track of it before we change
                            // pointer


                            // bool emptyCheck = true;
                            // while (emptyCheck)
                            //{
                            //    ++t_currentNode->loopInfo;
                            //    if (t_currentNode->loopInfo > 8)
                            //    {
                            //        t_currentNode->loopInfo = 0;
                            //        t_currentNode = t_currentNode->parent;
                            //        emptyCheck = false;
                            //    }
                            //    else if (!t_currentNode->children[t_currentNode->loopInfo - 1]->empty)
                            //    {
                            //        t_currentNode = t_currentNode->children[t_currentNode->loopInfo - 1];
                            //        emptyCheck = false;
                            //    }
                            //}
                            ++t_currentNode->loopInfo;
                            if(!t_currentNode->children[t_currentNode->loopInfo - 1]->empty)
                            {
                                t_currentNode = t_currentNode->children[t_currentNode->loopInfo - 1];
                            }
                            else
                            {
                                // Do nothing, we will do the loop again without going into the child and going to the next since we added
                                // loopinfo.
                            }
                        }
                        else
                        {
                            if(t_currentNode->depth == 0)
                            {
                                // Is done, we went through all children at the depth of zero which means we are at the root node
                                t_currentNode->loopInfo = 0;
                                t_isDone = true;
                            }
                            else
                            {
                                // Loop info is above 7 which means we have looped through all children and need to take a step back in the tree
                                t_currentNode->loopInfo = 0;
                                t_currentNode = t_currentNode->parent;
                            }
                        }
                    }

                    else
                    {
                        // Max depth reached, these will be the interesting objects to draw.

                        size_t loopSize = t_currentNode->objectsInTheArea.size();
                        // for (auto i: t_currentNode->objectsInTheArea)
                        //{
                        //    if(std::find(m_objectsToDraw.begin(), m_objectsToDraw.end(), t_currentNode->objectsInTheArea[i]) !=
                        //    m_objectsToDraw.end())
                        //    {
                        //           // Nothing
                        //    }
                        //    else
                        //    {
                        //        m_objectsToDraw.push_back(t_currentNode->objectsInTheArea[i]);
                        //    }
                        //}
                        for(size_t i = 0; i < loopSize; ++i)
                        {
                            NAMED_TIMER("TreeHandlerPushingTHingsIntoListForLoop")
                            // TODOEA Have to do a check if the object allready is in the list
                            if(std::find(m_objectsToDraw.begin(), m_objectsToDraw.end(), t_currentNode->objectsInTheArea[i]) != m_objectsToDraw.end())
                            {
                                // Nothing
                                ++hejsan;
                            }
                            else
                            {
                                m_objectsToDraw.push_back(t_currentNode->objectsInTheArea[i]);
                            }
                        }
                        t_currentNode->loopInfo = 0;
                        t_currentNode = t_currentNode->parent;
                    }
                }
                else
                { // If max depth isnt reached , minus one is needed to get the depth wanted
                    NAMED_TIMER("ElseSatsen")
                    if(/*t_currentNode->depth < m_treeCreator->m_treeDepth - 1 &&*/ t_currentNode->leaf == false)
                    {
                        // no collision with frustum
                        // Where to start next, if we reached t_maxdepth last loop we have to
                        if(t_currentNode->loopInfo <= 7)
                        {
                            // Adding the value before and subtracting it in the array because we want to keep track of it before we change
                            // pointer
                            ++t_currentNode->loopInfo;
                            if(!t_currentNode->children[t_currentNode->loopInfo - 1]->empty)
                            {
                                t_currentNode = t_currentNode->children[t_currentNode->loopInfo - 1];
                            }
                            else
                            {
                                // Do nothing, we will do the loop again without going into the child and going to the next since we added
                                // loopinfo.
                            }
                        }
                        else
                        {
                            if(t_currentNode->depth == 0)
                            {
                                // Is done, we went through all children at the depth of zero which means we are at the root node
                                t_currentNode->loopInfo = 0;
                                t_isDone = true;
                            }
                            else
                            {
                                // Loop info is above 7 which means we have looped through all children and need to take a step back in the tree
                                t_currentNode->loopInfo = 0;
                                t_currentNode = t_currentNode->parent;
                            }
                        }
                    }
                    else
                    {
                        // Max depth reached, these will be the interesting objects to draw.

                        t_currentNode->loopInfo = 0;
                        t_currentNode = t_currentNode->parent;
                    }
                }
                //}
                // else
                // {
                //     // if is empty we back out to the parent
                //     t_currentNode->loopInfo = 0;
                //     t_currentNode = t_currentNode->parent;
                // }
            }
            // std::cout << elseSatsen << "=ElseSatsen. " << std::endl;
            // std::cout << ifSatsen << "=IfSatsen. " << std::endl;
            return m_objectsToDraw;
        }
        void TreeHandler::OnEvent(Event* p_event) {}
        void TreeHandler::ResetObjectsToDraw() { m_objectsToDraw.clear(); }
    }
}