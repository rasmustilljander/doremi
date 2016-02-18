#pragma once
#include <Doremi/Core/Include/EventHandler/Subscriber.hpp>
#include <DirectXMath.h>
#include <vector>
#include <cstdint>

namespace Doremi
{
    namespace Core
    {
        class TreeCreator : public Subscriber
        {
            struct OctNode
            {
                OctNode(uint8_t p_depth, bool p_empty, DirectX::XMFLOAT3 p_boxDimensions)
                {
                    empty = p_empty;
                    depth = p_depth;
                    boxDimensions = p_boxDimensions;
                };
                OctNode()
                {
                    empty = true;
                    depth = 0;
                    boxDimensions = DirectX::XMFLOAT3(0, 0, 0);
                };
                ~OctNode(){};
                DirectX::XMFLOAT3 boxDimensions;
                std::vector<OctNode> children;
                // Keep them so we can check against them in the kid to this node.
                std::vector<uint32_t> objectsInTheArea;
                bool empty;
                uint8_t depth;
            };

        public:
            TreeCreator();
            ~TreeCreator();
            void Update();
            void OnEvent(Event* p_event) override;
            OctNode treeRoot;
            void CreateTree();

        private:
            void BuildIt(OctNode& o_treeNode);
        };
    }
}
