#pragma once
#include <DirectXMath.h>
namespace Doremi
{
    namespace Core
    {
        using namespace DirectX;
        /**
        Only intended as an example of what a component might look like
        Should not be used anywhere. Could be removed once exmaples are no longer necessary*/
        struct PlatformPatrolComponent
        {
            XMFLOAT3 startPosition;
            XMFLOAT3 endPosition;
            PlatformPatrolComponent(XMFLOAT3 p_startPosition, XMFLOAT3 p_endPosition) : startPosition(p_startPosition), endPosition(p_endPosition) {}
            PlatformPatrolComponent() : startPosition(0), endPosition(0) {}
        };
    }
}