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
                OctNode();
                ~OctNode(){};
                DirectX::XMVECTOR boxDimensions;
                OctNode* children[8];
                // Keep them so we can check against them in the kid to this node.
                std::vector<uint32_t> objectsInTheArea;
                bool empty = true;
            };
            // struct OctLeaf
            //{
            //    OctLeaf();
            //    ~OctLeaf() {};
            //    DirectX::XMVECTOR boxDimensions;
            //    bool empty = true;
            //};
        public:
            static TreeCreator* GetInstance();

            void Update();
            void OnEvent(Event* p_event) override;
            OctNode treeRoot;

        private:
            TreeCreator();
            ~TreeCreator();
            void CreateTree();

            static TreeCreator* m_singleton;
        };
    }
}
