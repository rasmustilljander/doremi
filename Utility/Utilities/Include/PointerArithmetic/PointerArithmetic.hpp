#pragma once

namespace Doremi
{
    namespace Utilities
    {
        namespace PointerArithmetic
        {
            // TODORT
            // Not sure how this works if one attempts to use "real types" instead of pointers.
            template <typename T> T Addition(T const p_src, const size_t& p_value)
            {
                // TODORT Check for overflow / underflow ?
                return reinterpret_cast<void*>(reinterpret_cast<size_t>(p_src) + p_value);
            }

            bool AssertAdresstInside(void* const p_adressToAssert, void* const p_low, void* const p_high)
            {
                if(p_adressToAssert >= p_low && p_adressToAssert < p_high)
                {
                    return true; // Inside
                }
                return false; // Outside
            }
        }
    }
}