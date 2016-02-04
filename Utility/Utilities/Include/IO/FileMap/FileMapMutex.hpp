#pragma once

#include <string>
#include <Utility/Utilities/Include/IO/Mutex/Mutex.hpp>
#include <cstdint>

namespace Doremi
{
    namespace Utilities
    {
        namespace IO
        {

            class FileMapMutex : public Mutex
            {
            public:
                FileMapMutex();

                virtual ~FileMapMutex();

                /**
                Returns true if successful
                */
                bool Initialize(const std::string& p_name);

                /**
                TODO docs
                */
                bool AttemptLock(const uint32_t& p_timeout) override;

                /**
                    Will only return if locked.
                */
                void Lock();

                /**
                    TODO docs
               */
                void Unlock() override;

            protected:
                bool InitializeExternalMutex();

                std::string m_name;
                void* m_handle;
                size_t m_lockTime;
            };
        }
    }
}
