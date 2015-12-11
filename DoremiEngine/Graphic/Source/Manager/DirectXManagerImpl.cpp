#pragma once
#include <Internal/Manager/DirectXManagerImpl.hpp>
#include <GraphicModuleContext.hpp>
#include <HelpFunctions.hpp>
#include <d3d11_1.h>
#include <SDL2/SDL.h>

#include <iostream>
namespace DoremiEngine
{
    namespace Graphic
    {
        DirectXManagerImpl::DirectXManagerImpl(const GraphicModuleContext& p_graphicContext) : m_graphicContext(p_graphicContext) {}
        DirectXManagerImpl::~DirectXManagerImpl() {}

        void InitializeSDL()
        {
            if(!SDL_WasInit(SDL_INIT_VIDEO))
            {
                if(SDL_Init(SDL_INIT_VIDEO) != 0)
                {
                    std::cout << "SDL_Ubut Error: " << SDL_GetError() << std::endl;
                    SDL_QUIT;
                }
                else
                {
                    // TODO logger maybe
                }
            }
        }

        void DirectXManagerImpl::InitializeDirectX()
        {

            InitializeSDL();

            SDL_Window* m_win = SDL_CreateWindow("Do-Re-Mi by Let Him Be: Interactive", 1200, 200, 800, 800,
                                                 SDL_WINDOW_SHOWN); // TODOKO Get height and width form reliable source
            if(!m_win)
            {
                std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
                SDL_QUIT;
            }
            DXGI_SWAP_CHAIN_DESC scd;

            // clear out the struct for use
            ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

            // fill the swap chain description struct
            scd.BufferCount = 1; // one back buffer
            scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // use 32-bit color
            scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // how swap chain is to be used
            scd.OutputWindow = GetActiveWindow(); // the window to be used
            if(scd.OutputWindow == nullptr)
            {
                InitializeSDL();
                SDL_Window* m_win = SDL_CreateWindow("Do-Re-Mi by Let Him Be: Interactive", 1200, 200, 800, 800,
                                                     SDL_WINDOW_SHOWN); // TODOKO Get height and width form reliable source
                if(!m_win)
                {
                    std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
                    SDL_QUIT;
                }
            }
            scd.OutputWindow = GetActiveWindow();
            scd.SampleDesc.Count = 4; // how many multisamples
            scd.Windowed = TRUE; // windowed/full-screen mode

            // TODOKO CHECK THIS pls :)
            HRESULT res = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, NULL, NULL,
                                                        D3D11_SDK_VERSION, &scd, &m_swapChain, &m_device, NULL, &m_deviceContext);

            ID3D11Texture2D* t_BackBuffer;
            m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&t_BackBuffer);
            m_device->CreateRenderTargetView(t_BackBuffer, NULL, &m_backBuffer);
            t_BackBuffer->Release();

            // Rasterizer info, here you can change from backface culling mm
            // We might want this as a class or something to enable easy switching between rasterizers
            D3D11_RASTERIZER_DESC rastDesc;
            ZeroMemory(&rastDesc, sizeof(rastDesc));
            rastDesc.FillMode = D3D11_FILL_SOLID;
            rastDesc.CullMode = D3D11_CULL_NONE;
            rastDesc.FrontCounterClockwise = false;
            rastDesc.DepthBias = 0;
            rastDesc.DepthBiasClamp = 0.0f;
            rastDesc.SlopeScaledDepthBias = 0.0f;
            rastDesc.DepthClipEnable = false;
            rastDesc.ScissorEnable = false;
            rastDesc.MultisampleEnable = true;
            rastDesc.AntialiasedLineEnable = false;
            ID3D11RasterizerState* tRastHandle;
            res = m_device->CreateRasterizerState(&rastDesc, &tRastHandle);
            m_deviceContext->RSSetState(tRastHandle);

            // Depth buffer
            // Might want this in a class for readability and easy changing between states
            D3D11_TEXTURE2D_DESC dbdesc;
            ZeroMemory(&dbdesc, sizeof(dbdesc));
            dbdesc.Width = 800; // TODOKO take height and width from a reliable source
            dbdesc.Height = 800;
            dbdesc.MipLevels = 1;
            dbdesc.ArraySize = 1;
            dbdesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
            dbdesc.SampleDesc.Count = 4;
            dbdesc.SampleDesc.Quality = 0;
            dbdesc.Usage = D3D11_USAGE_DEFAULT;
            dbdesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
            dbdesc.CPUAccessFlags = 0;
            dbdesc.MiscFlags = 0;

            res = m_device->CreateTexture2D(&dbdesc, NULL, &m_depthBuffer);

            D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
            ZeroMemory(&descDSV, sizeof(descDSV));

            descDSV.Format = dbdesc.Format;
            descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
            descDSV.Texture2D.MipSlice = 0;


