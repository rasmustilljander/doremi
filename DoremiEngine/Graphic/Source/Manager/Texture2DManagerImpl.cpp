#pragma once
#include <Internal/Manager/Texture2DManagerImpl.hpp>
#include <GraphicModuleContext.hpp>
#include <string>
namespace DoremiEngine
{
    namespace Graphic
    {
        Texture2DManagerImpl::Texture2DManagerImpl(const GraphicModuleContext& p_graphicContext) : m_graphicContext(p_graphicContext) {}
        Texture2DManagerImpl::~Texture2DManagerImpl() {}
        Texture2D* Texture2DManagerImpl::BuildTexture2D(const std::string& m_fileName) 
        {
            return nullptr;
        }
        
    }
}