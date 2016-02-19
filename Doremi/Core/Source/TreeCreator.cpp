#include <Doremi/Core/Include/TreeCreator.hpp>

namespace Doremi
{
    namespace Core
    {
        //&& TreeCreator* TreeCreator::m_singleton = nullptr;
        // TreeCreator* TreeCreator::GetInstance()
        //{
        //    if(m_singleton == nullptr)
        //    {
        //        m_singleton = new TreeCreator();
        //    }
        //    return m_singleton;
        //}
        TreeCreator::TreeCreator()
        {
            // Set the depth of the oct tree
            m_treeDepth = 4;

            // Set the box for the world
            treeRoot.boxDimensions = DirectX::XMFLOAT3(100, 100, 100);

            // Set the center for the box of the world
            treeRoot.center = DirectX::XMFLOAT3(0, 0, 0);

            // Start up for the oct tree. Without this function we don't have anything to start with
            CreateAndDivideTheChildren(treeRoot);
            int hej2 = 0;
        };
        TreeCreator::~TreeCreator() {}

        void TreeCreator::CreateTree()
        {
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
                    if(o_treeNode.depth <= m_treeDepth - 2)
                    {
                        // Collision and max depth wasn't reached

                        // Prepare the next BuildTree() function with 8 newly created children
                        CreateAndDivideTheChildren(*o_treeNode.children[i]);

                        // We start over in the next node to see if we can split it even further
                        BuildTree(*o_treeNode.children[i]);
                    }
                    else
                    {
                        // Collision but we reached max depth
                    }
                }
                else
                {
                    // No collision
                    o_treeNode.empty = true;
                }
            }
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