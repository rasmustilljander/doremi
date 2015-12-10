#pragma once
#include <Internal/Manager/ParticleSystemManagerImpl.hpp>
#include <GraphicModuleContext.hpp>
namespace DoremiEngine
{
    namespace Graphic
    {
        ParticleSystemManagerImpl::ParticleSystemManagerImpl(const GraphicModuleContext& p_graphicContext) : m_graphicContext(p_graphicContext) {}
        ParticleSystemManagerImpl::~ParticleSystemManagerImpl() {}
    }
}