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
#include <iostream>

namespace Doremi
{
    namespace Core
    {
        TreeHandler* TreeHandler::m_singleton = nullptr;

        TreeHandler::TreeHandler(const DoremiEngine::Core::SharedContext& p_sharedContext) : m_sharedContext(p_sharedContext)
        {
            m_treeCreator = new TreeCreator(p_sharedContext);
            m_viewDist = m_sharedContext.GetConfigurationModule().GetAllConfigurationValues().CameraViewDistance;
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
            // Check if any one point of the cube is in the view frustum.
            for (int i = 0; i < 6; ++i)
            {
                DirectX::XMFLOAT3 t_collideCheckFloats;
                DirectX::XMStoreFloat3(&t_collideCheckFloats, DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_planes[i].x, m_planes[i].y, m_planes[i].z)), DirectX::XMLoadFloat3(&DirectX::XMFLOAT3((p_center.x - p_dimensions.x), (p_center.y - p_dimensions.y), (p_center.z - p_dimensions.z)))));// +m_planes[i].w;)
                if (t_collideCheckFloats.x + m_planes[i].w >= 0.0f)
                {
                    continue;
                }
                DirectX::XMStoreFloat3(&t_collideCheckFloats, DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_planes[i].x, m_planes[i].y, m_planes[i].z)), DirectX::XMLoadFloat3(&DirectX::XMFLOAT3((p_center.x + p_dimensions.x), (p_center.y - p_dimensions.y), (p_center.z - p_dimensions.z)))));// +m_planes[i].w;)
                if (t_collideCheckFloats.x + m_planes[i].w >= 0.0f)
                {
                    continue;
                }
                DirectX::XMStoreFloat3(&t_collideCheckFloats, DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_planes[i].x, m_planes[i].y, m_planes[i].z)), DirectX::XMLoadFloat3(&DirectX::XMFLOAT3((p_center.x - p_dimensions.x), (p_center.y + p_dimensions.y), (p_center.z - p_dimensions.z)))));// +m_planes[i].w;)
                if (t_collideCheckFloats.x + m_planes[i].w >= 0.0f)
                {
                    continue;
                }
                DirectX::XMStoreFloat3(&t_collideCheckFloats, DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_planes[i].x, m_planes[i].y, m_planes[i].z)), DirectX::XMLoadFloat3(&DirectX::XMFLOAT3((p_center.x + p_dimensions.x), (p_center.y + p_dimensions.y), (p_center.z - p_dimensions.z)))));// +m_planes[i].w;)
                if (t_collideCheckFloats.x + m_planes[i].w >= 0.0f)
                {
                    continue;
                }
                DirectX::XMStoreFloat3(&t_collideCheckFloats, DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_planes[i].x, m_planes[i].y, m_planes[i].z)), DirectX::XMLoadFloat3(&DirectX::XMFLOAT3((p_center.x - p_dimensions.x), (p_center.y - p_dimensions.y), (p_center.z + p_dimensions.z)))));// +m_planes[i].w;)
                if (t_collideCheckFloats.x + m_planes[i].w >= 0.0f)
                {
                    continue;
                }
                DirectX::XMStoreFloat3(&t_collideCheckFloats, DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_planes[i].x, m_planes[i].y, m_planes[i].z)), DirectX::XMLoadFloat3(&DirectX::XMFLOAT3((p_center.x + p_dimensions.x), (p_center.y - p_dimensions.y), (p_center.z + p_dimensions.z)))));// +m_planes[i].w;)
                if (t_collideCheckFloats.x + m_planes[i].w >= 0.0f)
                {
                    continue;
                }
                DirectX::XMStoreFloat3(&t_collideCheckFloats, DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_planes[i].x, m_planes[i].y, m_planes[i].z)), DirectX::XMLoadFloat3(&DirectX::XMFLOAT3((p_center.x - p_dimensions.x), (p_center.y + p_dimensions.y), (p_center.z + p_dimensions.z)))));// +m_planes[i].w;)
                if (t_collideCheckFloats.x + m_planes[i].w >= 0.0f)
                {
                    continue;
                }
                DirectX::XMStoreFloat3(&t_collideCheckFloats, DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_planes[i].x, m_planes[i].y, m_planes[i].z)), DirectX::XMLoadFloat3(&DirectX::XMFLOAT3((p_center.x + p_dimensions.x), (p_center.y + p_dimensions.y), (p_center.z + p_dimensions.z)))));// +m_planes[i].w;)
                if (t_collideCheckFloats.x + m_planes[i].w >= 0.0f)
                {
                    continue;
                }

                return false;
            }
            return true;
        }
        std::vector<uint32_t> TreeHandler::Update()
        {
            bool t_isDone = false;
            int t_whatChild = 0;
            TreeCreator::OctNode* t_currentNode = &m_treeCreator->treeRoot;

            ////////// COLLISION test
            std::vector<int> t_sweepHits;
            size_t numberOfHits = 0;
            DirectX::XMFLOAT3 physicsCollideFloat;
            //////////

            // Getting the camera
            const DoremiEngine::Graphic::Camera& t_playerCamera = CameraHandler::GetInstance()->GetThirdPersonCamera();
            // Getting the matrices so we can build our frustum
            const DoremiEngine::Graphic::CameraMatrices t_camMatrices = t_playerCamera.GetCameraMatrices();
            // Needed variables
            DirectX::XMFLOAT4X4 t_projection4x4 = t_camMatrices.ProjectionMatrix;
            // DirectX::XMFLOAT4X4 t_view4x4 = t_camMatrices.ViewMatrix;
            float t_zMin;
            float r;
            DirectX::XMMATRIX t_viewAndProjectionMatrix;
            t_zMin = t_projection4x4._43 / t_projection4x4._33;
            r = m_viewDist / (m_viewDist - t_zMin);
            t_projection4x4._33 = r;
            t_projection4x4._43 = -r * t_zMin;
            t_viewAndProjectionMatrix = DirectX::XMMatrixMultiply(XMLoadFloat4x4(&t_projection4x4), XMLoadFloat4x4(&t_camMatrices.ViewMatrix));
            DirectX::XMFLOAT4X4 t_viewAndProjection4x4;
            DirectX::XMStoreFloat4x4(&t_viewAndProjection4x4, t_viewAndProjectionMatrix);

            // Calculate near plane of frustum.
            m_planes[0].x = t_viewAndProjection4x4._14 + t_viewAndProjection4x4._13;
            m_planes[0].y = t_viewAndProjection4x4._24 + t_viewAndProjection4x4._23;
            m_planes[0].z = t_viewAndProjection4x4._34 + t_viewAndProjection4x4._33;
            m_planes[0].w = t_viewAndProjection4x4._44 + t_viewAndProjection4x4._43;
            DirectX::XMStoreFloat4(&m_planes[0], DirectX::XMVector4Normalize(DirectX::XMLoadFloat4(&m_planes[0])));

            // Calculate far plane of frustum.
            m_planes[1].x = t_viewAndProjection4x4._14 - t_viewAndProjection4x4._13;
            m_planes[1].y = t_viewAndProjection4x4._24 - t_viewAndProjection4x4._23;
            m_planes[1].z = t_viewAndProjection4x4._34 - t_viewAndProjection4x4._33;
            m_planes[1].w = t_viewAndProjection4x4._44 - t_viewAndProjection4x4._43;
            DirectX::XMStoreFloat4(&m_planes[1], DirectX::XMVector4Normalize(DirectX::XMLoadFloat4(&m_planes[1])));

            // Calculate left plane of frustum.
            m_planes[2].x = t_viewAndProjection4x4._14 + t_viewAndProjection4x4._11;
            m_planes[2].y = t_viewAndProjection4x4._24 + t_viewAndProjection4x4._21;
            m_planes[2].z = t_viewAndProjection4x4._34 + t_viewAndProjection4x4._31;
            m_planes[2].w = t_viewAndProjection4x4._44 + t_viewAndProjection4x4._41;
            DirectX::XMStoreFloat4(&m_planes[2], DirectX::XMVector4Normalize(DirectX::XMLoadFloat4(&m_planes[2])));

            // Calculate right plane of frustum.
            m_planes[3].x = t_viewAndProjection4x4._14 - t_viewAndProjection4x4._11;
            m_planes[3].y = t_viewAndProjection4x4._24 - t_viewAndProjection4x4._21;
            m_planes[3].z = t_viewAndProjection4x4._34 - t_viewAndProjection4x4._31;
            m_planes[3].w = t_viewAndProjection4x4._44 - t_viewAndProjection4x4._41;
            DirectX::XMStoreFloat4(&m_planes[3], DirectX::XMVector4Normalize(DirectX::XMLoadFloat4(&m_planes[3])));

            // Calculate top plane of frustum.
            m_planes[4].x = t_viewAndProjection4x4._14 - t_viewAndProjection4x4._12;
            m_planes[4].y = t_viewAndProjection4x4._24 - t_viewAndProjection4x4._22;
            m_planes[4].z = t_viewAndProjection4x4._34 - t_viewAndProjection4x4._32;
            m_planes[4].w = t_viewAndProjection4x4._44 - t_viewAndProjection4x4._42;
            DirectX::XMStoreFloat4(&m_planes[4], DirectX::XMVector4Normalize(DirectX::XMLoadFloat4(&m_planes[4])));

            // Calculate bottom plane of frustum.
            m_planes[5].x = t_viewAndProjection4x4._14 + t_viewAndProjection4x4._12;
            m_planes[5].y = t_viewAndProjection4x4._24 + t_viewAndProjection4x4._22;
            m_planes[5].z = t_viewAndProjection4x4._34 + t_viewAndProjection4x4._32;
            m_planes[5].w = t_viewAndProjection4x4._44 + t_viewAndProjection4x4._42;
            DirectX::XMStoreFloat4(&m_planes[5], DirectX::XMVector4Normalize(DirectX::XMLoadFloat4(&m_planes[5])));


            while(!t_isDone)
            {
                if(!t_currentNode->empty)
                {

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
                    if(CollisionCheckForBox(t_currentNode->center, t_currentNode->boxDimensions))
                    {
                        // If max depth isnt reached , minus one is needed to get the depth wanted
                        if(t_currentNode->depth < m_treeCreator->m_treeDepth - 1)
                        {
                            // Max depth wasn't reached

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
                                    t_isDone = true;
                                }
                                else
                                {
                                    // Loop info is above 7 which means we have looped through all children and need to take a step back in the tree
                                    t_currentNode = t_currentNode->parent;
                                }
                            }
                        }

                        else
                        {
                            // Max depth reached, these will be the interesting objects to draw.

                            size_t loopSize = t_currentNode->objectsInTheArea.size();
                            for(size_t i = 0; i < loopSize; ++i)
                            {
                                // TODOEA Have to do a check if the object allready is in the list
                                m_objectsToDraw.push_back(t_currentNode->objectsInTheArea[i]);
                            }
                            t_currentNode = t_currentNode->parent;
                        }
                    }
                }
                else
                {
                    // if is empty we back out to the parent
                    t_currentNode = t_currentNode->parent;
                }
            }
            return m_objectsToDraw;
        }
        void TreeHandler::OnEvent(Event* p_event) {}
    }
}