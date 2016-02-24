#include <Doremi/Core/Include/Handler/TreeHandler.hpp>
#include <DoremiEngine/Physics/Include/RigidBodyManager.hpp>
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/PhysicsMaterialManager.hpp>
#include <DoremiEngine/Physics/Include/RayCastManager.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <Doremi/Core/Include/CameraHandler.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Camera/Camera.hpp>
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

        void TreeHandler::TrimTheTree()
        {
            bool t_isDone = false;
            int t_whatChild = 0;
            TreeCreator::OctNode* t_currentNode = &m_treeCreator->treeRoot;

            ////////// COLLISION test
            std::vector<int> t_sweepHits;
            size_t numberOfHits = 0;
            DirectX::XMFLOAT3 physicsCollideFloat;
            //////////

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
                    const DoremiEngine::Graphic::Camera& t_playerCamera = CameraHandler::GetInstance()->GetThirdPersonCamera();
                    const DoremiEngine::Graphic::CameraMatrices t_camMatrices = t_playerCamera.GetCameraMatrices();

                    // Collision with the frustum planes.
                    if(true)
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
                                    // Is done
                                    t_isDone = true;
                                }
                                else
                                {
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
        }

        void TreeHandler::Update()
        {
            // m_treeCreator->treeRoot
            // TreeCreator::OctNode
        }
        void TreeHandler::OnEvent(Event* p_event) {}
    }
}