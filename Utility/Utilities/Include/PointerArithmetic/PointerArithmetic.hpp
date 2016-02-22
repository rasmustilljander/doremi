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
                return reinterpret_cast<T>(reinterpret_cast<size_t>(p_src) + p_value);
            }

            // TODORT
            // Not sure how this works if one attempts to use "real types" instead of pointers.
            template <typename T> size_t Difference(T const p_low, T const p_high)
            {
                return (reinterpret_cast<size_t>(p_high) - reinterpret_cast<size_t>(p_low));
            }

            static bool AssertAdresstInside(void* const p_adressToAssert, void* const p_low, void* const p_high)
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