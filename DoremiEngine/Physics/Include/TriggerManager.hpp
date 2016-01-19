#pragma once
#include <DirectXMath.h>
#include <vector>
using namespace std;
using namespace DirectX;
namespace DoremiEngine
{
    namespace Physics
    {
        class TriggerManager
        {
        public:
            /**
            This adds a trigger TODOJB update docs*/
            virtual void AddTrigger(int p_id) = 0;
        };
    }
}