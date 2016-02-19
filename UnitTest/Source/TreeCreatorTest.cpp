#include <gtest/gtest.h>
#include <Doremi/Core/Include/EntityComponent/EntityHandler.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/TransformComponent.hpp>
#include <Doremi/Core/Include/TreeCreator.hpp>
#include <DirectXMath.h>
#include <vector>
#include <cstdint>

using namespace Doremi::Core;
using namespace DirectX;
TreeCreator t_treeCreator;
bool CheckDimensionsOnTree(TreeCreator::OctNode& o_octTree)
{
    bool functionResult;
    for(size_t i = 0; i < 8; i++)
    {
        if(o_octTree.children[i]->boxDimensions.x = o_octTree.boxDimensions.x)
        {
            if(o_octTree.children[i]->boxDimensions.y = o_octTree.boxDimensions.y)
            {
                if(o_octTree.children[i]->boxDimensions.z = o_octTree.boxDimensions.z)
                {
                    if(t_treeCreator.m_treeDepth - 1 > o_octTree.depth)
                    {
                        functionResult = CheckDimensionsOnTree(*o_octTree.children[i]);
                        if(functionResult)
                        {
                        }
                        else
                        {
                            return false;
                        }
                    }
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    return true;
}
TEST(TreeCreatorTest, Dimensions)
{
    t_treeCreator.CreateTree();
    bool result = false;
    result = CheckDimensionsOnTree(t_treeCreator.treeRoot);
    ASSERT_TRUE(result);
}