#pragma once

namespace Doremi
{
    namespace Utilities
    {
        namespace IO
        {
            class Mutex
            {
            public:
                /**
                Returns true if successfully initialized
                */
                virtual bool Initialize(const std::string& p_name) = 0;

                /**
                TODO docs
                */
                // TODOCONFIG
                // TODOXX
                // TODORT
                virtual bool AttemptLock(const uint32_t& p_timeout = 1000) = 0;

                /**
                TODO docs
                */
                virtual void Unlock() = 0;
            };
        }
    }
}