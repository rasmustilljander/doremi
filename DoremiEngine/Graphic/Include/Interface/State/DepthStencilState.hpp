#pragma once
class ID3D11DepthStencilState;
namespace DoremiEngine
{
    namespace Graphic
    {
        class DepthStencilState
        {
        public:
            virtual void SetDepthStencilState(ID3D11DepthStencilState* p_depthStencilState) = 0;
            virtual ID3D11DepthStencilState* GetDepthStencilState() = 0;
        };
    }
}