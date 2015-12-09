#include <DoremiEngine/Core/Include/SharedContext.hpp>

//Inside module
#include <GraphicMain.hpp>
#include <Shader/VertexShader.hpp>
#include <Shader/PixelShader.hpp>
#include <Internal/Mesh/MeshInfoImpl.hpp>
#include <ModelLoader.hpp>
#include <ShaderTypeEnum.hpp>
#include <HelpFunctions.hpp>

namespace DoremiEngine
{
    namespace Graphic
    {

        GraphicMain::GraphicMain(const Core::SharedContext& p_sharedContext) : m_sharedContext(p_sharedContext) {}

        GraphicMain::~GraphicMain() {}
        // TODOKO Remove!
        LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            // sort through and find what code to run for the message given
            switch(message)
            {
                // this message is read when the window is closed
                case WM_KEYDOWN:
                {
                    switch(wParam)
                    {
                        case VK_ESCAPE:
                            PostQuitMessage(0);
                            return 0;
                    }
                }
                break;

            case WM_DESTROY:
            {
                // close the application entirely
                PostQuitMessage(0);
                return 0;
            } break;
            }

            // Handle any messages the switch statement didn't
            return DefWindowProc(hWnd, message, wParam, lParam);
        }



        void GraphicMain::CreateGraphicWindow()
        {
            //Shouldnt be here and should be SDL window?
            HINSTANCE hInstance;
            hInstance = 0;
            HWND hWnd;
            WNDCLASSEX wc;
            ZeroMemory(&wc, sizeof(WNDCLASSEX));

            wc.cbSize = sizeof(WNDCLASSEX);
            wc.style = CS_HREDRAW | CS_VREDRAW;
            wc.lpfnWndProc = WindowProc;
            wc.hInstance = hInstance;
            wc.hCursor = LoadCursor(NULL, IDC_ARROW);
            wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
            wc.lpszClassName = L"TestWindow";

            RegisterClassEx(&wc);

            RECT wr = { 0,0,800,600 };
            AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

            hWnd = CreateWindowEx(NULL,
                L"TestWindow",    // name of the window class
                L"Our First Windowed Program",   // title of the window
                WS_OVERLAPPEDWINDOW,    // window style
                0,    // x-position of the window
                0,    // y-position of the window
                wr.right - wr.left,    // width of the window
                wr.bottom - wr.top,    // height of the window
                NULL,    // we have no parent window, NULL
                NULL,    // we aren't using menus, NULL
                wc.hInstance,    // application handle
                NULL);    // used with multiple windows, NULL

            ShowWindow(hWnd, 1);


        }

        void GraphicMain::InitializeDirectX()
        {
            DXGI_SWAP_CHAIN_DESC scd;

            // clear out the struct for use
            ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

            // fill the swap chain description struct
            scd.BufferCount = 1;                                    // one back buffer
            scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
            scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
            scd.OutputWindow = GetActiveWindow();                   // the window to be used
            scd.SampleDesc.Count = 4;                               // how many multisamples
            scd.Windowed = TRUE;                                    // windowed/full-screen mode

            HRESULT res = D3D11CreateDeviceAndSwapChain(NULL,
                D3D_DRIVER_TYPE_HARDWARE,
                NULL,
                D3D11_CREATE_DEVICE_DEBUG,
                NULL,
                NULL,
                D3D11_SDK_VERSION,
                &scd,
                &m_swapChain,
                &m_device,
                NULL,
                &m_deviceContext);

            ID3D11Texture2D* t_BackBuffer;
            m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&t_BackBuffer);
            m_device->CreateRenderTargetView(t_BackBuffer, NULL, &m_backBuffer);
            t_BackBuffer->Release();

            //Rasterizer info, here you can change from backface culling mm
            //We might want this as a class or something to enable easy switching between rasterizers
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

            //Depth buffer
            //Might want this in a class for readability and easy changing between states
            D3D11_TEXTURE2D_DESC dbdesc;
            ZeroMemory(&dbdesc, sizeof(dbdesc));
            dbdesc.Width = 800;
            dbdesc.Height = 600;
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

