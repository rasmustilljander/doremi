#pragma once
#include <Internal/Manager/DirectXManagerImpl.hpp>
#include <GraphicModuleContext.hpp>
#include <GraphicModuleImplementation.hpp>
#include <Internal/Manager/ComputeShaderManagerImpl.hpp>
#include <Internal/State/DepthStencilStateImpl.hpp>
#include <Internal/State/RasterizerStateImpl.hpp>
#include <Internal/Manager/LightManagerImpl.hpp>
#include <HelpFunctions.hpp>
#include <SDL2/SDL.h>
#include <Internal/Mesh/Vertex.hpp>
#include <VertexStruct.hpp>
#include <cstdint>
#include <iostream>
#include <algorithm> // std::sort

namespace DoremiEngine
{
    namespace Graphic
    {
        DirectXManagerImpl::DirectXManagerImpl(const GraphicModuleContext& p_graphicContext) : m_graphicContext(p_graphicContext)
        {
            m_screenResolution = DirectX::XMFLOAT2(800, 800);
        }
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

            if(GetActiveWindow() == nullptr)
            {
                SDL_Window* win = SDL_CreateWindow("Do-Re-Mi by Let Him Be: Interactive", 1200, 200, m_screenResolution.x, m_screenResolution.y,
                                                   SDL_WINDOW_SHOWN); // TODOKO Get height and width form reliable source
                if(!win)
                {
                    std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
                    SDL_QUIT;
                }
            }
            DXGI_SWAP_CHAIN_DESC scd;

            // clear out the struct for use
            ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

            // fill the swap chain description struct
            scd.BufferCount = 1; // one back buffer
            scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // use 32-bit color
            scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_UNORDERED_ACCESS; // how swap chain is to be used
            scd.OutputWindow = GetActiveWindow(); // the window to be used
            scd.SampleDesc.Count = 1; // how many multisamples
            scd.Windowed = TRUE; // windowed/full-screen mode

