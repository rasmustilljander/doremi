#pragma once
#include <DoremiEngine/Graphic/Include/Interface/State/DepthStencilState.hpp>

namespace DoremiEngine
{
    namespace Graphic
    {
        class DepthStencilStateImpl : public DepthStencilState
        {
        public:
            DepthStencilStateImpl();
            virtual ~DepthStencilStateImpl();

            void SetDepthStencilState(ID3D11DepthStencilState* p_depthStencilState) override;

            ID3D11DepthStencilState* GetDepthStencilState() override;

        private:
            ID3D11DepthStencilState* m_stencilState;
        };
    }
}