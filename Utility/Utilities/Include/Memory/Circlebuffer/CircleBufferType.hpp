#pragma once
namespace Doremi
{
    namespace Utilities
    {
        namespace Memory
        {
            enum class CircleBufferTypeEnum : int8_t
            {
                TEXT,
                DATA,
                UNKNOWN,
            };

            union CircleBufferType
            {
                CircleBufferType(const CircleBufferTypeEnum& p_enum) : typeEnum(p_enum) {}

                CircleBufferType(const int8_t& p_type) : typeValue(p_type) {}

                CircleBufferType& operator=(const CircleBufferTypeEnum& p_enum)
                {
                    this->typeEnum = p_enum;
                    return *this;
                }

                bool operator==(const CircleBufferTypeEnum& p_enum)
                {
                    if(typeEnum == p_enum)
                    {
                        return true;
                    }
                    return false;
                }

                int8_t typeValue;
                CircleBufferTypeEnum typeEnum;
            };
        }
    }
}