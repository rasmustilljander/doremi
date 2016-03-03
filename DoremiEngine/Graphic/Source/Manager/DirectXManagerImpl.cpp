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
#include <DoremiEditor/Core/Include/MaterialMessage.hpp>
#include <Interface/Texture/SpriteInfo.hpp>

// COnfiguration
#include <DoremiEngine/Configuration/Include/ConfigurationModule.hpp>

namespace DoremiEngine
{
    namespace Graphic
    {
        DirectXManagerImpl::DirectXManagerImpl(const GraphicModuleContext& p_graphicContext) : m_graphicContext(p_graphicContext)
        {
            float x = m_graphicContext.config.GetAllConfigurationValues().ScreenWidth;
            float y = m_graphicContext.config.GetAllConfigurationValues().ScreenHeight;
            m_screenResolution = DirectX::XMFLOAT2(x, y);
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
                m_window = SDL_CreateWindow("Do-Re-Mi by Let Him Be: Interactive", 100, 100, m_screenResolution.x, m_screenResolution.y,
                                            SDL_WINDOW_SHOWN); // TODOKO Get height and width form reliable source

                if(!m_window)
                {
                    std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
                    SDL_QUIT;
                }
            }
            else
            {
                std::cout << "Window already initialized, some features might be disabled! Please try to initialize window through graphic module" << std::endl;
                std::cout << "Will try to fetch window by ID, volotile function " << std::endl;
                m_window = SDL_GetWindowFromID(0);
                if(m_window == nullptr)
                {
                    std::cout << "Couldn't fetch window from ID, some options disabled " << std::endl;
                }
            }
            UINT t_flags = 0;
#ifdef _DEBUG
            t_flags |= D3D11_CREATE_DEVICE_DEBUG;


#endif
            D3D_FEATURE_LEVEL featureLevels[] = {
                D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0,
            };

            UINT numFeatureLevels = ARRAYSIZE(featureLevels);

            DXGI_SWAP_CHAIN_DESC scd;

            // clear out the struct for use
            ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

            // fill the swap chain description struct
            scd.BufferCount = 1; // one back buffer
            scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // use 32-bit color
            scd.BufferDesc.RefreshRate.Numerator = 60;
            scd.BufferDesc.RefreshRate.Denominator = 1;
            scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT | DXGI_USAGE_UNORDERED_ACCESS; // how swap chain is to be used
            scd.OutputWindow = GetActiveWindow(); // the window to be used
            scd.SampleDesc.Count = 1; // how many multisamples
            scd.Windowed = TRUE; // windowed/full-screen mode
            scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

            HRESULT res = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, t_flags, featureLevels, numFeatureLevels,
                                                        D3D11_SDK_VERSION, &scd, &m_swapChain, &m_device, NULL, &m_deviceContext);

