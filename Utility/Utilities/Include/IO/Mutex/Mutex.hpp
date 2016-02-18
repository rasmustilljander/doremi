#pragma once
#include <mutex>

namespace Doremi
{
    namespace Utilities
    {
        namespace IO
        {
            class Mutex : public std::mutex
            {
            public:
                /**
                Returns true if successfully initialized
                */
                virtual bool Initialize(const std::string& p_name) = 0;

                virtual void lock() = 0;

                virtual bool try_lock() = 0;

                virtual bool try_lock(const uint32_t& p_timeout) = 0;

                virtual void unlock() = 0;
            };
        }
    }
}