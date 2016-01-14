#pragma once
#include <DoremiEngine/Graphic/Include/Interface/State/RasterizerState.hpp>

namespace DoremiEngine
{
    namespace Graphic
    {
        class RasterizerStateImpl : public RasterizerState
        {
        public:
            RasterizerStateImpl();
            virtual ~RasterizerStateImpl();

            void SetRasterizerState(ID3D11RasterizerState* p_rasterizerState) override;

            ID3D11RasterizerState* GetRasterizerState() override;

        private:
            ID3D11RasterizerState* m_rasterizerState;
        };
    }
}