            if(!CheckHRESULT(res, "Error when creating device and swapchain, trying withour debug flag"))
            {
                res = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL, D3D11_SDK_VERSION, &scd, &m_swapChain,
                                                    &m_device, NULL, &m_deviceContext);
                CheckHRESULT(res, "Error when creating device and swapchain");
            }

            ID3D11Texture2D* t_BackBuffer;
            m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&t_BackBuffer);

            res = m_device->CreateShaderResourceView(t_BackBuffer, NULL, &m_backbufferSRV);
            if(FAILED(res))
            {
                // ERROR MESSAGE
                std::cout << "Failed to create shader resource view" << std::endl;
            }
            res = m_device->CreateRenderTargetView(t_BackBuffer, NULL, &m_backBufferRTV);
            if(FAILED(res))
            {
                // ERROR MESSAGE
                std::cout << "Failed to create render target view" << std::endl;
            }
            res = m_device->CreateUnorderedAccessView(t_BackBuffer, NULL, &m_backbufferUAV);
            if(FAILED(res))
            {
                // ERROR MESSAGE
                std::cout << "Failed to create unordered access view" << std::endl;
            }
            t_BackBuffer->Release();

            D3D11_TEXTURE2D_DESC dbdesc;
            ZeroMemory(&dbdesc, sizeof(dbdesc));
            dbdesc.Width = m_screenResolution.x;
            dbdesc.Height = m_screenResolution.y;
            dbdesc.MipLevels = 1;
            dbdesc.ArraySize = 1;
            dbdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            dbdesc.SampleDesc.Count = 1;
            dbdesc.SampleDesc.Quality = 0;
            dbdesc.Usage = D3D11_USAGE_DEFAULT;
            dbdesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
            dbdesc.CPUAccessFlags = 0;
            dbdesc.MiscFlags = 0;


            res = m_device->CreateTexture2D(&dbdesc, NULL, &m_scene);
            if(FAILED(res))
            {
                // ERROR MESSAGE
                std::cout << "Failed to create texture" << std::endl;
            }
            res = m_device->CreateTexture2D(&dbdesc, NULL, &m_depth);
            if (FAILED(res))
            {
                std::cout << "Failed to create texture" << std::endl;
            }

            res = m_device->CreateTexture2D(&dbdesc, NULL, &m_glowmap);
            if(FAILED(res))
            {
                std::cout << "Failed to create texture" << std::endl;
            }
            res = m_device->CreateRenderTargetView(m_glowmap, NULL, &m_sceneRTV);
            if(FAILED(res))
            {
                std::cout << "Failed to create render target view" << std::endl;
            }
            res = m_device->CreateRenderTargetView(m_depth, NULL, &m_depthRTV);
            if (FAILED(res))
            {
                std::cout << "Failed to create render target view" << std::endl;
            }
            res = m_device->CreateUnorderedAccessView(m_glowmap, NULL, &m_glowUAV);
            if(FAILED(res))
            {
                std::cout << "Failed to create unordered access view" << std::endl;
            }
            res = m_device->CreateUnorderedAccessView(m_scene, NULL, &m_sceneUAV);
            if (FAILED(res))
            {
                std::cout << "Failed to create shader resource view" << std::endl;
            }
            res = m_device->CreateShaderResourceView(m_glowmap, NULL, &m_glowSRV);
            if(FAILED(res))
            {
                std::cout << "Failed to create shader resource view" << std::endl;
            }
            res = m_device->CreateShaderResourceView(m_scene, NULL, &m_sceneSRV);
            if(FAILED(res))
            {
                std::cout << "Failed to create shader resource view" << std::endl;
            }
            res = m_device->CreateShaderResourceView(m_depth, NULL, &m_depthSRV);
            if (FAILED(res))
            {
                std::cout << "Failed to create shader resource view" << std::endl;
            }
            res = m_device->CreateRenderTargetView(m_scene, NULL, &m_glowRTV);
            if(FAILED(res))
            {
                std::cout << "Failed to create render target view" << std::endl;
            }

            D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
            shaderResourceViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
            shaderResourceViewDesc.Texture2D.MipLevels = 1;

            // Depth buffer
            // Might want this in a class for readability and easy changing between states

            ZeroMemory(&dbdesc, sizeof(dbdesc));
            dbdesc.Width = m_screenResolution.x;
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
                std::cout << "Failed to create texture" << std::endl;
            }

            D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
            ZeroMemory(&descDSV, sizeof(descDSV));

            descDSV.Format = DXGI_FORMAT_D32_FLOAT;
            descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
            descDSV.Texture2D.MipSlice = 0;

            res = m_device->CreateDepthStencilView(m_depthBuffer, &descDSV, &m_depthView);
            if(FAILED(res))
            {
                std::cout << "Failed to create depth stencil view" << std::endl;
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
                std::cout << "Failed to create shader resource view" << std::endl;
            }

            m_deviceContext->OMSetRenderTargets(1, &m_backBufferRTV, m_depthView);

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

            // For default texture sampler
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

            BuildConstantBuffers();

            // For default rasterizer state
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
            m_defaultRasterizerState = CreateRasterizerState(rastDesc);

            // For default depth stencil state
            D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
            ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
            depthStencilDesc.DepthEnable = true;
            depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
            depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
            m_defaultDepthStencilState = CreateDepthStencilState(depthStencilDesc);

            D3D11_BLEND_DESC t_blendDesc;
            // Clear the blend state description.
            ZeroMemory(&t_blendDesc, sizeof(D3D11_BLEND_DESC));


            t_blendDesc.RenderTarget[0].BlendEnable = TRUE;
            t_blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
            t_blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
            t_blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
            t_blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
            t_blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
            t_blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
            t_blendDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;

            // Create the blend state using the description.
            res = m_device->CreateBlendState(&t_blendDesc, &m_enableBlendState);
            if (FAILED(res))
            {
                std::cout << "Failed to create blend state" << std::endl;
            }

            // Modify the description to create an alpha disabled blend state description.
            t_blendDesc.RenderTarget[0].BlendEnable = FALSE;

            // Create the blend state using the description.
            res = m_device->CreateBlendState(&t_blendDesc, &m_disableBlendState);
            if (FAILED(res))
            {
                std::cout << "Failed to create blend state" << std::endl;
            }
        }

        void DirectXManagerImpl::SetScreenResolution(DirectX::XMFLOAT2 p_res) { m_screenResolution = p_res; }

        DirectX::XMFLOAT2 DirectXManagerImpl::GetScreenResolution() { return m_screenResolution; }

        void DirectXManagerImpl::BuildConstantBuffers()
        {
            D3D11_BUFFER_DESC bd;
            ZeroMemory(&bd, sizeof(bd));
            bd.Usage = D3D11_USAGE_DYNAMIC;
            bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            bd.MiscFlags = 0;
            bd.StructureByteStride = 0;
            bd.ByteWidth = sizeof(WorldMatrices);
            m_device->CreateBuffer(&bd, NULL, &m_worldMatrix);

            bd.ByteWidth = sizeof(DoremiEditor::Core::MaterialData);
            m_device->CreateBuffer(&bd, NULL, &m_materialBuffer);


            ZeroMemory(&bd, sizeof(bd));
            bd.Usage = D3D11_USAGE_DYNAMIC;
            bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            bd.MiscFlags = 0;
            bd.StructureByteStride = 0;
            bd.ByteWidth = sizeof(SpriteData);
            HRESULT res = m_device->CreateBuffer(&bd, NULL, &m_spriteDataBuffer);
            if(!CheckHRESULT(res, "Error when creating sprite buffer"))
            {
                return;
            }
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
            HRESULT hr = m_device->CreateDepthStencilState(&p_depthStencilDesc, &t_depthState);
            if(FAILED(hr))
            {
                return nullptr;
            }
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

        void DirectXManagerImpl::RenderSprites(ID3D11RasterizerState* p_rasterizerState, ID3D11DepthStencilState* p_depthStencilState)
        {
            using namespace DoremiEditor::Core;
            m_deviceContext->OMSetDepthStencilState(p_depthStencilState, 0);
            m_deviceContext->RSSetState(p_rasterizerState);

            // Setup vertex variables
            const uint32_t stride = 0;
            const uint32_t offset = 0;
            ID3D11Buffer* vertexData = {0};

            // If nothing to draw return;
            if(spriteRenderData.size() == 0)
            {
                return;
            }

            // Setup material
            MaterialMessage material = spriteRenderData[0].materialMessage;
            ID3D11ShaderResourceView* texture = spriteRenderData[0].diffuseTexture;
            ID3D11ShaderResourceView* glowtexture = spriteRenderData[0].glowTexture;
            ID3D11SamplerState* samplerState = spriteRenderData[0].samplerState;

            // Iterate all the entries and do the smallest amount of changes to the GPU
            // Render the first entry outside of the loop because it's a specialcase

            // Update constant buffer
            D3D11_MAPPED_SUBRESOURCE tMS;
            m_deviceContext->Map(m_spriteDataBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMS);

            // Copy data
            memcpy(tMS.pData, &spriteRenderData[0].spriteData, sizeof(SpriteData));

            // Unmap
            m_deviceContext->Unmap(m_spriteDataBuffer, NULL);

            // Set GS buffer
            m_deviceContext->GSSetConstantBuffers(0, 1, &m_spriteDataBuffer);


            m_deviceContext->PSSetSamplers(0, 1, &samplerState);
            m_deviceContext->PSSetShaderResources(0, 1, &texture);
            m_deviceContext->PSSetShaderResources(5, 1, &glowtexture);
            m_deviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);


            // Update constant buffers
            m_deviceContext->Map(m_materialBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMS);
            memcpy(tMS.pData, &spriteRenderData[0].materialMessage.data, sizeof(&spriteRenderData[0].materialMessage.data));
            m_deviceContext->Unmap(m_materialBuffer, NULL);
            m_deviceContext->PSSetConstantBuffers(1, 1, &m_materialBuffer);


            // Draw first special case
            m_deviceContext->Draw(1, 0);


            // TODO Can be upgraded with instanced drawing
            const size_t vectorSize = spriteRenderData.size();
            for(size_t i = 1; i < vectorSize; ++i)
            {
                if(spriteRenderData[i].samplerState != spriteRenderData[i - 1].samplerState)
                {
                    samplerState = spriteRenderData[i].samplerState;
                    if(samplerState != nullptr) // TODORT is it even required to check for null? Can this happen? Remove
                    {
                        m_deviceContext->PSSetSamplers(0, 1, &samplerState);
                    }
                    else
                    {
                        std::cout << "Something went wrong with the sampler in DirXmanagerImpl.cpp" << std::endl;
                    }
                }

                if(spriteRenderData[i].diffuseTexture != spriteRenderData[i - 1].diffuseTexture) // Check if texture has been changed
                {
                    texture = spriteRenderData[i].diffuseTexture;
                    if(texture != nullptr) // TODORT is it even required to check for null? Can this happen? Remove
                    {
                        m_deviceContext->PSSetShaderResources(0, 1, &texture);
                    }
                }

                if(spriteRenderData[i].glowTexture != spriteRenderData[i - 1].glowTexture) // Check if texture has been changed
                {
                    glowtexture = spriteRenderData[i].glowTexture;
                    if(glowtexture != nullptr) // TODORT is it even required to check for null? Can this happen? Remove
                    {
                        m_deviceContext->PSSetShaderResources(5, 1, &glowtexture);
                    }
                }

                if(&spriteRenderData[i].materialMessage != &spriteRenderData[i - 1].materialMessage) // Check if texture has been changed
                {
                    material = spriteRenderData[i].materialMessage;
                    if(&material != nullptr) // TODORT is it even required to check for null? Can this happen? Remove
                    {
                        D3D11_MAPPED_SUBRESOURCE tMS;
                        m_deviceContext->Map(m_materialBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMS);
                        memcpy(tMS.pData, &spriteRenderData[i].materialMessage.data, sizeof(spriteRenderData[i].materialMessage.data));
                        m_deviceContext->Unmap(m_materialBuffer, NULL);

                        m_deviceContext->PSSetConstantBuffers(1, 1, &m_materialBuffer);
                    }
                }

                // Update constant buffer
                D3D11_MAPPED_SUBRESOURCE tMS;
                m_deviceContext->Map(m_spriteDataBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMS);

                // Copy data
                memcpy(tMS.pData, &spriteRenderData[i].spriteData, sizeof(SpriteData));

                // Unmap
                m_deviceContext->Unmap(m_spriteDataBuffer, NULL);

                // Set GS buffer
                m_deviceContext->GSSetConstantBuffers(0, 1, &m_spriteDataBuffer);


                // Draw
                m_deviceContext->Draw(1, 0);
            }
            spriteRenderData.clear(); // Empty the vector
        }

        void DirectXManagerImpl::Render2D(ID3D11RasterizerState* p_rasterizerState, ID3D11DepthStencilState* p_depthStencilState)
        {
            using namespace DoremiEditor::Core;
            m_deviceContext->OMSetDepthStencilState(p_depthStencilState, 0);
            m_deviceContext->RSSetState(p_rasterizerState);

            // Sort the data according after mesh then texture
            std::sort(renderData.begin(), renderData.end(), SortOnVertexThenTexture);
            // std::sort(renderData.begin(), renderData.end(), SortRenderData); //TODORT remove

            // Setup required variables
            const uint32_t stride = sizeof(Vertex);
            const uint32_t offset = 0;
            ID3D11Buffer* vertexData = renderData[0].vertexData;
            MaterialMessage material = renderData[0].materialMessage;
            ID3D11ShaderResourceView* texture = renderData[0].diffuseTexture;
            ID3D11ShaderResourceView* glowtexture = renderData[0].glowTexture;
            ID3D11SamplerState* samplerState = renderData[0].samplerState;

            // Iterate all the entries and do the smallest amount of changes to the GPU
            // Render the first entry outside of the loop because it's a specialcase

            D3D11_MAPPED_SUBRESOURCE tMS;
            m_deviceContext->Map(m_worldMatrix, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMS);
            memcpy(tMS.pData, &renderData[0].worldMatrix, sizeof(DirectX::XMFLOAT4X4));
            m_deviceContext->Unmap(m_worldMatrix, NULL);

            m_deviceContext->PSSetSamplers(0, 1, &samplerState);
            m_deviceContext->PSSetShaderResources(0, 1, &texture);
            m_deviceContext->PSSetShaderResources(5, 1, &glowtexture);
            m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            m_deviceContext->IASetVertexBuffers(0, 1, &vertexData, &stride, &offset);
            m_deviceContext->VSSetConstantBuffers(0, 1, &m_worldMatrix);

            m_deviceContext->Map(m_materialBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMS);
            memcpy(tMS.pData, &renderData[0].materialMessage.data, sizeof(&renderData[0].materialMessage.data));
            m_deviceContext->Unmap(m_materialBuffer, NULL);
            m_deviceContext->PSSetConstantBuffers(1, 1, &m_materialBuffer);

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

                if(renderData[i].diffuseTexture != renderData[i - 1].diffuseTexture) // Check if texture has been changed
                {
                    texture = renderData[i].diffuseTexture;
                    if(texture != nullptr) // TODORT is it even required to check for null? Can this happen? Remove
                    {
                        m_deviceContext->PSSetShaderResources(0, 1, &texture);
                    }
                }
                if(renderData[i].glowTexture != renderData[i - 1].glowTexture) // Check if texture has been changed
                {
                    glowtexture = renderData[i].glowTexture;
                    if(glowtexture != nullptr) // TODORT is it even required to check for null? Can this happen? Remove
                    {
                        m_deviceContext->PSSetShaderResources(5, 1, &glowtexture);
                    }
                }

                if(&renderData[i].materialMessage != &renderData[i - 1].materialMessage) // Check if texture has been changed
                {
                    material = renderData[i].materialMessage;
                    if(&material != nullptr) // TODORT is it even required to check for null? Can this happen? Remove
                    {
                        D3D11_MAPPED_SUBRESOURCE tMS;
                        m_deviceContext->Map(m_materialBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMS);
                        memcpy(tMS.pData, &renderData[i].materialMessage.data, sizeof(renderData[i].materialMessage.data));
                        m_deviceContext->Unmap(m_materialBuffer, NULL);

                        m_deviceContext->PSSetConstantBuffers(1, 1, &m_materialBuffer);
                    }
                }

                m_deviceContext->Map(m_worldMatrix, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMS);

                WorldMatrixPair t_worldMatrices;

                DirectX::XMMATRIX t_mat = DirectX::XMLoadFloat4x4(&renderData[i].worldMatrix);
                DirectX::XMVECTOR t_det = DirectX::XMMatrixDeterminant(t_mat);
                t_mat = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(&t_det, t_mat));
                DirectX::XMStoreFloat4x4(&t_worldMatrices.invTransWorldMat, t_mat);
                t_worldMatrices.worldMat = renderData[i].worldMatrix;
                memcpy(tMS.pData, &t_worldMatrices, sizeof(WorldMatrixPair)); // Copy matrix to buffer
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

        void DirectXManagerImpl::DrawCurrentRenderListSkeletal(ID3D11RasterizerState* p_rasterizerState, ID3D11DepthStencilState* p_depthStencilState)
        {
            m_deviceContext->OMSetDepthStencilState(p_depthStencilState, 0);
            m_deviceContext->RSSetState(p_rasterizerState);
            RenderSkeletalMesh();
        }

        void DirectXManagerImpl::RenderAllMeshs()
        {
            using namespace DoremiEditor::Core;
            using namespace DirectX;
            // Sort the data according after mesh then texture, only mesh though
            std::sort(renderData.begin(), renderData.end(), SortOnVertexThenTexture);

            // Nothing to draw
            if(renderData.size() == 0)
            {
                return;
            }

            // Setup required variables
            const uint32_t stride = sizeof(Vertex);
            const uint32_t offset = 0;
            ID3D11Buffer* vertexData = renderData[0].vertexData;
            MaterialMessage materialData = renderData[0].materialMessage;
            ID3D11ShaderResourceView* texture = renderData[0].diffuseTexture;
            ID3D11ShaderResourceView* glowtexture = renderData[0].glowTexture;
            ID3D11SamplerState* samplerState = renderData[0].samplerState;

            m_deviceContext->PSSetSamplers(0, 1, &samplerState);
            m_deviceContext->PSSetShaderResources(0, 1, &texture);
            m_deviceContext->PSSetShaderResources(5, 1, &glowtexture);
            m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            m_deviceContext->IASetVertexBuffers(0, 1, &vertexData, &stride, &offset);
            m_deviceContext->VSSetConstantBuffers(0, 1, &m_worldMatrix);

            D3D11_MAPPED_SUBRESOURCE tMS;
            m_deviceContext->Map(m_materialBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMS);
            memcpy(tMS.pData, &renderData[0].materialMessage.data, sizeof(&renderData[0].materialMessage.data));
            m_deviceContext->Unmap(m_materialBuffer, NULL);
            m_deviceContext->PSSetConstantBuffers(1, 1, &m_materialBuffer);

            // TODO Can be upgraded with instanced drawing
            // sista saken som ska renderas kanske inte renderas nu...

            const size_t vectorSize = renderData.size();
            size_t i = 0;
            for(; i < vectorSize - 1;)
            {
                std::vector<WorldMatrixPair> worldMatrices;
                size_t j = i;
                for(; j < vectorSize - 1; j++)
                {
                    WorldMatrixPair newPair;
                    newPair.worldMat = renderData[j].worldMatrix;
                    DirectX::XMMATRIX t_mat = DirectX::XMLoadFloat4x4(&renderData[j].worldMatrix);
                    t_mat = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, t_mat));
                    DirectX::XMStoreFloat4x4(&newPair.invTransWorldMat, t_mat);
                    worldMatrices.push_back(newPair);

                    // Check if anything have changed or if we are above max instances
                    // Really horrible way of checking if mat changed
                    if(renderData[j].vertexData != renderData[j + 1].vertexData || renderData[j].samplerState != renderData[j + 1].samplerState ||
                       renderData[j].diffuseTexture != renderData[j + 1].diffuseTexture || renderData[j].glowTexture != renderData[j + 1].glowTexture ||
                       renderData[j].materialMessage.data.color[0] != renderData[j + 1].materialMessage.data.color[0] ||
                       renderData[j].materialMessage.data.color[1] != renderData[j + 1].materialMessage.data.color[1] ||
                       renderData[j].materialMessage.data.color[2] != renderData[j + 1].materialMessage.data.color[2] || worldMatrices.size() >= MAX_NUMBER_OF_INSTANCES)
                    {
                        j++;
                        break;
                    }
                }

                m_deviceContext->Map(m_worldMatrix, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMS);
                memcpy(tMS.pData, worldMatrices.data(), sizeof(WorldMatrixPair) * worldMatrices.size()); // Copy matrix to buffer
                m_deviceContext->Unmap(m_worldMatrix, NULL);

                // Set the material to j-1
                materialData = renderData[j - 1].materialMessage;
                if(&materialData != nullptr) // TODORT is it even required to check for null? Can this happen? Remove
                {
                    D3D11_MAPPED_SUBRESOURCE tMS;
                    m_deviceContext->Map(m_materialBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMS);
                    memcpy(tMS.pData, &renderData[j - 1].materialMessage.data, sizeof(renderData[j - 1].materialMessage.data));
                    m_deviceContext->Unmap(m_materialBuffer, NULL);

                    m_deviceContext->PSSetConstantBuffers(1, 1, &m_materialBuffer);
                }

                if(renderData[i].indexData != nullptr)
                {
                    m_deviceContext->IASetIndexBuffer(renderData[i].indexData, DXGI_FORMAT_R32_UINT, 0);
                    m_deviceContext->DrawIndexedInstanced(renderData[i].indexCount, worldMatrices.size(), 0, 0, 0);
                }
                else
                {
                    m_deviceContext->DrawInstanced(renderData[i].vertexCount, worldMatrices.size(), 0, 0);
                }

                i = j;
                vertexData = renderData[j].vertexData;
                m_deviceContext->IASetVertexBuffers(0, 1, &vertexData, &stride, &offset);

                // Check what needs changing!
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

                if(renderData[i].diffuseTexture != renderData[i - 1].diffuseTexture) // Check if texture has been changed
                {
                    texture = renderData[i].diffuseTexture;
                    if(texture != nullptr) // TODORT is it even required to check for null? Can this happen? Remove
                    {
                        m_deviceContext->PSSetShaderResources(0, 1, &texture);
                    }
                }
                if(renderData[i].glowTexture != renderData[i - 1].glowTexture) // Check if texture has been changed
                {
                    glowtexture = renderData[i].glowTexture;
                    if(glowtexture != nullptr) // TODORT is it even required to check for null? Can this happen? Remove
                    {
                        m_deviceContext->PSSetShaderResources(5, 1, &glowtexture);
                    }
                }
            }

            // Draw the last one since it's a special case
            WorldMatrixPair newPair;
            newPair.worldMat = renderData[i].worldMatrix;
            DirectX::XMMATRIX t_mat = DirectX::XMLoadFloat4x4(&renderData[i].worldMatrix);
            t_mat = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, t_mat));
            DirectX::XMStoreFloat4x4(&newPair.invTransWorldMat, t_mat);

            m_deviceContext->Map(m_worldMatrix, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMS);
            memcpy(tMS.pData, &newPair, sizeof(WorldMatrixPair)); // Copy matrix to buffer
            m_deviceContext->Unmap(m_worldMatrix, NULL);

            m_deviceContext->VSSetConstantBuffers(0, 1, &m_worldMatrix);

            if(renderData[i].indexData != nullptr)
            {
                m_deviceContext->IASetIndexBuffer(renderData[i].indexData, DXGI_FORMAT_R32_UINT, 0);
                m_deviceContext->DrawIndexedInstanced(renderData[i].indexCount, 1, 0, 0, 0);
            }
            else
            {
                m_deviceContext->DrawInstanced(renderData[i].vertexCount, 1, 0, 0);
            }

            renderData.clear(); // Empty the vector
        }

        void DirectXManagerImpl::RenderTransMeshs()
        {
            using namespace DoremiEditor::Core;
            // Sort the data according after mesh then texture
            std::sort(transRenderData.begin(), transRenderData.end(), SortOnVertexThenTexture);
            // std::sort(transRenderData.begin(), transRenderData.end(), SortRenderData); //TODORT remove

            // Setup required variables
            const uint32_t stride = sizeof(Vertex);
            const uint32_t offset = 0;
            ID3D11Buffer* vertexData = transRenderData[0].vertexData;
            MaterialMessage materialData = transRenderData[0].materialMessage;
            ID3D11ShaderResourceView* texture = transRenderData[0].diffuseTexture;
            ID3D11ShaderResourceView* glowtexture = transRenderData[0].glowTexture;
            ID3D11SamplerState* samplerState = transRenderData[0].samplerState;

            // Iterate all the entries and do the smallest amount of changes to the GPU
            // Render the first entry outside of the loop because it's a specialcase

            D3D11_MAPPED_SUBRESOURCE tMS;
            m_deviceContext->Map(m_worldMatrix, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMS);
            memcpy(tMS.pData, &transRenderData[0].worldMatrix, sizeof(DirectX::XMFLOAT4X4));
            m_deviceContext->Unmap(m_worldMatrix, NULL);

            m_deviceContext->PSSetSamplers(0, 1, &samplerState);
            m_deviceContext->PSSetShaderResources(0, 1, &texture);
            m_deviceContext->PSSetShaderResources(5, 1, &glowtexture);
            m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            m_deviceContext->IASetVertexBuffers(0, 1, &vertexData, &stride, &offset);
            m_deviceContext->VSSetConstantBuffers(0, 1, &m_worldMatrix);

            m_deviceContext->Map(m_materialBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMS);
            memcpy(tMS.pData, &transRenderData[0].materialMessage.data, sizeof(&transRenderData[0].materialMessage.data));
            m_deviceContext->Unmap(m_materialBuffer, NULL);
            m_deviceContext->PSSetConstantBuffers(1, 1, &m_materialBuffer);


            if(transRenderData[0].indexData != nullptr)
            {
                m_deviceContext->IASetIndexBuffer(transRenderData[0].indexData, DXGI_FORMAT_R32_UINT, 0);
                m_deviceContext->DrawIndexed(transRenderData[0].indexCount, 0, 0);
            }
            else
            {
                m_deviceContext->Draw(transRenderData[0].vertexCount, 0);
            }

            // TODO Can be upgraded with instanced drawing
            const size_t vectorSize = transRenderData.size();
            for(size_t i = 1; i < vectorSize; ++i)
            {
                if(transRenderData[i].vertexData != transRenderData[i - 1].vertexData) // Check if vertexdata has been changed
                {
                    vertexData = transRenderData[i].vertexData;
                    m_deviceContext->IASetVertexBuffers(0, 1, &vertexData, &stride, &offset);
                }

                if(transRenderData[i].samplerState != transRenderData[i - 1].samplerState)
                {
                    samplerState = transRenderData[i].samplerState;
                    if(samplerState != nullptr) // TODORT is it even required to check for null? Can this happen? Remove
                    {
                        m_deviceContext->PSSetSamplers(0, 1, &samplerState);
                    }
                    else
                    {
                        std::cout << "Something went wrong with the sampler in DirXmanagerImpl.cpp" << std::endl;
                    }
                }

                if(transRenderData[i].diffuseTexture != transRenderData[i - 1].diffuseTexture) // Check if texture has been changed
                {
                    texture = transRenderData[i].diffuseTexture;
                    if(texture != nullptr) // TODORT is it even required to check for null? Can this happen? Remove
                    {
                        m_deviceContext->PSSetShaderResources(0, 1, &texture);
                    }
                }
                if(transRenderData[i].glowTexture != transRenderData[i - 1].glowTexture) // Check if texture has been changed
                {
                    glowtexture = transRenderData[i].glowTexture;
                    if(glowtexture != nullptr) // TODORT is it even required to check for null? Can this happen? Remove
                    {
                        m_deviceContext->PSSetShaderResources(5, 1, &glowtexture);
                    }
                }
                // if (&transRenderData[i].materialMessage != &transRenderData[i - 1].materialMessage) // Check if material has been changed
                {
                    materialData = transRenderData[i].materialMessage;
                    if(&materialData != nullptr) // TODORT is it even required to check for null? Can this happen? Remove
                    {
                        // Just for the luls
                        // float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
                        // float g = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
                        // float b = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
                        //
                        // transRenderData[i].materialMessage.SetColor(r, g, b);

                        D3D11_MAPPED_SUBRESOURCE tMS;
                        m_deviceContext->Map(m_materialBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMS);
                        memcpy(tMS.pData, &transRenderData[i].materialMessage.data, sizeof(transRenderData[i].materialMessage.data));
                        m_deviceContext->Unmap(m_materialBuffer, NULL);

                        m_deviceContext->PSSetConstantBuffers(1, 1, &m_materialBuffer);
                    }
                }

                m_deviceContext->Map(m_worldMatrix, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMS);

                WorldMatrixPair t_worldMatrices;

                DirectX::XMMATRIX t_mat = DirectX::XMLoadFloat4x4(&transRenderData[i].worldMatrix);
                DirectX::XMVECTOR t_det = DirectX::XMMatrixDeterminant(t_mat);
                t_mat = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(&t_det, t_mat));
                DirectX::XMStoreFloat4x4(&t_worldMatrices.invTransWorldMat, t_mat);
                t_worldMatrices.worldMat = transRenderData[i].worldMatrix;
                memcpy(tMS.pData, &t_worldMatrices, sizeof(WorldMatrixPair)); // Copy matrix to buffer
                m_deviceContext->Unmap(m_worldMatrix, NULL);

                m_deviceContext->VSSetConstantBuffers(0, 1, &m_worldMatrix);
                if(transRenderData[i].indexData != nullptr)
                {
                    m_deviceContext->IASetIndexBuffer(transRenderData[i].indexData, DXGI_FORMAT_R32_UINT, 0);
                    m_deviceContext->DrawIndexed(transRenderData[i].indexCount, 0, 0);
                }
                else
                {
                    m_deviceContext->Draw(transRenderData[i].vertexCount, 0);
                }
            }
            transRenderData.clear(); // Empty the vector
        }

        void DirectXManagerImpl::RenderSkeletalMesh()
        {
            // Sort the data according after mesh then texture
            std::sort(renderData.begin(), renderData.end(), SortOnVertexThenTexture);
            // std::sort(renderData.begin(), renderData.end(), SortRenderData); //TODORT remove

            // Setup required variables
            const uint32_t stride = sizeof(SkeletalVertex);
            const uint32_t offset = 0;
            ID3D11Buffer* vertexData = renderData[0].vertexData;
            ID3D11ShaderResourceView* texture = renderData[0].diffuseTexture;
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
            m_deviceContext->VSSetConstantBuffers(0, 1, &m_worldMatrix);
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

                if(renderData[i].diffuseTexture != renderData[i - 1].diffuseTexture) // Check if texture has been changed
                {
                    texture = renderData[i].diffuseTexture;
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


        void DirectXManagerImpl::DispatchCompute()
        {
            ID3D11ShaderResourceView* nullSRV = {NULL};
            ID3D11UnorderedAccessView* nullUAV = {NULL};
            ID3D11RenderTargetView* nullRTV[2] = {NULL, NULL};
            // Remove depth bind to OM
            m_deviceContext->OMSetRenderTargets(2, nullRTV, nullptr); // switch between &m_backBuffer and &nullRTV

            m_deviceContext->CSSetUnorderedAccessViews(6, 1, &m_backbufferUAV, 0); // Remove to render normally
            m_deviceContext->CSSetShaderResources(1, 1, &m_depthSRV);

            // dispatch frustum shader
            m_graphicContext.m_graphicModule->GetSubModuleManager().GetComputeShaderManager().DispatchFrustum();
            m_graphicContext.m_graphicModule->GetSubModuleManager().GetComputeShaderManager().DispatchCulling();

            // Unbind depth
            m_deviceContext->CSSetShaderResources(1, 1, &nullSRV);
            m_deviceContext->CSSetUnorderedAccessViews(6, 1, &nullUAV, 0);

            SetRenderTargetGlow();
        }

        void DirectXManagerImpl::SwapRasterizerState(RasterizerState* p_rasterizerState)
        {
            m_deviceContext->RSSetState(p_rasterizerState->GetRasterizerState());
        }

        void DirectXManagerImpl::SwapDepthStencilState(DepthStencilState* p_depthStencilState)
        {
            m_deviceContext->OMSetDepthStencilState(p_depthStencilState->GetDepthStencilState(), 0);
        }

        void DirectXManagerImpl::SetRenderTargetNormal()
        {
            // Bind render target to backbuffer again

            ID3D11RenderTargetView* nullRTV[3] = {NULL, NULL, NULL};
            m_deviceContext->OMSetRenderTargets(3, nullRTV, m_depthView);
            m_deviceContext->OMSetRenderTargets(1, &m_backBufferRTV, m_depthView);
        }

        void DirectXManagerImpl::SetRenderTargetGlow()
        {
            // Add depth bind to OM
            ID3D11RenderTargetView* t_RTArray[3] = { m_glowRTV, m_backBufferRTV, m_depthRTV };
            m_deviceContext->OMSetRenderTargets(3, t_RTArray, m_depthView);
        }

        void DirectXManagerImpl::EnableBlend()
        {
            float blendFactor[4];

            // Setup the blend factor.
            blendFactor[0] = 1.0f;
            blendFactor[1] = 1.0f;
            blendFactor[2] = 1.0f;
            blendFactor[3] = 1.0f;

            // Turn on the alpha blending.
            m_deviceContext->OMSetBlendState(m_enableBlendState, blendFactor, 0xffffffff);
        }

        void DirectXManagerImpl::DisableBlend()
        {
            float blendFactor[4];

            // Setup the blend factor.
            blendFactor[0] = 0.0f;
            blendFactor[1] = 0.0f;
            blendFactor[2] = 0.0f;
            blendFactor[3] = 0.0f;

            // Turn off the alpha blending.
            m_deviceContext->OMSetBlendState(m_disableBlendState, blendFactor, 0xffffffff);
        }

        void DirectXManagerImpl::ComputeGlow()
        {
            ID3D11ShaderResourceView* nullSRV = {NULL};
            ID3D11UnorderedAccessView* nullUAV = {NULL};
            ID3D11RenderTargetView* nullRTV[3] = {NULL, NULL, NULL};

            m_deviceContext->OMSetRenderTargets(3, nullRTV, nullptr);

            //////BLUR HORIZONAL
            m_deviceContext->CSSetShaderResources(0, 1, &m_backbufferSRV);
            m_deviceContext->CSSetUnorderedAccessViews(0, 1, &m_glowUAV, 0);

            m_graphicContext.m_graphicModule->GetSubModuleManager().GetComputeShaderManager().DispatchBlurHorizontal();

            m_deviceContext->CSSetShaderResources(0, 1, &nullSRV);
            m_deviceContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, 0);

            /////////BLUR VERTICAL

            m_deviceContext->CSSetShaderResources(0, 1, &m_glowSRV);
            m_deviceContext->CSSetShaderResources(1, 1, &m_sceneSRV);

            // uav   m_glowUAV
            m_deviceContext->CSSetUnorderedAccessViews(0, 1, &m_backbufferUAV, 0);
            // dispatch
            m_graphicContext.m_graphicModule->GetSubModuleManager().GetComputeShaderManager().DispatchBlurVertical();
            // clear srv and uav
            m_deviceContext->CSSetShaderResources(0, 1, &nullSRV);
            m_deviceContext->CSSetShaderResources(1, 1, &nullSRV);
            m_deviceContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, 0);

        }

        void DirectXManagerImpl::BeginDraw()
        {
            DispatchCompute();

        }

        void DirectXManagerImpl::EndDraw()
        {

            if(transRenderData.size() > 0)
            {
                SetRenderTargetNormal();
                EnableBlend();
                RenderTransMeshs();
                DisableBlend();
            }
            ComputeGlow();

            m_swapChain->Present(0, 0); // TODO Evaluate if vsync should always be active
            float color[] = {0.0f, 0.0f, 0.0f, 1.0f};
            m_deviceContext->ClearRenderTargetView(m_backBufferRTV, color);
            m_deviceContext->ClearRenderTargetView(m_sceneRTV, color);
            m_deviceContext->ClearRenderTargetView(m_glowRTV, color);
            m_deviceContext->ClearUnorderedAccessViewFloat(m_backbufferUAV, color);
            m_deviceContext->ClearDepthStencilView(m_depthView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
        }

        void DirectXManagerImpl::AddMeshForRendering(MeshRenderData& p_renderData)
        {
            // TODORT Could be redesigned so that this class asks MeshManager for it's data instead
            renderData.push_back(std::move(p_renderData));
        }
        void DirectXManagerImpl::AddTransMeshForRendering(MeshRenderData& p_renderData)
        {
            // TODORT Could be redesigned so that this class asks MeshManager for it's data instead
            transRenderData.push_back(std::move(p_renderData));
        }

        void DirectXManagerImpl::AddSpriteForRendering(SpriteRenderData& p_spriteRenderData) { spriteRenderData.push_back(p_spriteRenderData); }


        void DirectXManagerImpl::SetFullscreen(const bool& p_fullscreen)
        {
            if(m_window == nullptr)
            {
                // No window for some strange reason, log error
                std::cout << "No active sdl window when changing to fullscreen" << std::endl;
                return;
            }
            if(p_fullscreen)
            {

                SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN);
            }
            else
            {
                SDL_SetWindowFullscreen(m_window, 0);
            }
        }
    }
}