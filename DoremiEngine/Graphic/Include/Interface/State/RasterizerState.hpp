#pragma once
class ID3D11RasterizerState;
namespace DoremiEngine
{
    namespace Graphic
    {
        class RasterizerState
        {
        public:
            virtual void SetRasterizerState(ID3D11RasterizerState* p_rasterizerState) = 0;
            virtual ID3D11RasterizerState* GetRasterizerState() = 0;
        };
    }
}