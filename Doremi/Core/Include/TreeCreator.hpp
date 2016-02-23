#pragma once
#include <Doremi/Core/Include/EventHandler/Subscriber.hpp>
#include <DirectXMath.h>
#include <vector>
#include <cstdint>

namespace DoremiEngine
{
    namespace Core
    {
        class SharedContext;
    }
}
namespace Doremi
{
    namespace Core
    {
        class TreeCreator : public Subscriber
        {


        public:
            struct OctNode
            {
                OctNode(uint8_t p_depth, bool p_empty, DirectX::XMFLOAT3 p_boxDimensions, DirectX::XMFLOAT3 p_center, OctNode* p_parent)
                {
                    empty = p_empty;
                    depth = p_depth;
                    boxDimensions = p_boxDimensions;
                    center = p_center;
                    parent = p_parent;
                    loopInfo = 0;
                };
                OctNode()
                {
                    empty = true;
                    depth = 0;
                    boxDimensions = DirectX::XMFLOAT3(0, 0, 0);
                    center = DirectX::XMFLOAT3(0, 0, 0);
                    loopInfo = 0;
                };
                ~OctNode(){};
                OctNode* children[8];// 32
                DirectX::XMFLOAT3 boxDimensions;// 12 
                DirectX::XMFLOAT3 center;// 12
                OctNode* parent;// 4

                // to know where we are.
                uint8_t loopInfo;// 1
                uint8_t empty;
                uint8_t depth;// 1
                // Keep them so we can check against them in the kid to this node.
                std::vector<uint32_t> objectsInTheArea;// 16/24
            };

            TreeCreator::TreeCreator(const DoremiEngine::Core::SharedContext& p_sharedContext);
            ~TreeCreator();

            void Update();
            void OnEvent(Event* p_event) override;
            void CreateTree();

            OctNode treeRoot;
            uint16_t m_treeDepth;


        private:
            OctNode* m_currentNode;
            const DoremiEngine::Core::SharedContext& m_sharedContext;
            void BuildTree();
            void CreateAndDivideTheChildren(OctNode& o_treeNode);
            // OctNode GetTree() const { return treeRoot; }
        };
    }
}