            //Viewport
            //TODOKO change so width and height is modifiable and taken from some smarter place
            D3D11_VIEWPORT viewport;
            ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

            viewport.TopLeftX = 0;
            viewport.TopLeftY = 0;
            viewport.Width = 800;
            viewport.Height = 600;
            viewport.MinDepth = 0.0f;
            viewport.MaxDepth = 1.0f;

            m_deviceContext->RSSetViewports(1, &viewport);

            //TODO add more different things like transparancy
            //TODOKO Move code to better location
            int vertexShaderID = LoadShader(ShaderType::VertexShader, "BasicVertexShader.hlsl");
            int pixelShadderID = LoadShader(ShaderType::PixelShader, "BasicPixelShader.hlsl");

            D3D11_INPUT_ELEMENT_DESC ied[] =
            {
                { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
                { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
                { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            };

            m_vertexShaders[vertexShaderID]->SetInputLayout(ied, ARRAYSIZE(ied), m_device);
            m_vertexShaders[vertexShaderID]->SetActiveShader(m_deviceContext);
            m_pixelShaders[pixelShadderID]->SetActiveShader(m_deviceContext);

            //Buffer for projection and view matrix in vertex shader Should still be moved
            D3D11_BUFFER_DESC bd;
            ZeroMemory(&bd, sizeof(bd));

            bd.Usage = D3D11_USAGE_DYNAMIC;
            bd.ByteWidth = sizeof(MatrixBufferType);
            bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            bd.MiscFlags = 0;
            bd.StructureByteStride = 0;
            ID3D11Buffer* tBuffer;
            m_device->CreateBuffer(&bd, NULL, &tBuffer);
            using namespace DirectX;
            MatrixBufferType tBufferInfo;
            tBufferInfo.view = XMMatrixTranspose(XMMatrixLookAtLH(XMLoadFloat3(&XMFLOAT3(0.0f, 0.0f, 0.0f)), XMLoadFloat3(&XMFLOAT3(0, 0, 1)), XMLoadFloat3(&XMFLOAT3(0, 1, 0))));
            tBufferInfo.projection = XMMatrixTranspose(XMMatrixPerspectiveFovLH(90*3.14 / 180.0f, 800.0f / 600.0f, 0.1f, 1000.0f));  //XMMatrixPerspectiveFovLH(45.0f, 600.0f / 800.0f, 0.1f, 100));

            D3D11_MAPPED_SUBRESOURCE tMS;
            m_deviceContext->Map(tBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMS);
            memcpy(tMS.pData, &tBufferInfo, sizeof(tBufferInfo));
            m_deviceContext->Unmap(tBuffer, NULL);
            m_deviceContext->VSSetConstantBuffers(0, 1, &tBuffer);

            //World Matrix, should perhaps be here....
            bd.ByteWidth = sizeof(XMFLOAT4X4);
            m_device->CreateBuffer(&bd, NULL, &m_worldMatrix);

            m_deviceContext->VSSetConstantBuffers(1, 1, &m_worldMatrix);

            //For texture sampler
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
                                                                             //mParticleTexID = CreateTexture(L"Textures/VitPlupp.dds");
            res = m_device->CreateSamplerState(&texSamDesc, &m_sampler);
            CheckHRESULT(res, "Fault when creating sampler");
            m_deviceContext->PSSetSamplers(0, 1, &m_sampler);

            LoadMesh("test");
            LoadTexture("Test.dds");
        }

        int GraphicMain::LoadShader(const ShaderType& p_type, const std::string& p_fileName)
        {
            
            std::string filePath = m_sharedContext.GetWorkingDirectory() + "ShaderFiles/" + p_fileName;
            int returnID;
            bool success;
            switch (p_type)
            {
            case ShaderType::VertexShader:
            {
                VertexShader* t_shader = new VertexShader();
                success = t_shader->LoadShader(filePath, m_device);
                if (!success)
                {
                    //Some kind of error, use debugg logger :)
                    delete t_shader;
                    return -1;
                }
                m_vertexShaders.push_back(t_shader);
                returnID = m_vertexShaders.size() - 1;
                break;
            }
            case ShaderType::PixelShader:
            {
                PixelShader* t_shader = new PixelShader();
                success = t_shader->LoadShader(filePath, m_device);
                if (!success)
                {
                    //Some kind of error, use debugg logger :)
                    delete t_shader;
                    return -1;
                }
                m_pixelShaders.push_back(t_shader);
                returnID = m_pixelShaders.size() - 1;
                break;
            }
            default:
                break;
            }
            return returnID;
        }

        void GraphicMain::BindShader(const ShaderType& p_type, const int& p_shaderID)
        {
            switch (p_type)
            {
            case ShaderType::VertexShader:
                m_vertexShaders[p_shaderID]->SetActiveShader(m_deviceContext);
                break;
            case ShaderType::PixelShader:
                m_pixelShaders[p_shaderID]->SetActiveShader(m_deviceContext);
                break;
            default:
                break;
            }
        }

        int GraphicMain::LoadMesh(const std::string& p_meshFileName)
        {
            if (m_loadedMeshes.find(p_meshFileName) != m_loadedMeshes.end())
            {
                return m_loadedMeshes[p_meshFileName];
            }
            ModelLoader t_loader = ModelLoader();
            MeshInfo* tMeshInfo = new MeshInfoImpl();
            bool success = t_loader.LoadMesh(tMeshInfo, p_meshFileName, m_deviceContext, m_device);
            if (!success)
            {
                //ERROR
                return -1;
            }
            m_Meshes.push_back(tMeshInfo);
            int meshID = m_Meshes.size() - 1;;
            m_loadedMeshes[p_meshFileName] = meshID;
            return meshID;
        }

        int GraphicMain::LoadTexture(const std::string& p_textureFileName)
        {
            //Doesnt work for some reason
            std::string fileLocation = m_sharedContext.GetWorkingDirectory() + "Textures/" + p_textureFileName;
            ModelLoader t_loader = ModelLoader();
            ID3D11ShaderResourceView* newTexture = t_loader.LoadTexture(fileLocation, m_device);
            if (newTexture == nullptr)
            {
                //ERROR
                return -1;
            }
            m_textures.push_back(newTexture);
            return m_textures.size() - 1;         
        }

        void GraphicMain::Draw(const int& p_meshID, const int& p_textureID, const DirectX::XMFLOAT4X4& p_translationMatrix) //Should take one matrix
        {
            if (p_meshID > m_Meshes.size() - 1)
            {
                //TODOKO Print error message
                return;
            }
            D3D11_MAPPED_SUBRESOURCE tMS;
            m_deviceContext->Map(m_worldMatrix, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMS);
            memcpy(tMS.pData, &p_translationMatrix, sizeof(p_translationMatrix));
            m_deviceContext->Unmap(m_worldMatrix, NULL);
            m_deviceContext->VSSetConstantBuffers(1, 1, &m_worldMatrix);

            if (p_textureID <= m_textures.size() -1)
            {
                m_deviceContext->PSSetShaderResources(0, 1, &m_textures[p_textureID]);
            }
            
            ID3D11Buffer* bufferPointer = m_Meshes[p_meshID]->GetBufferHandle();
            unsigned int stride = sizeof(Vertex);
            unsigned int offset = 0;
            m_deviceContext->IASetVertexBuffers(0, 1, &bufferPointer, &stride, &offset);
            m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            m_deviceContext->Draw(m_Meshes[p_meshID]->GetVerticeCount(), 0);
        }

        void GraphicMain::DrawInstanced(const int& p_meshID) {} //Should take a list of matrices too

        void GraphicMain::DrawParticles() {}

        void GraphicMain::ComputeAfterEffects() {}

        void GraphicMain::EndDraw() 
        {

            //To here

            m_swapChain->Present(1, 0); //TODO Evaluate if vsync should always be active
            float color[] = { 0.3f,0.0f,0.5f,1.0f };
            m_deviceContext->ClearRenderTargetView(m_backBuffer, color);
            m_deviceContext->ClearDepthStencilView(m_depthView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
        }
    }
}