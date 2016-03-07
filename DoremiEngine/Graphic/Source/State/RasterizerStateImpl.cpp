#pragma once
#include <DoremiEngine/Graphic/Include/Internal/State/RasterizerStateImpl.hpp>

namespace DoremiEngine
{
    namespace Graphic
    {
        RasterizerStateImpl::RasterizerStateImpl() : m_rasterizerState(nullptr) {}
        RasterizerStateImpl::~RasterizerStateImpl() {}

        ID3D11RasterizerState* RasterizerStateImpl::GetRasterizerState() { return m_rasterizerState; }
        void RasterizerStateImpl::SetRasterizerState(ID3D11RasterizerState* p_rasterizerState) { m_rasterizerState = p_rasterizerState; }
    }
}