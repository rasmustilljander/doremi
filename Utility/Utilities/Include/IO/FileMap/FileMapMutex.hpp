#pragma once

#include <Windows.h>
#include <string>
#include <Utility/Utilities/Include/String/StringHelper.hpp>
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
                FileMapMutex() : m_handle(nullptr)
                {
                    // TODORT
                    // TODOXX
                    // TODOCONFIG
                    m_lockTime = 10000000000000;
                }

                virtual ~FileMapMutex() { ReleaseMutex(m_handle); }

                /**
                Returns true if successful
                */
                bool Initialize(const std::string& p_name)
                {
                    // Set variables
                    m_name = p_name;

                    // Set success variable
                    bool success = false;

                    // Open/CreateMutex
                    success = InitializeExternalMutex();

                    if(m_handle)
                    {
                        success = true;
                    }
                    return success;
                }

                bool AttemptLock(const uint32_t& p_timeout) override
                {
                    DWORD check;
                    check = WaitForSingleObject(m_handle, p_timeout);

                    if(check == WAIT_ABANDONED)
                    {
                        return false; // Did not get mutex
                    }

                    return true; // Got mutex
                }

                /**
                    Will only return if locked.
                */
                void Lock()
                {
                    DWORD check;
                    while(true)
                    {
                        // TODORT Define better than 1000?
                        check = WaitForSingleObject(m_handle, 1000);
                        if(check == WAIT_OBJECT_0)
                        {
                            break;
                        }
                    }
                }

                /**
                    TODO docs
               */
                void Unlock() override { ReleaseMutex(m_handle); }

            protected:
                bool InitializeExternalMutex()
                {
                    m_handle = CreateMutex(nullptr, FALSE, String::s2ws(m_name).c_str());
                    if(m_handle != NULL)
                    {
                        return true;
                    }
                    return false;
                }

                std::string m_name;
                HANDLE m_handle;
                size_t m_lockTime;
            };
        }
    }
}
