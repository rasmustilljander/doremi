#pragma once
#include <string>
#include <Windows.h>

#include <iostream>
namespace DoremiEngine
{
    namespace Graphic
    {
        static std::wstring StringToWstring(const std::string& s)
        {
            int len;
            int slength = (int)s.length() + 1;
            len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
            wchar_t* buf = new wchar_t[len];
            MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
            std::wstring r(buf);
            delete[] buf;
            return r;
        }

        static bool CheckHRESULT(HRESULT p_result, const std::string& p_errorMessage)
        {
            if(FAILED(p_result))
            {
                // TODOKO Print error or send to debugg logger
                std::cout << p_errorMessage << std::endl;
                return false;
            }
            return true;
        }
    }
}