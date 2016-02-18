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
        TreeCreator::TreeCreator() { treeRoot.boxDimensions = DirectX::XMFLOAT3(100, 100, 100); };
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

        void TreeCreator::CreateAndDivideTheChildren(OctNode& o_treeNode) {}

        void TreeCreator::Update() {}
        void TreeCreator::OnEvent(Event* p_event) {}
    }
}