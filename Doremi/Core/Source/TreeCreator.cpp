#include <Doremi/Core/Include/TreeCreator.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <DoremiEngine/Physics/Include/RigidBodyManager.hpp>
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/PhysicsMaterialManager.hpp>
#include <DoremiEngine/Physics/Include/RayCastManager.hpp>

namespace Doremi
{
    namespace Core
    {
        TreeCreator::TreeCreator(const DoremiEngine::Core::SharedContext& p_sharedContext) : m_sharedContext(p_sharedContext)
        {
            // m_sharedContext = p_sharedContext;

            // Set the depth of the oct tree
            m_treeDepth = 3;

            // Set the box for the world
            treeRoot.boxDimensions = DirectX::XMFLOAT3(2900, 2150, 7000); // TODOEA Borde läsas in från någonting TODOCONFIG kanske
            // treeRoot.boxDimensions = DirectX::XMFLOAT3(100, 50, 100); 2900
            //
            // treeRoot.boxDimensions = DirectX::XMFLOAT3(7000, 2150, 7000);

            // Set the center for the box of the world
            treeRoot.center = DirectX::XMFLOAT3(-485, 158, 2041);
            // treeRoot.center = DirectX::XMFLOAT3(0, 0, 0); // TODOEA Borde läsas in från någonting. TODOCONFIG kanske // treeRoot.center =
            // DirectX::XMFLOAT3(-485, 158, 2041)
            m_allreadyBuilt = false;
            // Start up for the oct tree. Without this function we don't have anything to start with
            // CreateAndDivideTheChildren(treeRoot);
        };
        TreeCreator::~TreeCreator() {}

        void TreeCreator::CreateTree()
        {
            // Start building the tree
            BuildTree();
        }

