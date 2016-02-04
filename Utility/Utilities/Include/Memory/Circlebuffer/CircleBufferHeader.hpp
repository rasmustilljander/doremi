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
                CircleBufferHeader() : packageType(CircleBufferTypeEnum::UNKNOWN), packageSize(0), packageFlags(0) {}
                CircleBufferType packageType;
                int32_t packageSize;
                int8_t packageFlags; // TODORT 2012-02-04 Not currently used
            };
        }
    }
}