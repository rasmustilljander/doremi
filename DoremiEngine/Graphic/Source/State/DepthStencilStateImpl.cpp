#pragma once
#include <DoremiEngine/Graphic/Include/Internal/State/DepthStencilStateImpl.hpp>

namespace DoremiEngine
{
    namespace Graphic
    {
        DepthStencilStateImpl::DepthStencilStateImpl() : m_stencilState(nullptr) {}
        DepthStencilStateImpl::~DepthStencilStateImpl() {}

        ID3D11DepthStencilState* DepthStencilStateImpl::GetDepthStencilState() { return m_stencilState; }
        void DepthStencilStateImpl::SetDepthStencilState(ID3D11DepthStencilState* p_depthStencilState) { m_stencilState = p_depthStencilState; }
    }
}