#include <GraphicMain.hpp>
#include <Shader/VertexShader.hpp>
#include <Shader/PixelShader.hpp>
#include <GraphicObject.hpp>
#include <ModelLoader.hpp>
#include <ShaderTypeEnum.hpp>
namespace DoremiEngine
{
    namespace Graphic
    {
        GraphicMain* GraphicMain::m_singleton = nullptr;
        GraphicMain* GraphicMain::GetInstance()
        {
            if (m_singleton == nullptr)
            {
                m_singleton = new GraphicMain();
            }
            return m_singleton;
        }

        GraphicMain::GraphicMain()
        {
        }


        GraphicMain::~GraphicMain()
        {
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
            scd.OutputWindow = GetActiveWindow();                                // the window to be used
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
        }

        int GraphicMain::LoadShader(const ShaderType& p_type, const std::string& p_fileName)
        {
            int returnID;
            bool success;
            switch (p_type)
            {
            case ShaderType::VertexShader:
            {
                VertexShader* t_shader = new VertexShader();
                success = t_shader->LoadShader(p_fileName);
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
                success = t_shader->LoadShader(p_fileName);
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
                m_vertexShaders[p_shaderID]->SetActiveShader();
                break;
            case ShaderType::PixelShader:
                m_pixelShaders[p_shaderID]->SetActiveShader();
                break;
            default:
                break;
            }
        }
        int GraphicMain::LoadObject(const std::string& p_meshFileName, const std::string& p_materialName)
        {
            ModelLoader t_loader = ModelLoader();
            int meshID = t_loader.LoadMesh(p_meshFileName);
            if (meshID == -1)
            {
                //TODORK call error and make sure it doesnt crash
            }
            int materialID = t_loader.LoadMaterial(p_materialName);
            if (materialID == -1)
            {
                //TODORK call error and make sure it doesnt crash
            }
            GraphicObject* newObject = new GraphicObject(meshID, materialID);
            m_graphicObjects.push_back(newObject);
            return m_graphicObjects.size() - 1;
        }
    }
}