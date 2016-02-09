#include <IO/FileMap/FileMapMutex.hpp>

#include <Windows.h>
#include <Utility/Utilities/Include/String/StringHelper.hpp>

namespace Doremi
{
    namespace Utilities
    {
        namespace IO
        {

            FileMapMutex::FileMapMutex() : m_handle(nullptr)
            {
                // TODORT
                // TODOXX
                // TODOCONFIG
                m_lockTime = 10000000000000;
            }

            FileMapMutex::~FileMapMutex() { ReleaseMutex(m_handle); }

            bool FileMapMutex::Initialize(const std::string& p_name)
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

            bool FileMapMutex::AttemptLock(const uint32_t& p_timeout)
            {
                DWORD check;
                check = WaitForSingleObject(m_handle, p_timeout);

                if(check == WAIT_ABANDONED)
                {
                    return false; // Did not get mutex
                }

                return true; // Got mutex
            }

            void FileMapMutex::Lock()
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

            void FileMapMutex::Unlock() { ReleaseMutex(m_handle); }

            bool FileMapMutex::InitializeExternalMutex()
            {
                m_handle = CreateMutex(nullptr, FALSE, String::s2ws(m_name).c_str());
                if(m_handle != NULL)
                {
                    return true;
                }
                return false;
            }
        }
    }
}