        void TreeCreator::BuildTree()
        {
            int hejsan = 0;
            if(m_allreadyBuilt)
            {
                return;
            }
            bool t_isDone = false;
            //bool t_maxDepthReached = false;
            int t_whatChild = 0;

            // Hör till debuggen i while satsen.
            int materialID = m_sharedContext.GetPhysicsModule().GetPhysicsMaterialManager().CreateMaterial(0, 0, 0);
            m_currentNode = &treeRoot;
            DirectX::XMFLOAT3 physicsCollideFloat;

            while(!t_isDone)
            {
                ////// DEBUG!
                //if (m_currentNode->depth == m_treeDepth -1)
                //{
                //  int myID = MAX_NUM_ENTITIES;
                //  // m_sharedContext.GetPhysicsModule().GetRigidBodyManager().AddBoxBodyStatic(myID, m_currentNode->center, DirectX::XMFLOAT4(0, 0, 0, 1),
                //  // m_currentNode->boxDimensions, materialID);
                //  // m_sharedContext.GetPhysicsModule().GetRigidBodyManager().SetTrigger(myID, true);
                //   // int myID = MAX_NUM_ENTITIES;
                //    m_sharedContext.GetPhysicsModule().GetRigidBodyManager().AddBoxBodyStatic(myID, m_currentNode->center, DirectX::XMFLOAT4(0, 0, 0, 1),
                //        DirectX::XMFLOAT3(m_currentNode->boxDimensions.x * 0.5f, m_currentNode->boxDimensions.y * 0.5f, m_currentNode->boxDimensions.z * 0.5f), materialID);
                //    
                //    m_sharedContext.GetPhysicsModule().GetRigidBodyManager().SetTrigger(myID, true);
                //}
                 // DEBUG SLUT

                // Kan optimera med detta senare.
                // m_sharedContext.GetPhysicsModule().GetRigidBodyManager().SetCallbackFiltering(triggerid, 0, );

                std::vector<int> t_sweepHits;
                size_t numberOfHits = 0;
                if (m_currentNode->loopInfo == 0)
                {
                    // Loading the float to a vector so we can manipulate it
                    XMVECTOR t_halfDimensions = XMLoadFloat3(&m_currentNode->boxDimensions);


                    // Dividing it by two so we can use it as half extents as needed in the physicsfunction
                    DirectX::XMStoreFloat3(&physicsCollideFloat, t_halfDimensions * 0.5f);
                    t_sweepHits = m_sharedContext.GetPhysicsModule().GetRayCastManager().OverlapBoxMultipleHits(m_currentNode->center, physicsCollideFloat);
                    // For the loop
                    numberOfHits = t_sweepHits.size();
                }

                // if there is a collision between the child box and any object that needs drawing
                if (numberOfHits > 0 || m_currentNode->empty == false)
                {
                    m_currentNode->empty = false;

                    // If max depth isnt reached , minus one is needed to get the depth wanted
                    if(numberOfHits != 1 && m_currentNode->depth < m_treeDepth - 1)
                    {
                        // Collision and max depth wasn't reached

                        // If we haven't created the children, we do so
                        if(m_currentNode->loopInfo == 0)
                        {
                            CreateAndDivideTheChildren(*m_currentNode);
                        }

                        // Add the things we collided with to the list.
                        // Where to start next, if we reached t_maxdepth last loop we have to
                        if (m_currentNode->loopInfo <= 7)
                        {
                            // Adding the value before and subtracting it in the array because we want to keep track of it before we change pointer
                            ++m_currentNode->loopInfo;
                            m_currentNode = m_currentNode->children[m_currentNode->loopInfo - 1];
                        }
                        else
                        {
                            if (m_currentNode->depth == 0)
                            {
                                // Is done
                                m_currentNode->loopInfo = 0;                                
                                //int myID = MAX_NUM_ENTITIES;
                                //m_sharedContext.GetPhysicsModule().GetRigidBodyManager().AddBoxBodyStatic(myID, m_currentNode->center, DirectX::XMFLOAT4(0, 0, 0, 1),
                                //    DirectX::XMFLOAT3(m_currentNode->boxDimensions.x * 0.5f, m_currentNode->boxDimensions.y * 0.5f, m_currentNode->boxDimensions.z * 0.5f), materialID);

                                //m_sharedContext.GetPhysicsModule().GetRigidBodyManager().SetTrigger(myID, true);
                                t_isDone = true;
                            }
                            else
                            {
                                // Maybe remove everyone in the nodelist of objects.
                                m_currentNode->loopInfo = 0;
                                m_currentNode = m_currentNode->parent;
                            }
                        }
                    }

                    else
                    {
                        // Collision but we reached max depth
                        for(size_t i = 0; i < numberOfHits; ++i)
                        {
                            // Check if entity will be affected by the render.
                            // Placing all the render components into the leaf that collided with the box.
                            if(EntityHandler::GetInstance().HasComponents(t_sweepHits[i], (int)ComponentType::Render) &&
                               !EntityHandler::GetInstance().HasComponents(t_sweepHits[i], (int)ComponentType::FrequencyAffected))
                            {
                                m_currentNode->objectsInTheArea.push_back(t_sweepHits[i]);
                            }
                        }
                        ++hejsan;
                        m_currentNode->loopInfo = 0;
                        m_currentNode->leaf = true;
                        m_currentNode = m_currentNode->parent;
                    }
                }
                else
                {
                    // No collision
                    // Maybe not needed.
                    // if (m_currentNode->depth == 0)
                    // {
                    //     m_currentNode->loopInfo = 0;
                    //     t_isDone = true;
                    // }
                    if(m_currentNode->depth < m_treeDepth - 1)
                    {
                        m_currentNode->empty = true;
                        m_currentNode->leaf = true;
                        m_currentNode->loopInfo = 0;
                        m_currentNode = m_currentNode->parent;
                    }
                    else
                    {
                        ++hejsan;
                        m_currentNode->leaf = true;
                        m_currentNode->empty = true;
                        m_currentNode->loopInfo = 0;
                        m_currentNode = m_currentNode->parent;
                    }
                }
            }
            m_allreadyBuilt = true;

            //// Looping through the
            // for(size_t i = 0; i < 8; i++)
            //{
            //    // if there is a collision between the child box and any object that needs drawing
            //    if(true) // TODOEA Here should be the collision check between all the objects in the objectsInTheArea list
            //    {
            //        // Since we had a collision, there is something in this box
            //        o_treeNode.empty = false;

            //        // If max depth isnt reached , minus two is needed to get the depth wanted.
            //        if(o_treeNode.depth < m_treeDepth - 1)
            //        {
            //            // Collision and max depth wasn't reached

            //            // Prepare the next BuildTree() function with 8 newly created children
            //            CreateAndDivideTheChildren(*o_treeNode.children[i]);

            //            // We start over in the next node to see if we can split it even further
            //            BuildTree(*o_treeNode.children[i]);
            //        }
            //        else if(o_treeNode.depth == m_treeDepth - 1)
            //        {
            //            // int hejsan = 0;
            //            // Collision but we reached max depth
            //            // Check where the objects will be placed in this nodes children
            //        }
            //        else
            //        {
            //            // Collision but we're beyond max depth
            //        }
            //    }
            //    else
            //    {
            //        // No collision
            //        o_treeNode.empty = true;
            //    }
            //}
        }
        void TreeCreator::CreateAndDivideTheChildren(OctNode& o_treeNode)
        {
            // Using DirectXNamespace because I couldnt multiply two vectors with eafchother without it
            using namespace DirectX;


            /*
            Make the dimensions available for usage and make split it in half
            Splitting them in half so they can be used for the childrens dimensions
            */
            XMVECTOR t_halfDimensions = XMLoadFloat3(&o_treeNode.boxDimensions);
            t_halfDimensions = t_halfDimensions * 0.5f;

            // Same with the center, making it availabe for manipulation
            XMVECTOR t_startPosForThisOct = XMLoadFloat3(&o_treeNode.center);
            t_startPosForThisOct = t_startPosForThisOct - t_halfDimensions;

            // to use for getting the new center we need a xmfloat
            XMFLOAT3 t_startPosForThisOctFLOAT;
            DirectX::XMStoreFloat3(&t_startPosForThisOctFLOAT, t_startPosForThisOct);

            // Store the vecotr to a xmfloat3 for later use
            XMFLOAT3 t_halfDimensionsXMFLOAT;
            DirectX::XMStoreFloat3(&t_halfDimensionsXMFLOAT, t_halfDimensions);
            XMFLOAT3 t_fourthOfDimensionsXMFLOAT;
            DirectX::XMStoreFloat3(&t_fourthOfDimensionsXMFLOAT, t_halfDimensions * 0.5f);

            DirectX::XMStoreFloat3(&t_startPosForThisOctFLOAT, XMLoadFloat3(&t_startPosForThisOctFLOAT) + XMLoadFloat3(&t_fourthOfDimensionsXMFLOAT));
            // Get the first center for
            for(size_t x = 0; x < 2; ++x)
            {
                for(size_t y = 0; y < 2; ++y)
                {
                    for(size_t z = 0; z < 2; ++z)
                    {
                        // Getting the values for the eight different children
                        XMFLOAT3 t_finalCenter;
                        t_finalCenter.x = t_startPosForThisOctFLOAT.x + t_halfDimensionsXMFLOAT.x * x;
                        t_finalCenter.y = t_startPosForThisOctFLOAT.y + t_halfDimensionsXMFLOAT.y * y;
                        t_finalCenter.z = t_startPosForThisOctFLOAT.z + t_halfDimensionsXMFLOAT.z * z;
                        // Since we are createing a new layer of children we add one depth to it.
                        OctNode* t_intoTheTree = new OctNode(o_treeNode.depth + 1, true, t_halfDimensionsXMFLOAT, t_finalCenter, &o_treeNode);
                        o_treeNode.children[z + (y * 2) + (x * 4)] = t_intoTheTree;
                    }
                }
            }
        }
        void TreeCreator::Update() {}
        void TreeCreator::OnEvent(Event* p_event) {}
    }
}