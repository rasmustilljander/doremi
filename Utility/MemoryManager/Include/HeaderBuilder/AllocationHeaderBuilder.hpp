#pragma once

#include <cstdint>
#include <array>

namespace Utility
{
    namespace MemoryManager
    {
        class AllocationHeaderBuilder
        {
            private:
            AllocationHeaderBuilder();
            virtual ~AllocationHeaderBuilder();

            public:
            /**
            Sets the byte to zero
            */
            // TODORT move to .cpp file
            static void SetByteAsZero(void* p_pointerToByte) { *(uint8_t*)p_pointerToByte = 0; }

            /**
                Sets the byte to a specific value;
            */
            // TODORT move to .cpp file
            static void SetByte(void* p_pointerToByte, const uint8_t& value) { *(uint8_t*)p_pointerToByte = value; }

            // TODORT move to .cpp file
            static void BuildCustomByteBasedOnFlags(void* p_pointerToByte, const bool& f0, const bool& f1, const bool& f2, const bool& f3,
                                                    const bool& f4, const bool& f5, const bool& f6, const bool& f7)
            {
                uint8_t flagValue = 0;
                if(f0)
                {
                    flagValue |= 1;
                }
                if(f1)
                {
                    flagValue |= 2;
                }
                if(f2)
                {
                    flagValue |= 4;
                }
                if(f3)
                {
                    flagValue |= 8;
                }
                if(f4)
                {
                    flagValue |= 16;
                }
                if(f5)
                {
                    flagValue |= 32;
                }
                if(f6)
                {
                    flagValue |= 64;
                }
                if(f7)
                {
                    flagValue |= 128;
                }
                *(uint8_t*)p_pointerToByte = flagValue;
            }

            // TODORT move to .cpp file
            static void MarkByteBasedOnFlags(void* p_pointerToByte, const std::array<bool, 8>& flags)
            {
                uint8_t flagValue = 0;
                if(flags[0])
                {
                    flagValue |= 1;
                }
                if(flags[1])
                {
                    flagValue |= 2;
                }
                if(flags[2])
                {
                    flagValue |= 4;
                }
                if(flags[3])
                {
                    flagValue |= 8;
                }
                if(flags[4])
                {
                    flagValue |= 16;
                }
                if(flags[5])
                {
                    flagValue |= 32;
                }
                if(flags[6])
                {
                    flagValue |= 64;
                }
                if(flags[7])
                {
                    flagValue |= 128;
                }
                *(uint8_t*)p_pointerToByte = flagValue;
            }

            // TODORT move to .cpp file
            static void MarkByteAsOccupied(void* p_pointerToByte)
            {
                /*
                   XXXX XXXX
                 | 0000 0001
                */
                *(uint8_t*)p_pointerToByte |= 1;
            }

            // TODORT move to .cpp file
            static void MarkByteAsFree(void* p_pointerToByte)
            {
                /*
                    XXXX XXXX
                  & 1111 1110
                */
                *(uint8_t*)p_pointerToByte &= 254;
            }

            // TODORT move to .cpp file
            static std::array<bool, 8>& FetchAllFlagsFromByte(const void const* p_pointerToByte)
            {
                const uint8_t& value = *(uint8_t*)p_pointerToByte;
                std::array<bool, 8> returnValue; // All false at the beginning
                if((value & 1) == 1)
                {
                    returnValue[0] = true;
                }
                if((value & 2) == 2)
                {
                    returnValue[1] = true;
                }
                if((value & 4) == 4)
                {
                    returnValue[2] = true;
                }
                if((value & 8) == 8)
                {
                    returnValue[3] = true;
                }
                if((value & 16) == 16)
                {
                    returnValue[4] = true;
                }
                if((value & 32) == 32)
                {
                    returnValue[5] = true;
                }
                if((value & 64) == 64)
                {
                    returnValue[6] = true;
                }
                if((value & 128) == 128)
                {
                    returnValue[7] = true;
                }
                return returnValue;
            }
        };
    }
}