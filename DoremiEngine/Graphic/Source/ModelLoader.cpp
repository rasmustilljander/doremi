#include <ModelLoader.hpp>
#include <MeshInfo.hpp>
#include <HelpFunctions.hpp>
#include <DDSTextureLoader.h>

#include <d3d11_1.h>

namespace DoremiEngine
{
    namespace Graphic
    {
        ModelLoader::ModelLoader()
        {
        }


        ModelLoader::~ModelLoader()
        {
        }

        bool ModelLoader::LoadMesh(MeshInfo* o_meshInfo, const std::string& p_fileName, ID3D11DeviceContext* p_deviceContext, ID3D11Device* p_device)
        {
            Vertex tQuad[] = {
                { -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f},//1 //Lilla boxen
                { -0.5f, -0.5, -0.5f, 0.0f, 1.0f, 0.0f,0.0f,-1.0f }, //2//Framsidan
                { 0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f,0.0f,-1.0f }, //3
                { -0.5f, 0.5,  -0.5f, 0.0f, 1.0f, 0.0f,0.0f,-1.0f }, //1//Framsidan              
                { 0.5f, 0.5f,  -0.5f, 1.0f, 1.0f, 0.0f,0.0f,-1.0f }, //4
                { 0.5f,-0.5f,  -0.5f, 1.0f, 0.0f, 0.0f,0.0f,-1.0f }, //3

                { -0.5f, 0.5f, 0.5f, 1.0f, 0.0f , 0.0f,0.0f,1.0f }, //4
                { -0.5f, -0.5, 0.5f, 1.0f, 1.0f , 0.0f,0.0f,1.0f }, //5  Baksidan
                { 0.5f, -0.5f, 0.5f, 0.0f, 1.0f , 0.0f,0.0f,1.0f }, //6
                { -0.5f, 0.5f, 0.5f, 1.0f, 0.0f , 0.0f,0.0f,1.0f }, //4  Baksidan            
                { 0.5f, 0.5f, 0.5f, 0.0f, 0.0f  , 0.0f,0.0f,1.0f },  //7
                { 0.5f, -0.5f, 0.5f, 0.0f, 1.0f , 0.0f,0.0f,1.0f }, //6


                { -0.5f, 0.5f, -0.5f, 0.0f,0.0f, 0.0f,1.0f,0.0f },  //ovanpå 8
                { -0.5f, 0.5, 0.5f, 0.0f, 1.0f, 0.0f,1.0f,0.0f },   //// 9
                { 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f,1.0f,0.0f },   ////10
                { -0.5f, 0.5f, -0.5f, 0.0f,0.0f, 0.0f,1.0f,0.0f },  //ovanpå 8
                { 0.5f, 0.5f, -0.5f, 1.0f, 0.0f , 0.0f,1.0f,0.0f },  //11
                { 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f,1.0f,0.0f },   ////10
                

                { -0.5f, -0.5f, 0.5f, 0.0f,0.0f, 0.0f,-1.0f,0.0f },  //under 12
                { -0.5f, -0.5, -0.5f, 0.0f, 1.0f , 0.0f,-1.0f,0.0f },   //// 13
                { 0.5f, -0.5f, -0.5f, 1.0f, 1.0f , 0.0f,-1.0f,0.0f },   ////14
                { -0.5f, -0.5f, 0.5f, 0.0f,0.0f, 0.0f,-1.0f,0.0f },  //under 12             
                { 0.5f, -0.5f, 0.5f, 1.0f, 0.0f , 0.0f,-1.0f,0.0f },  //15
                { 0.5f, -0.5f, -0.5f, 1.0f, 1.0f , 0.0f,-1.0f,0.0f },   ////14

                { -0.5f, 0.5f, -0.5f, 0.0f,0.0f, -1.0f,0.0f,0.0f },  //vänster 16
                { -0.5f, -0.5, -0.5f, 0.0f, 1.0f, -1.0f,0.0f,0.0f },   //// 17
                { -0.5f, -0.5f, 0.5f, 1.0f, 1.0f, -1.0f,0.0f,0.0f },   ////18
                { -0.5f, 0.5f, -0.5f, 0.0f,0.0f, -1.0f,0.0f,0.0f },  //vänster 16           
                { -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, -1.0f,0.0f,0.0f },  //19
                { -0.5f, -0.5f, 0.5f, 1.0f, 1.0f, -1.0f,0.0f,0.0f },   ////18

                { 0.5f, 0.5f, 0.5f, 0.0f,0.0f , 1.0f,0.0f,0.0f },  //höger 20
                { 0.5f, -0.5, 0.5f, 0.0f, 1.0f , 1.0f,0.0f,0.0f },   //// 21
                { 0.5f, -0.5f, -0.5f, 1.0f, 1.0f , 1.0f,0.0f,0.0f },   ////22
                { 0.5f, 0.5f, 0.5f, 0.0f,0.0f , 1.0f,0.0f,0.0f },  //höger 20
                { 0.5f, 0.5f, -0.5f, 1.0f, 0.0f , 1.0f,0.0f,0.0f },  //23
                { 0.5f, -0.5f, -0.5f, 1.0f, 1.0f , 1.0f,0.0f,0.0f },   ////22            
            };
            int size = sizeof(tQuad);

            o_meshInfo->m_vertexCount = ARRAYSIZE(tQuad);
            D3D11_BUFFER_DESC bd;
            ZeroMemory(&bd, sizeof(bd));

            bd.Usage = D3D11_USAGE_DYNAMIC;
            bd.ByteWidth = size;
            bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
           
            HRESULT res = p_device->CreateBuffer(&bd, NULL, &o_meshInfo->m_bufferHandle);
            CheckHRESULT(res, "Error when creating mesh buffer");

            D3D11_MAPPED_SUBRESOURCE tMS;
            p_deviceContext->Map(o_meshInfo->m_bufferHandle, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMS);
            memcpy(tMS.pData, tQuad, size);
            p_deviceContext->Unmap(o_meshInfo->m_bufferHandle, NULL);

            return true;
        }

        int ModelLoader::LoadMaterial(const std::string& p_materialName)
        {
            return -1;
        }

        ID3D11ShaderResourceView* ModelLoader::LoadTexture(const std::string& p_fileName, ID3D11Device* p_device)
        {
            ID3D11ShaderResourceView* returnView;
            std::wstring tempName = StringToWstring(p_fileName);
            HRESULT res = DirectX::CreateDDSTextureFromFile(p_device, tempName.c_str(), nullptr, &returnView);
            if (!CheckHRESULT(res, "Couldnt load texture from file: " + p_fileName))
            {
                return nullptr;
            }
            return returnView;
        }

    }
}