            HRESULT res = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, NULL, NULL,
                                                        D3D11_SDK_VERSION, &scd, &m_swapChain, &m_device, NULL, &m_deviceContext);
            if(!CheckHRESULT(res, "Error when creating device and swapchain, trying withour debug flag"))
            {
                res = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL, D3D11_SDK_VERSION, &scd, &m_swapChain,
                                                    &m_device, NULL, &m_deviceContext);
                CheckHRESULT(res, "Error when creating device and swapchain");
            }
            ID3D11Texture2D* t_BackBuffer;
            m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&t_BackBuffer);
            res = m_device->CreateUnorderedAccessView(t_BackBuffer, NULL, &m_backbufferUAV);
            if(FAILED(res))
            {
                int a = 3;
            }
            res = m_device->CreateRenderTargetView(t_BackBuffer, NULL, &m_backBuffer);
            if(FAILED(res))
            {
                int a = 3;
            }
            t_BackBuffer->Release();


            // Depth buffer
            // Might want this in a class for readability and easy changing between states
            D3D11_TEXTURE2D_DESC dbdesc;
            ZeroMemory(&dbdesc, sizeof(dbdesc));
            dbdesc.Width = m_screenResolution.x; // TODOKO take height and width from a reliable source
            dbdesc.Height = m_screenResolution.y;
            dbdesc.MipLevels = 1;
            dbdesc.ArraySize = 1;
            dbdesc.Format = DXGI_FORMAT_R32_TYPELESS;
            dbdesc.SampleDesc.Count = 1;
            dbdesc.SampleDesc.Quality = 0;
            dbdesc.Usage = D3D11_USAGE_DEFAULT;
            dbdesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
            dbdesc.CPUAccessFlags = 0;
            dbdesc.MiscFlags = 0;

            res = m_device->CreateTexture2D(&dbdesc, NULL, &m_depthBuffer);
            if(FAILED(res))
            {
                int a = 3;
            }

            D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
            ZeroMemory(&descDSV, sizeof(descDSV));

            descDSV.Format = DXGI_FORMAT_D32_FLOAT;
            descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
            descDSV.Texture2D.MipSlice = 0;

            res = m_device->CreateDepthStencilView(m_depthBuffer, &descDSV, &m_depthView);
            if(FAILED(res))
            {
                int a = 3;
            }

            m_srv = NULL;

            D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
            ZeroMemory(&srvd, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
            srvd.Format = DXGI_FORMAT_R32_FLOAT;
            srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            srvd.Texture2D.MipLevels = 1;


            res = m_device->CreateShaderResourceView(m_depthBuffer, &srvd, &m_srv);
            if(FAILED(res))
            {
                int a = 3;
            }

            m_deviceContext->OMSetRenderTargets(1, &m_backBuffer, m_depthView);

            // Viewport
            // TODOKO change so width and height is modifiable and taken from some smarter place
            D3D11_VIEWPORT viewport;
            ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

            viewport.TopLeftX = 0;
            viewport.TopLeftY = 0;
            viewport.Width = m_screenResolution.x;
            viewport.Height = m_screenResolution.y;
            viewport.MinDepth = 0.0f;
            viewport.MaxDepth = 1.0f;

            m_deviceContext->RSSetViewports(1, &viewport);

            // TODO add more different things like transparancy
            // TODOKO Move code to better location

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
            res = m_device->CreateSamplerState(&texSamDesc, &m_defaultSamplerState);
            CheckHRESULT(res, "Fault when creating sampler");
            m_deviceContext->PSSetSamplers(0, 1, &m_defaultSamplerState);

            BuildWorldMatrix();
        }

        void DirectXManagerImpl::SetScreenResolution(DirectX::XMFLOAT2 p_res) { m_screenResolution = p_res; }

        DirectX::XMFLOAT2 DirectXManagerImpl::GetScreenResolution() { return m_screenResolution; }

        void DirectXManagerImpl::BuildWorldMatrix()
        {
            D3D11_BUFFER_DESC bd;
            ZeroMemory(&bd, sizeof(bd));
            bd.Usage = D3D11_USAGE_DYNAMIC;
            bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            bd.MiscFlags = 0;
            bd.StructureByteStride = 0;
            bd.ByteWidth = sizeof(DirectX::XMFLOAT4X4);
            m_device->CreateBuffer(&bd, NULL, &m_worldMatrix);
        }

        ID3D11Device* DirectXManagerImpl::GetDevice() { return m_device; }
        ID3D11DeviceContext* DirectXManagerImpl::GetDeviceContext() { return m_deviceContext; }

        ID3D11SamplerState* DirectXManagerImpl::CreateSamplerState(D3D11_SAMPLER_DESC p_samplerDesc)
        {
            ID3D11SamplerState* t_sampleState;
            m_device->CreateSamplerState(&p_samplerDesc, &t_sampleState);
            return t_sampleState;
        }

        DepthStencilState* DirectXManagerImpl::CreateDepthStencilState(D3D11_DEPTH_STENCIL_DESC p_depthStencilDesc)
        {
            DepthStencilState* t_depthStencilState = new DepthStencilStateImpl();
            ID3D11DepthStencilState* t_depthState;
            m_device->CreateDepthStencilState(&p_depthStencilDesc, &t_depthState);
            t_depthStencilState->SetDepthStencilState(t_depthState);
            return t_depthStencilState;
        }
        RasterizerState* DirectXManagerImpl::CreateRasterizerState(D3D11_RASTERIZER_DESC p_rasterizerDesc)
        {
            RasterizerState* t_rasterizerState = new RasterizerStateImpl();
            ID3D11RasterizerState* t_devRasterizerState;
            m_device->CreateRasterizerState(&p_rasterizerDesc, &t_devRasterizerState);
            t_rasterizerState->SetRasterizerState(t_devRasterizerState);
            return t_rasterizerState;
        }

        void DirectXManagerImpl::DrawCurrentRenderList(ID3D11RasterizerState* p_rasterizerState, ID3D11DepthStencilState* p_depthStencilState)
        {
            m_deviceContext->OMSetDepthStencilState(p_depthStencilState, 0);
            m_deviceContext->RSSetState(p_rasterizerState);
            RenderAllMeshs();
        }

        void DirectXManagerImpl::RenderAllMeshs()
        {
            // Sort the data according after mesh then texture
            std::sort(renderData.begin(), renderData.end(), SortOnVertexThenTexture);
            // std::sort(renderData.begin(), renderData.end(), SortRenderData); //TODORT remove

            // Setup required variables
            const uint32_t stride = sizeof(Vertex);
            const uint32_t offset = 0;
            ID3D11Buffer* vertexData = renderData[0].vertexData;
            ID3D11ShaderResourceView* texture = renderData[0].texture;
            ID3D11SamplerState* samplerState = renderData[0].samplerState;

            // Iterate all the entries and do the smallest amount of changes to the GPU
            // Render the first entry outside of the loop because it's a specialcase

            D3D11_MAPPED_SUBRESOURCE tMS;
            m_deviceContext->Map(m_worldMatrix, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMS);
            memcpy(tMS.pData, &renderData[0].worldMatrix, sizeof(DirectX::XMFLOAT4X4));
            m_deviceContext->Unmap(m_worldMatrix, NULL);

            m_deviceContext->PSSetSamplers(0, 1, &samplerState);
            m_deviceContext->CSSetSamplers(0, 1, &samplerState);
            m_deviceContext->PSSetShaderResources(0, 1, &texture);
            m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            m_deviceContext->IASetVertexBuffers(0, 1, &vertexData, &stride, &offset);
            if(renderData[0].indexData != nullptr)
            {
                m_deviceContext->IASetIndexBuffer(renderData[0].indexData, DXGI_FORMAT_R32_UINT, 0);
                m_deviceContext->DrawIndexed(renderData[0].indexCount, 0, 0);
            }
            else
            {
                m_deviceContext->Draw(renderData[0].vertexCount, 0);
            }

            // TODO Can be upgraded with instanced drawing
            const size_t vectorSize = renderData.size();
            for(size_t i = 1; i < vectorSize; ++i)
            {
                if(renderData[i].vertexData != renderData[i - 1].vertexData) // Check if vertexdata has been changed
                {
                    vertexData = renderData[i].vertexData;
                    m_deviceContext->IASetVertexBuffers(0, 1, &vertexData, &stride, &offset);
                }

                if(renderData[i].samplerState != renderData[i - 1].samplerState)
                {
                    samplerState = renderData[i].samplerState;
                    if(samplerState != nullptr) // TODORT is it even required to check for null? Can this happen? Remove
                    {
                        m_deviceContext->PSSetSamplers(0, 1, &samplerState);
                    }
                    else
                    {
                        std::cout << "Something went wrong with the sampler in DirXmanagerImpl.cpp" << std::endl;
                    }
                }

                if(renderData[i].texture != renderData[i - 1].texture) // Check if texture has been changed
                {
                    texture = renderData[i].texture;
                    if(texture != nullptr) // TODORT is it even required to check for null? Can this happen? Remove
                    {
                        m_deviceContext->PSSetShaderResources(0, 1, &texture);
                    }
                }
                m_deviceContext->Map(m_worldMatrix, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMS);
                memcpy(tMS.pData, &renderData[i].worldMatrix, sizeof(DirectX::XMFLOAT4X4)); // Copy matrix to buffer
                m_deviceContext->Unmap(m_worldMatrix, NULL);

                m_deviceContext->VSSetConstantBuffers(0, 1, &m_worldMatrix);
                if(renderData[i].indexData != nullptr)
                {
                    m_deviceContext->IASetIndexBuffer(renderData[i].indexData, DXGI_FORMAT_R32_UINT, 0);
                    m_deviceContext->DrawIndexed(renderData[i].indexCount, 0, 0);
                }
                else
                {
                    m_deviceContext->Draw(renderData[i].vertexCount, 0);
                }
            }
            renderData.clear(); // Empty the vector
        }

        void DirectXManagerImpl::SwapRasterizerState(RasterizerState* p_rasterizerState)
        {
            m_deviceContext->RSSetState(p_rasterizerState->GetRasterizerState());
        }

        void DirectXManagerImpl::SwapDepthStencilState(DepthStencilState* p_depthStencilState)
        {
            m_deviceContext->OMSetDepthStencilState(p_depthStencilState->GetDepthStencilState(), 0);
        }


        void DirectXManagerImpl::EndDraw()
        {

            m_graphicContext.m_graphicModule->GetSubModuleManager().GetLightManager().TestFunc();

            ID3D11ShaderResourceView* nullSRV = {NULL};
            ID3D11UnorderedAccessView* nullUAV = {NULL};
            ID3D11RenderTargetView* nullRTV = {NULL};
            // Remove depth bind to OM
            m_deviceContext->OMSetRenderTargets(1, &m_backBuffer, nullptr);

            // m_deviceContext->CSSetUnorderedAccessViews(6, 1, &m_backbufferUAV, 0);
            m_deviceContext->CSSetShaderResources(1, 1, &m_srv);
            m_deviceContext->PSSetShaderResources(5, 1, &m_srv);

            // dispatch frustum shader
            m_graphicContext.m_graphicModule->GetSubModuleManager().GetComputeShaderManager().DispatchFrustum();
            m_graphicContext.m_graphicModule->GetSubModuleManager().GetComputeShaderManager().DispatchCulling();

            // Unbind depth
            m_deviceContext->CSSetShaderResources(1, 1, &nullSRV);
            m_deviceContext->PSSetShaderResources(5, 1, &nullSRV);
            m_deviceContext->CSSetUnorderedAccessViews(6, 1, &nullUAV, 0);

            // Add depth bind to OM
            m_deviceContext->OMSetRenderTargets(1, &m_backBuffer, m_depthView);

            m_swapChain->Present(0, 0); // TODO Evaluate if vsync should always be active
            float color[] = {0.3f, 0.0f, 0.5f, 1.0f};
            m_deviceContext->ClearRenderTargetView(m_backBuffer, color);
            m_deviceContext->ClearDepthStencilView(m_depthView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
        }

        void DirectXManagerImpl::AddMeshForRendering(MeshRenderData& p_renderData)
        {
            // TODORT Could be redesigned so that this class asks MeshManager for it's data instead
            renderData.push_back(std::move(p_renderData));
        }
    }
}