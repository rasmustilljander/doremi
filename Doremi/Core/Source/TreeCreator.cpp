#include <Doremi/Core/Include/TreeCreator.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <DoremiEngine/Physics/Include/RigidBodyManager.hpp>


namespace Doremi
{
    namespace Core
    {
        TreeCreator::TreeCreator(const DoremiEngine::Core::SharedContext& p_sharedContext) : m_sharedContext(p_sharedContext)
        {
            // m_sharedContext = p_sharedContext;

            // Set the depth of the oct tree
            m_treeDepth = 7;

            // Set the box for the world
            treeRoot.boxDimensions = DirectX::XMFLOAT3(7000, 2150, 7000);

            // Set the center for the box of the world
            treeRoot.center = DirectX::XMFLOAT3(0, 0, 0);

            // Start up for the oct tree. Without this function we don't have anything to start with
            CreateAndDivideTheChildren(treeRoot);
            int hej2 = 0;
        };
        TreeCreator::~TreeCreator() {}

        void TreeCreator::CreateTree()
        {
            // m_sharedContext.GetPhysicsModule().GetRigidBodyManager().AddBoxBodyDynamic(//(myID, quadCenter, XMFLOAT4(0, 0, 0, 1),
            // XMFLOAT3(quadSize.x * 0.5f, 0.5f, quadSize.y * 0.5f),
            // materialID);
            // m_sharedContext.GetPhysicsModule().GetRigidBodyManager().SetTrigger(myID, true);

            // Get the max entities so we can collect all of the draw objects in the world
            size_t maxEntities = EntityHandler::GetInstance().GetLastEntityIndex();
            // Loop over all entities to add the things that have render components into the list.
            for(size_t j = 0; j < maxEntities; j++)
            {
                // Check if entity will be affected by the render.
                if(EntityHandler::GetInstance().HasComponents(j, (int)ComponentType::Render))
                {
                    // If they are colliding with the rootTree box we add them to the list.
                    if(true)
                    {
                        // Colliding with the box and it is a render component
                        treeRoot.objectsInTheArea.push_back(j);
                    }
                    else
                    {
                        // Not colliding with the box but are a render component
                    }
                }
            }

            // Start building the tree
            BuildTree(treeRoot);

            int hej10 = 0;
        }

        void TreeCreator::BuildTree(OctNode& o_treeNode)
        {
            // Looping through the
            for(size_t i = 0; i < 8; i++)
            {
                // if there is a collision between the child box and any object that needs drawing
                if(true) // TODOEA Here should be the collision check between all the objects in the objectsInTheArea list
                {
                    // Since we had a collision, there is something in this box
                    o_treeNode.empty = false;

                    // If max depth isnt reached , minus two is needed to get the depth wanted.
                    if(o_treeNode.depth < m_treeDepth - 1)
                    {
                        // Collision and max depth wasn't reached

                        // Prepare the next BuildTree() function with 8 newly created children
                        CreateAndDivideTheChildren(*o_treeNode.children[i]);

                        // We start over in the next node to see if we can split it even further
                        BuildTree(*o_treeNode.children[i]);
                    }
                    else if(o_treeNode.depth == m_treeDepth - 1)
                    {
                        // int hejsan = 0;
                        // Collision but we reached max depth
                        // Check where the objects will be placed in this nodes children
                    }
                    else
                    {
                        // Collision but we're beyond max depth
                    }
                }
                else
                {
                    // No collision
                    o_treeNode.empty = true;
                }
            }


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
            XMStoreFloat3(&t_startPosForThisOctFLOAT, t_startPosForThisOct);

            // Store the vecotr to a xmfloat3 for later use
            XMFLOAT3 t_halfDimensionsXMFLOAT;
            XMStoreFloat3(&t_halfDimensionsXMFLOAT, t_halfDimensions);
            XMFLOAT3 t_fourthOfDimensionsXMFLOAT;
            XMStoreFloat3(&t_fourthOfDimensionsXMFLOAT, t_halfDimensions * 0.5f);

            XMStoreFloat3(&t_startPosForThisOctFLOAT, XMLoadFloat3(&t_startPosForThisOctFLOAT) + XMLoadFloat3(&t_fourthOfDimensionsXMFLOAT));

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
                        OctNode* t_intoTheTree = new OctNode(o_treeNode.depth + 1, true, t_halfDimensionsXMFLOAT, t_finalCenter);
                        o_treeNode.children[z + (y * 2) + (x * 4)] = t_intoTheTree;
                    }
                }
            }
        }
        void TreeCreator::Update() {}
        void TreeCreator::OnEvent(Event* p_event) {}
    }
}