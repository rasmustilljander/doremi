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
#include <DoremiEngine/Graphic/Include/Internal/Manager/MeshManagerImpl.hpp>
#include <DoremiEngine/Graphic/Include/Internal/Mesh/MaterialInfoImpl.hpp>
#include <DoremiEngine/Graphic/Include/Loaders/ModelLoader.hpp>

// COnfiguration
#include <DoremiEngine/Configuration/Include/ConfigurationModule.hpp>

#define NUM_BACK_BUFFERS 2

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

            InitializeDisplayModes();

            DoremiEngine::Configuration::ConfiguartionInfo configInfo = m_graphicContext.config.GetAllConfigurationValues();
            m_isFullscreen = static_cast<bool>(configInfo.Fullscreen);

            if(GetActiveWindow() == nullptr)
            {
                Uint32 t_flags = SDL_WINDOW_SHOWN;
                if(m_isFullscreen)
                {
                    t_flags |= SDL_WINDOW_FULLSCREEN;
                }
                // m_maxResolution.x - m_screenResolution.x/2.0f , m_maxResolution.y - m_screenResolution.y / 2.0f,
                m_window = SDL_CreateWindow("Do-Re-Mi by Let Him Be: Interactive", 10, 10, m_screenResolution.x, m_screenResolution.y,
                                            t_flags); // TODOKO Get height and width form reliable source

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

            const std::string pathToIcon = m_graphicContext.m_workingDirectory + std::string("\Icons\\ArgeeNBeats.bmp");
            SDL_Surface* surface = SDL_LoadBMP(pathToIcon.c_str());
            if(surface == nullptr)
            {
                std::cout << "Could not find icon at path: " << pathToIcon << std::endl;
            }
            else
            {
                SDL_SetWindowIcon(m_window, surface);
            }


            // Device flags
            UINT t_flags = 0;
#ifdef _DEBUG
// t_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
            t_flags |= D3D11_CREATE_DEVICE_SINGLETHREADED;

            D3D_FEATURE_LEVEL featureLevels[] = {
                D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0,
            };

            UINT numFeatureLevels = ARRAYSIZE(featureLevels);

            DXGI_SWAP_CHAIN_DESC scd;

            // clear out the struct for use
            ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

            // fill the swap chain description struct
            scd.BufferCount = NUM_BACK_BUFFERS; // one back buffer
            scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // use 32-bit color
            scd.BufferDesc.RefreshRate.Numerator = m_refreshRate;
            scd.BufferDesc.RefreshRate.Denominator = 1;
            scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT | DXGI_USAGE_UNORDERED_ACCESS; // how swap chain is to be used
            scd.OutputWindow = GetActiveWindow(); // the window to be used
            scd.SampleDesc.Count = 1; // how many multisamples
            scd.Windowed = static_cast<int>(!m_isFullscreen); // windowed/full-screen mode
            scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

            HRESULT res = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, t_flags, featureLevels, numFeatureLevels,
                                                        D3D11_SDK_VERSION, &scd, &m_swapChain, &m_device, NULL, &m_deviceContext);

            if(!CheckHRESULT(res, "Error when creating device and swapchain."))
            {
                return;
            }

            CreateBackBufferViews();

            CreateBlurrBuffers();

            CreateDepthViews();

            CreateColorBuffer();

            CreateRealDepthBuffer();


            m_deviceContext->OMSetRenderTargets(1, &m_colorRTV, m_depthView);

            CreateViewport();

            CreateSamplers();

            BuildConstantBuffers();

            CreateRasterizerStates();

            CreateBlendStates();

            t_frustrumComputed = false;

            std::string fileLocation = m_graphicContext.m_workingDirectory + "Textures/" + "Skybox.dds";
            ID3D11ShaderResourceView* t_resouceView = ModelLoader::GetInstance()->LoadTexture(fileLocation, m_device);
            if(t_resouceView != nullptr)
            {

                m_deviceContext->PSSetShaderResources(6, 1, &t_resouceView);
            }
        }

        void DirectXManagerImpl::InitializeDisplayModes()
        {
            // Get resolutions
            UINT t_numDispalys = SDL_GetNumVideoDisplays();

            UINT t_numDisplayModes = SDL_GetNumDisplayModes(0);
            m_maxResolution.x = 0;
            m_maxResolution.y = 0;

            // For each display
            for(size_t dispIndex = 0; dispIndex < t_numDispalys; dispIndex++)
            {
                // For each mode
                for(size_t modeIndex = 0; modeIndex < t_numDisplayModes; modeIndex++)
                {
                    SDL_DisplayMode mode = {SDL_PIXELFORMAT_UNKNOWN, 0, 0, 0, 0};

                    SDL_GetDisplayMode(dispIndex, modeIndex, &mode);

                    std::pair<uint32_t, uint32_t> m_resolution;
                    m_resolution.first = mode.w;
                    m_resolution.second = mode.h;

                    // Hard coded only disp 1, should check config if we can put on another screen
                    if(m_maxResolution.x < m_resolution.first && dispIndex == 0)
                    {
                        m_maxResolution.x = m_resolution.first;
                        m_maxResolution.y = m_resolution.second;
                    }

                    // If we don't already have refreshrate saved add, or nothing saved add
                    if(m_displayModes[dispIndex][m_resolution].size() && m_displayModes[dispIndex][m_resolution].back() != mode.refresh_rate)
                    {
                        m_displayModes[dispIndex][m_resolution].push_back(mode.refresh_rate);
                    }
                    else
                    {
                        m_displayModes[dispIndex][m_resolution].push_back(mode.refresh_rate);
                    }
                }
            }


            float x = m_graphicContext.config.GetAllConfigurationValues().ScreenWidth;
            float y = m_graphicContext.config.GetAllConfigurationValues().ScreenHeight;

            // TODOCM read config values as refreshrate and monitor here

            if(m_displayModes.count(0))
            {
                std::pair<uint32_t, uint32_t> t_resolutionToCheck;
                t_resolutionToCheck.first = static_cast<uint32_t>(x);
                t_resolutionToCheck.second = static_cast<uint32_t>(y);

                if(m_displayModes[0].count(t_resolutionToCheck))
                {
                    // It's ok, we can keep the resolution
                }
                else
                {
                    // Change to proper resolution
                    x = (m_displayModes[0].rbegin())->first.first;
                    y = (m_displayModes[0].rbegin())->first.second;
                }
            }
            else
            {
                // Well... no displays..
            }

            m_screenResolution = DirectX::XMFLOAT2(x, y);
            m_refreshRate = 60;
            m_currentMonitor = 0;
        }

        void DirectXManagerImpl::CreateBackBufferViews()
        {
            ID3D11Texture2D* t_BackBuffer;
            m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&t_BackBuffer);

            HRESULT res = m_device->CreateUnorderedAccessView(t_BackBuffer, NULL, &m_backbufferUAV);
            if(FAILED(res))
            {
                // ERROR MESSAGE
                std::cout << "Failed to create unordered access view" << std::endl;
            }
            t_BackBuffer->Release();
        }

        void DirectXManagerImpl::CreateBlurrBuffers()
        {
            ID3D11Texture2D* t_glowmap;
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
            dbdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
            dbdesc.CPUAccessFlags = 0;
            dbdesc.MiscFlags = 0;

            D3D11_SHADER_RESOURCE_VIEW_DESC t_SrvDesc;
            t_SrvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            t_SrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            t_SrvDesc.Texture2D.MostDetailedMip = 0;
            t_SrvDesc.Texture2D.MipLevels = 1;

            // Create texture for RTV
            HRESULT res = m_device->CreateTexture2D(&dbdesc, NULL, &t_glowmap);
            if(FAILED(res))
            {
                // ERROR MESSAGE
                std::cout << "Failed to create texture" << std::endl;
            }

            // Create RTV view
            res = m_device->CreateRenderTargetView(t_glowmap, nullptr, &m_horGlowRTV);
            if(FAILED(res))
            {
                // ERROR MESSAGE
                std::cout << "Failed to create rtv of glow" << std::endl;
            }

            // Create SRV view
            res = m_device->CreateShaderResourceView(t_glowmap, &t_SrvDesc, &m_horGlowSRV);
            if(FAILED(res))
            {
                // ERROR MESSAGE
                std::cout << "Failed to create srv of glow" << std::endl;
            }
            t_glowmap->Release();
            t_glowmap = nullptr;

            // Create the other texture
            dbdesc.Width = std::ceil(m_screenResolution.x / 2.0f);
            dbdesc.Height = std::ceil(m_screenResolution.y / 2.0f);
            dbdesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;

            // Create texture for RTV
            res = m_device->CreateTexture2D(&dbdesc, NULL, &t_glowmap);
            if(FAILED(res))
            {
                // ERROR MESSAGE
                std::cout << "Failed to create texture" << std::endl;
            }

            // Create UAV view
            res = m_device->CreateUnorderedAccessView(t_glowmap, nullptr, &m_vertGlowUAV);
            if(FAILED(res))
            {
                // ERROR MESSAGE
                std::cout << "Failed to create rtv of glow" << std::endl;
            }

            // Create SRV view
            res = m_device->CreateShaderResourceView(t_glowmap, &t_SrvDesc, &m_vertGlowSRV);
            if(FAILED(res))
            {
                // ERROR MESSAGE
                std::cout << "Failed to create srv of glow" << std::endl;
            }
            t_glowmap->Release();
        }

        void DirectXManagerImpl::CreateColorBuffer()
        {
            ID3D11Texture2D* t_colorBuffer;
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
            dbdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
            dbdesc.CPUAccessFlags = 0;
            dbdesc.MiscFlags = 0;

            D3D11_SHADER_RESOURCE_VIEW_DESC t_SrvDesc;
            t_SrvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            t_SrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            t_SrvDesc.Texture2D.MostDetailedMip = 0;
            t_SrvDesc.Texture2D.MipLevels = 1;

            // Create texture for RTV
            HRESULT res = m_device->CreateTexture2D(&dbdesc, NULL, &t_colorBuffer);
            if(FAILED(res))
            {
                // ERROR MESSAGE
                std::cout << "Failed to create texture" << std::endl;
            }

            // Create RTV view
            res = m_device->CreateRenderTargetView(t_colorBuffer, nullptr, &m_colorRTV);
            if(FAILED(res))
            {
                // ERROR MESSAGE
                std::cout << "Failed to create rtv of glow" << std::endl;
            }

            // Create SRV view
            res = m_device->CreateShaderResourceView(t_colorBuffer, &t_SrvDesc, &m_colorSRV);
            if(FAILED(res))
            {
                // ERROR MESSAGE
                std::cout << "Failed to create srv of glow" << std::endl;
            }
            t_colorBuffer->Release();
        }

        void DirectXManagerImpl::CreateDepthViews()
        {
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
            dbdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
            dbdesc.CPUAccessFlags = 0;
            dbdesc.MiscFlags = 0;

            // Create texture
            HRESULT res = m_device->CreateTexture2D(&dbdesc, NULL, &m_depth);
            if(FAILED(res))
            {
                std::cout << "Failed to create texture" << std::endl;
            }

            res = m_device->CreateRenderTargetView(m_depth, NULL, &m_depthRTV);
            if(FAILED(res))
            {
                std::cout << "Failed to create render target view" << std::endl;
            }

            res = m_device->CreateShaderResourceView(m_depth, NULL, &m_depthSRV);
            if(FAILED(res))
            {
                std::cout << "Failed to create shader resource view" << std::endl;
            }
        }

        void DirectXManagerImpl::CreateRealDepthBuffer()
        {
            D3D11_TEXTURE2D_DESC dbdesc;
            ZeroMemory(&dbdesc, sizeof(dbdesc));
            dbdesc.Width = m_screenResolution.x;
            dbdesc.Height = m_screenResolution.y;
            dbdesc.MipLevels = 1;
            dbdesc.ArraySize = 1;
            dbdesc.Format = DXGI_FORMAT_R32_TYPELESS;
            dbdesc.SampleDesc.Count = 1;
            dbdesc.SampleDesc.Quality = 0;
            dbdesc.Usage = D3D11_USAGE_DEFAULT;
            dbdesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
            dbdesc.CPUAccessFlags = 0;
            dbdesc.MiscFlags = 0;

            D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
            ZeroMemory(&descDSV, sizeof(descDSV));
            descDSV.Format = DXGI_FORMAT_D32_FLOAT;
            descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
            descDSV.Texture2D.MipSlice = 0;

            HRESULT res = m_device->CreateTexture2D(&dbdesc, NULL, &m_depthBuffer);
            if(FAILED(res))
            {
                std::cout << "Failed to create texture" << std::endl;
            }

            res = m_device->CreateDepthStencilView(m_depthBuffer, &descDSV, &m_depthView);
            if(FAILED(res))
            {
                std::cout << "Failed to create depth stencil view" << std::endl;
            }

            // Depth buffer
            // Might want this in a class for readability and easy changing between states
        }

        void DirectXManagerImpl::CreateViewport()
        {
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
        }

        void DirectXManagerImpl::CreateSamplers()
        {
            // For default texture sampler
            D3D11_SAMPLER_DESC texSamDesc;
            texSamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; // could be D3D11_FILTER_ANISOTROPIC
            texSamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
            texSamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
            texSamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
            texSamDesc.MipLODBias = 0;
            texSamDesc.MaxAnisotropy = 1; // this should be 16
            texSamDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
            texSamDesc.BorderColor[0] = 0.0f;
            texSamDesc.BorderColor[1] = 0.0f;
            texSamDesc.BorderColor[2] = 0.0f;
            texSamDesc.BorderColor[3] = 0.0f;
            texSamDesc.MinLOD = -3.402823466e+38F; // -FLT_MAX
            texSamDesc.MaxLOD = 3.402823466e+38F; // FLT_MAX
            // mParticleTexID = CreateTexture(L"Textures/VitPlupp.dds");
            // TODO add more different things like transparancy
            HRESULT res = m_device->CreateSamplerState(&texSamDesc, &m_defaultSamplerState);
            CheckHRESULT(res, "Fault when creating sampler");
            m_deviceContext->PSSetSamplers(0, 1, &m_defaultSamplerState);

            // Create glow sapler

            // for compute shader
            texSamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            texSamDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
            texSamDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
            texSamDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
            res = m_device->CreateSamplerState(&texSamDesc, &m_glowSamplerState);
            CheckHRESULT(res, "Fault when creating sampler");

            m_deviceContext->CSSetSamplers(0, 1, &m_glowSamplerState);
        }

        void DirectXManagerImpl::CreateRasterizerStates()
        {
            // For default rasterizer state
            D3D11_RASTERIZER_DESC rastDesc;
            ZeroMemory(&rastDesc, sizeof(rastDesc));
            rastDesc.FillMode = D3D11_FILL_SOLID;
            rastDesc.CullMode = D3D11_CULL_BACK;
            rastDesc.FrontCounterClockwise = false;
            rastDesc.DepthBias = 0;
            rastDesc.DepthBiasClamp = 0.0f;
            rastDesc.SlopeScaledDepthBias = 0.0f;
            rastDesc.DepthClipEnable = false;
            rastDesc.ScissorEnable = false;
            rastDesc.MultisampleEnable = false;
            rastDesc.AntialiasedLineEnable = false;
            m_defaultRasterizerState = CreateRasterizerState(rastDesc);

            // For default depth stencil state
            D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
            ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
            depthStencilDesc.DepthEnable = true;
            depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
            depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
            m_defaultDepthStencilState = CreateDepthStencilState(depthStencilDesc);
        }

        void DirectXManagerImpl::CreateBlendStates()
        {
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
            HRESULT res = m_device->CreateBlendState(&t_blendDesc, &m_enableBlendState);
            if(FAILED(res))
            {
                std::cout << "Failed to create blend state" << std::endl;
            }

            t_blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
            t_blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
            res = m_device->CreateBlendState(&t_blendDesc, &m_enableBlendStateText);
            if(FAILED(res))
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
            HRESULT res = m_device->CreateBuffer(&bd, NULL, &m_worldMatrix);
            if(!CheckHRESULT(res, "Error when creating sprite buffer"))
            {
                return;
            }

            bd.ByteWidth = sizeof(DoremiEditor::Core::MaterialData);
            res = m_device->CreateBuffer(&bd, NULL, &m_materialBuffer);
            if(!CheckHRESULT(res, "Error when creating sprite buffer"))
            {
                return;
            }

            ZeroMemory(&bd, sizeof(bd));
            bd.Usage = D3D11_USAGE_DYNAMIC;
            bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            bd.MiscFlags = 0;
            bd.StructureByteStride = 0;
            bd.ByteWidth = sizeof(SpriteData);
            res = m_device->CreateBuffer(&bd, NULL, &m_spriteDataBuffer);
            if(!CheckHRESULT(res, "Error when creating sprite buffer"))
            {
                return;
            }

            bd.ByteWidth = sizeof(ResolutionStruct);
            res = m_device->CreateBuffer(&bd, NULL, &m_resolutionBuffer);
            if(!CheckHRESULT(res, "Error when creating sprite buffer"))
            {
                return;
            }
            ResolutionStruct t_resoltution;
            t_resoltution.resolution = m_screenResolution;
            t_resoltution.padding = DirectX::XMFLOAT2(0, 0);

            D3D11_MAPPED_SUBRESOURCE tMS;
            m_deviceContext->Map(m_resolutionBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMS);
            memcpy(tMS.pData, &t_resoltution, sizeof(ResolutionStruct));
            m_deviceContext->Unmap(m_resolutionBuffer, NULL);

            m_deviceContext->PSSetConstantBuffers(2, 1, &m_resolutionBuffer);
            m_deviceContext->CSSetConstantBuffers(2, 1, &m_resolutionBuffer);
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
            memcpy(tMS.pData, &spriteRenderData[0].materialMessage.data, sizeof(spriteRenderData[0].materialMessage.data));
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

            m_deviceContext->Map(m_materialBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMS);
            memcpy(tMS.pData, &materialData.data, sizeof(&materialData.data));
            m_deviceContext->Unmap(m_materialBuffer, NULL);
            m_deviceContext->PSSetConstantBuffers(1, 1, &m_materialBuffer);

            m_deviceContext->PSSetSamplers(0, 1, &samplerState);
            m_deviceContext->PSSetShaderResources(0, 1, &texture);
            m_deviceContext->PSSetShaderResources(5, 1, &glowtexture);
            m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            m_deviceContext->IASetVertexBuffers(0, 1, &vertexData, &stride, &offset);
            m_deviceContext->VSSetConstantBuffers(0, 1, &m_worldMatrix);

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
            using namespace DoremiEditor::Core;
            // Sort the data according after mesh then texture
            std::sort(renderData.begin(), renderData.end(), SortOnVertexThenTexture);
            // std::sort(renderData.begin(), renderData.end(), SortRenderData); //TODORT remove

            // Nothing to draw
            if(renderData.size() == 0)
            {
                return;
            }

            // Setup required variables
            const uint32_t stride = sizeof(SkeletalVertex);
            const uint32_t offset = 0;
            ID3D11Buffer* vertexData = renderData[0].vertexData;
            MaterialMessage materialData = renderData[0].materialMessage;
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
            m_deviceContext->CSSetSamplers(0, 1, &samplerState);
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

                if (renderData[i].glowTexture != renderData[i - 1].glowTexture) // Check if texture has been changed
                {
                    glowtexture = renderData[i].glowTexture;
                    if (glowtexture != nullptr) // TODORT is it even required to check for null? Can this happen? Remove
                    {
                        m_deviceContext->PSSetShaderResources(5, 1, &glowtexture);
                    }
                }

                materialData = renderData[i].materialMessage;
                if(&materialData != nullptr) // TODORT is it even required to check for null? Can this happen? Remove
                {
                    D3D11_MAPPED_SUBRESOURCE tMS;
                    m_deviceContext->Map(m_materialBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMS);
                    memcpy(tMS.pData, &renderData[i].materialMessage.data, sizeof(renderData[i].materialMessage.data));
                    m_deviceContext->Unmap(m_materialBuffer, NULL);

                    m_deviceContext->PSSetConstantBuffers(1, 1, &m_materialBuffer);
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
            m_deviceContext->OMSetRenderTargets(1, &m_horGlowRTV, m_depthView);
        }

        void DirectXManagerImpl::SetRenderTargetGlow()
        {
            // Add depth bind to OM
            ID3D11RenderTargetView* t_RTArray[3] = {m_colorRTV, m_horGlowRTV, m_depthRTV};
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

        void DirectXManagerImpl::EnableTextBlend()
        {
            float blendFactor[4];

            // Setup the blend factor.
            blendFactor[0] = 0.0f;
            blendFactor[1] = 0.0f;
            blendFactor[2] = 0.0f;
            blendFactor[3] = 0.0f;

            // Turn on the alpha blending.
            m_deviceContext->OMSetBlendState(m_enableBlendStateText, blendFactor, 0xffffffff);
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
            // Set sampler here because, why not
            m_deviceContext->CSSetSamplers(0, 1, &m_glowSamplerState);

            ID3D11ShaderResourceView* nullSRV = {NULL};
            ID3D11UnorderedAccessView* nullUAV = {NULL};
            ID3D11RenderTargetView* nullRTV[3] = {NULL, NULL, NULL};

            m_deviceContext->OMSetRenderTargets(3, nullRTV, nullptr);

            //////BLUR HORIZONAL
            m_deviceContext->CSSetShaderResources(0, 1, &m_horGlowSRV);
            m_deviceContext->CSSetUnorderedAccessViews(0, 1, &m_vertGlowUAV, 0);

            // dispatch
            m_graphicContext.m_graphicModule->GetSubModuleManager().GetComputeShaderManager().DispatchBlurVertical();

            m_deviceContext->CSSetShaderResources(0, 1, &nullSRV);
            m_deviceContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, 0);

            /////////BLUR VERTICAL

            m_deviceContext->CSSetShaderResources(0, 1, &m_vertGlowSRV);
            m_deviceContext->CSSetShaderResources(1, 1, &m_colorSRV);

            // uav   m_glowUAV
            m_deviceContext->CSSetUnorderedAccessViews(0, 1, &m_backbufferUAV, 0);

            // dispatch
            m_graphicContext.m_graphicModule->GetSubModuleManager().GetComputeShaderManager().DispatchBlurHorizontal();

            // clear srv and uav
            m_deviceContext->CSSetShaderResources(0, 1, &nullSRV);
            m_deviceContext->CSSetShaderResources(1, 1, &nullSRV);
            m_deviceContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, 0);
        }

        void DirectXManagerImpl::BeginDraw() { DispatchCompute(); }

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
            m_deviceContext->ClearRenderTargetView(m_colorRTV, color);
            m_deviceContext->ClearRenderTargetView(m_horGlowRTV, color);
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

            if(m_isFullscreen != p_fullscreen)
            {
                m_isFullscreen = p_fullscreen;

                if(m_isFullscreen)
                {

                    SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN);
                }
                else
                {
                    SDL_SetWindowFullscreen(m_window, 0);
                }
            }
        }


        std::vector<std::pair<uint32_t, uint32_t>> DirectXManagerImpl::GetResolutions(const uint32_t& p_monitor)
        {
            if(m_displayModes.count(p_monitor))
            {
                std::vector<std::pair<uint32_t, uint32_t>> t_resolutions;

                for(auto& t_resolution : m_displayModes[p_monitor])
                {
                    t_resolutions.push_back(t_resolution.first);
                }

                return t_resolutions;
            }
            else
            {
                // something wrong..
                return std::vector<std::pair<uint32_t, uint32_t>>();
            }
        }

        std::vector<uint32_t> DirectXManagerImpl::GetRefreshRates(const uint32_t& p_monitor, const std::pair<uint32_t, uint32_t>& p_resolution)
        {
            if(m_displayModes.count(p_monitor))
            {
                if(m_displayModes[p_monitor].count(p_resolution))
                {
                    std::vector<uint32_t> t_refreshRates;

                    for(auto& t_refreshRate : m_displayModes[p_monitor][p_resolution])
                    {
                        t_refreshRates.push_back(t_refreshRate);
                    }

                    return t_refreshRates;
                }
            }

            // something wrong..
            return std::vector<uint32_t>();
        }

        void DirectXManagerImpl::ClearBuffersForResize()
        {
            // Release backbuffer
            m_backbufferUAV->Release();

            // Release glow
            m_horGlowRTV->Release();
            m_horGlowSRV->Release();
            m_vertGlowUAV->Release();
            m_vertGlowSRV->Release();

            // Release depth
            m_depth->Release();
            m_depthRTV->Release();
            m_depthSRV->Release();

            // Release color
            m_colorRTV->Release();
            m_colorSRV->Release();

            // Release depth;
            m_depthBuffer->Release();
            m_depthView->Release();
        }

        void DirectXManagerImpl::SetResolution(const std::pair<uint32_t, uint32_t>& p_resolution)
        {
            // If there is a change
            if(m_screenResolution.x != static_cast<float>(p_resolution.first) || m_screenResolution.y != static_cast<float>(p_resolution.second))
            {
                m_screenResolution.x = static_cast<float>(p_resolution.first);
                m_screenResolution.y = static_cast<float>(p_resolution.second);

                SDL_SetWindowSize(m_window, p_resolution.first, p_resolution.second);

                // Clear buffers and views
                ClearBuffersForResize();

                // Update swap chain
                HRESULT hr = m_swapChain->ResizeBuffers(NUM_BACK_BUFFERS, p_resolution.first, p_resolution.second, DXGI_FORMAT_R8G8B8A8_UNORM,
                                                        DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
                if(FAILED(hr))
                {
                    std::cout << "failed setting resolution" << std::endl;
                }

                CreateBackBufferViews();

                CreateBlurrBuffers();

                CreateDepthViews();

                CreateColorBuffer();

                CreateRealDepthBuffer();

                m_deviceContext->OMSetRenderTargets(1, &m_colorRTV, m_depthView);

                CreateViewport();

                // Update resolution constant buffer
                ResolutionStruct t_resoltution;
                t_resoltution.resolution = m_screenResolution;
                t_resoltution.padding = DirectX::XMFLOAT2(0, 0);

                D3D11_MAPPED_SUBRESOURCE tMS;
                m_deviceContext->Map(m_resolutionBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMS);
                memcpy(tMS.pData, &t_resoltution, sizeof(ResolutionStruct));
                m_deviceContext->Unmap(m_resolutionBuffer, NULL);

                // if (m_isFullscreen)
                //{
                //    SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN);
                //}
            }
        }

        void DirectXManagerImpl::SetRefreshRate(uint32_t p_refreshRate)
        {
            if(m_refreshRate != p_refreshRate)
            {
                DXGI_MODE_DESC scd;
                ZeroMemory(&scd, sizeof(DXGI_MODE_DESC));
                scd.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // use 32-bit color
                scd.RefreshRate.Numerator = m_refreshRate;
                scd.RefreshRate.Denominator = 1;

                HRESULT hr = m_swapChain->ResizeTarget(&scd);
                if(FAILED(hr))
                {
                    std::cout << "failed setting refreshrate" << std::endl;
                }
            }
        }

        void DirectXManagerImpl::SetMonitor(uint32_t p_monitor)
        {
            if(m_currentMonitor != p_monitor)
            {
                m_currentMonitor = p_monitor;

                if(m_isFullscreen)
                {
                }
            }
        }
    }
}