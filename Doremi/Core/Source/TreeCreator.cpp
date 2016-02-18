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
            treeRoot.boxDimensions = DirectX::XMFLOAT3(100, 100, 100);
            treeRoot.center = DirectX::XMFLOAT3(0, 0, 0);
            CreateAndDivideTheChildren(treeRoot);
        };
        TreeCreator::~TreeCreator() {}

        void TreeCreator::CreateTree()
        {
            // standard root beginning
            // Root checks eight boxes(its children) for anycollision.

            BuildIt(treeRoot);
        }

        void TreeCreator::BuildIt(OctNode& o_treeNode)
        {
            for(size_t i = 0; i < 8; i++)
            {
                // o_treeNode.children[i]->depth = o_treeNode.depth + (uint8_t)1;
                // o_treeNode.children[i]->empty = true;
                // if there is a collision set bool empty to false
                if(true)
                {

                    o_treeNode.empty = false;
                    // If max depth isnt reached
                    if(o_treeNode.depth <= 6)
                    {
                        // If there is a collision we split the node to 8 smaller nodes if we havent reached max depth
                        CreateAndDivideTheChildren(o_treeNode);
                        BuildIt(o_treeNode);
                    }
                    else
                    {

                        // We reached max depth.
                    }
                }
                else
                {
                    // No collision
                    // Set the node to empty = true;
                    o_treeNode.empty = true;
                }
            }
        }

        void TreeCreator::CreateAndDivideTheChildren(OctNode& o_treeNode)
        {
            // Using DirectXNamespace because I couldnt multiply two vectors with eafchother without it
            using namespace DirectX;
            // DirectX::XMVECTOR t_newDimensionsVector = DirectX::XMLoadFloat3(&o_treeNode.boxDimensions);
            // t_newDimensionsVector = t_newDimensionsVector * 0.5f;
            // DirectX::XMFLOAT3 t_newDimensionsFloatThree;
            // DirectX::XMStoreFloat3(&t_newDimensionsFloatThree, t_newDimensionsVector);


            // make the dimensions available for usage and make it half so we can split it up into 8 pieces
            XMVECTOR t_halfDimensions = XMLoadFloat3(&o_treeNode.boxDimensions);
            t_halfDimensions = t_halfDimensions * 0.5f;
            // Same with the center, making it availabe for manipulation
            XMVECTOR t_startPosForThisOct = XMLoadFloat3(&o_treeNode.center);
            t_startPosForThisOct = t_startPosForThisOct - t_halfDimensions;
            // to use for getting the new center we need a xmfloat
            XMFLOAT3 t_startPosForThisOctFLOAT;
            XMStoreFloat3(&t_startPosForThisOctFLOAT, t_startPosForThisOct);

            // DirectX::XMFLOAT3 t_negativeHalfDimensions = DirectX::XMFLOAT3(o_treeNode.boxDimensions.x * -0.5f, o_treeNode.boxDimensions.y * -0.5f,
            // o_treeNode.boxDimensions.z * -0.5f);

            // Store the vecotr to a xmfloat3 for later use
            XMFLOAT3 t_halfDimensionsXMFLOAT;
            XMStoreFloat3(&t_halfDimensionsXMFLOAT, t_halfDimensions);
            XMFLOAT3 t_fourthOfDimensionsXMFLOAT;
            XMStoreFloat3(&t_fourthOfDimensionsXMFLOAT, t_halfDimensions * 0.5f);
            // DirectX::XMVECTOR t_newCenter = t_startPosForThisOct + t_halfDimensions * 0.5f;

            XMStoreFloat3(&t_startPosForThisOctFLOAT, XMLoadFloat3(&t_startPosForThisOctFLOAT) + XMLoadFloat3(&t_fourthOfDimensionsXMFLOAT));

            // Get the first center for

            for(size_t x = 0; x < 2; ++x)
            {
                for(size_t y = 0; y < 2; ++y)
                {
                    for(size_t z = 0; z < 2; ++z)
                    {
                        // t_startPosForThisOctFLOAT.x = t_startPosForThisOctFLOAT.x + t_fourthOfDimensionsXMFLOAT.x;
                        // t_startPosForThisOctFLOAT.y = t_startPosForThisOctFLOAT.y + t_fourthOfDimensionsXMFLOAT.y;
                        // t_startPosForThisOctFLOAT.z = t_startPosForThisOctFLOAT.z + t_fourthOfDimensionsXMFLOAT.z;
                        // DirectX::XMVECTOR t_halfDimensionsForTheNextCenter = t_newDimensionsVector * 0.5f;
                        // DirectX::XMFLOAT3 t_changesToCenter = DirectX::XMFLOAT3(o_treeNode.boxDimensions.x * 0.25f + x *
                        // (o_treeNode.boxDimensions.x * 0.5), o_treeNode.boxDimensions.y * 0.25f + y * (o_treeNode.boxDimensions.y * 0.5),
                        // o_treeNode.boxDimensions.z * 0.25f + z * (o_treeNode.boxDimensions.z * 0.5));
                        // t_newCenter
                        XMFLOAT3 t_finalCenter;
                        t_finalCenter.x = t_startPosForThisOctFLOAT.x + t_halfDimensionsXMFLOAT.x * x;
                        t_finalCenter.y = t_startPosForThisOctFLOAT.y + t_halfDimensionsXMFLOAT.y * y;
                        t_finalCenter.z = t_startPosForThisOctFLOAT.z + t_halfDimensionsXMFLOAT.z * z;

                        OctNode* t_intoTheTree = new OctNode(o_treeNode.depth + 1, true, t_halfDimensionsXMFLOAT, t_finalCenter);
                        o_treeNode.children[z + (y * 2) + (x * 4)] = t_intoTheTree;
                        // o_treeNode.boxDimensions;
                    }
                }
            }
        }

        void TreeCreator::Update() {}
        void TreeCreator::OnEvent(Event* p_event) {}
    }
}