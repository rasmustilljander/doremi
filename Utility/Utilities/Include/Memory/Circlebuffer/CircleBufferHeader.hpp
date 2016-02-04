#pragma once
#include <Utility/Utilities/Include/Memory/Circlebuffer/CircleBufferType.hpp>
namespace Doremi
{
    namespace Utilities
    {
        namespace Memory
        {
            struct CircleBufferHeader
            {
                CircleBufferHeader() : packageType(CircleBufferTypeEnum::UNKNOWN), packageSize(0) {}
                CircleBufferType packageType;
                int32_t packageSize;
            };
        }
    }
}