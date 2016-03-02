#pragma once
#ifndef MUTEX_HPP
#define MUTEX_HPP
#include <DoremiEditor\DRMEditorPlugin\Include\Includes.hpp>
namespace DoremiEditor
{
    namespace Plugin
    {
        class Mutex
        {
        private:
            HANDLE handle_;

        public:
            Mutex(const char* name = "__my_mutex__")
            {
                // handle_ = CreateMutex(nullptr, false, (LPWSTR)(name));
            }
            ~Mutex() { ReleaseMutex(handle_); }
            void Create(const char* name = "__my_mutex__")
            {
                // handle_ = CreateMutex(nullptr, false, (LPWSTR)(name));

                handle_ = OpenMutex(MUTEX_ALL_ACCESS, FALSE, (LPWSTR)(name)); //(LPWSTR)(name) TEXT("__info_Mutex__")
            }
            bool Lock(DWORD milliseconds = 10000000000000)
            {
                DWORD check;
                check = WaitForSingleObject(handle_, milliseconds);
                // Sleep(milliseconds); //check returneras direkt så vänta ut skiten oxå??
                if(check == WAIT_ABANDONED)
                {
                    return false; // didnt get mutex
                }

                return true; // got mutex
            }
            void Unlock() { ReleaseMutex(handle_); }
        };
    }
}
#endif