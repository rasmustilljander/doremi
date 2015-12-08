#pragma once
#include <string>
namespace DoremiEngine
{
    namespace Graphic
    {
        class Texture2D;
        class Texture2DManager 
        {
            virtual Texture2D* BuildTexture2D(const std::string& m_fileName) = 0;
        };
    }
}