            res = m_device->CreateDepthStencilView(m_depthBuffer, &descDSV, &m_depthView);
            m_deviceContext->OMSetRenderTargets(1, &m_backBuffer, m_depthView);

            // Viewport
            // TODOKO change so width and height is modifiable and taken from some smarter place
            D3D11_VIEWPORT viewport;
            ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

            viewport.TopLeftX = 0;
            viewport.TopLeftY = 0;
            viewport.Width = 800;
            viewport.Height = 600;
            viewport.MinDepth = 0.0f;
            viewport.MaxDepth = 1.0f;

            m_deviceContext->RSSetViewports(1, &viewport);

            // TODO add more different things like transparancy
            // TODOKO Move code to better location
            // int vertexShaderID = LoadShader(ShaderType::VertexShader, "BasicVertexShader.hlsl");
            // int pixelShadderID = LoadShader(ShaderType::PixelShader, "BasicPixelShader.hlsl");

            // D3D11_INPUT_ELEMENT_DESC ied[] =
            //{
            //    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            //    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            //    { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            //};

            // m_vertexShaders[vertexShaderID]->SetInputLayout(ied, ARRAYSIZE(ied), m_device);
            // m_vertexShaders[vertexShaderID]->SetActiveShader(m_deviceContext);
            // m_pixelShaders[pixelShadderID]->SetActiveShader(m_deviceContext);

            // Buffer for projection and view matrix in vertex shader Should still be moved
            // D3D11_BUFFER_DESC bd;
            // ZeroMemory(&bd, sizeof(bd));

            // bd.Usage = D3D11_USAGE_DYNAMIC;
            // bd.ByteWidth = sizeof(MatrixBufferType);
            // bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            // bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            // bd.MiscFlags = 0;
            // bd.StructureByteStride = 0;
            // ID3D11Buffer* tBuffer;
            // m_device->CreateBuffer(&bd, NULL, &tBuffer);
            // using namespace DirectX;
            // MatrixBufferType tBufferInfo;
            // tBufferInfo.view = XMMatrixTranspose(XMMatrixLookAtLH(XMLoadFloat3(&XMFLOAT3(0.0f, 0.0f, 0.0f)), XMLoadFloat3(&XMFLOAT3(0, 0, 1)),
            // XMLoadFloat3(&XMFLOAT3(0, 1, 0))));
            // tBufferInfo.projection = XMMatrixTranspose(XMMatrixPerspectiveFovLH(90 * 3.14 / 180.0f, 800.0f / 600.0f, 0.1f, 1000.0f));
            // //XMMatrixPerspectiveFovLH(45.0f, 600.0f / 800.0f, 0.1f, 100));

            // D3D11_MAPPED_SUBRESOURCE tMS;
            // m_deviceContext->Map(tBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMS);
            // memcpy(tMS.pData, &tBufferInfo, sizeof(tBufferInfo));
            // m_deviceContext->Unmap(tBuffer, NULL);
            // m_deviceContext->VSSetConstantBuffers(0, 1, &tBuffer);

            ////World Matrix, should perhaps be here....
            // bd.ByteWidth = sizeof(XMFLOAT4X4);
            // m_device->CreateBuffer(&bd, NULL, &m_worldMatrix);

            // m_deviceContext->VSSetConstantBuffers(1, 1, &m_worldMatrix);

            // For texture sampler TODOKO Should not be here!
            D3D11_SAMPLER_DESC texSamDesc;
            texSamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            texSamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
            texSamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
            texSamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
            texSamDesc.MipLODBias = 0;
            texSamDesc.MaxAnisotropy = 1;
            texSamDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
            texSamDesc.BorderColor[0] = 0.0f;
            texSamDesc.BorderColor[1] = 0.0f;
            texSamDesc.BorderColor[2] = 0.0f;
            texSamDesc.BorderColor[3] = 0.0f;
            texSamDesc.MinLOD = -3.402823466e+38F; // -FLT_MAX
            texSamDesc.MaxLOD = 3.402823466e+38F; // FLT_MAX
            // mParticleTexID = CreateTexture(L"Textures/VitPlupp.dds");
            ID3D11SamplerState* m_sampler;
            res = m_device->CreateSamplerState(&texSamDesc, &m_sampler);
            CheckHRESULT(res, "Fault when creating sampler");
            m_deviceContext->PSSetSamplers(0, 1, &m_sampler);
            m_sampler->Release();
        }
        ID3D11Device* DirectXManagerImpl::GetDevice() { return m_device; }
        ID3D11DeviceContext* DirectXManagerImpl::GetDeviceContext() { return m_deviceContext; }

        void DirectXManagerImpl::EndDraw()
        {
            m_swapChain->Present(1, 0); // TODO Evaluate if vsync should always be active
            float color[] = {0.3f, 0.0f, 0.5f, 1.0f};
            m_deviceContext->ClearRenderTargetView(m_backBuffer, color);
            m_deviceContext->ClearDepthStencilView(m_depthView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
        }
    }
}