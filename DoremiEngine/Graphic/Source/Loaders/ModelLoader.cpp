#include <Loaders/ModelLoader.hpp>
#include <Internal/Mesh/MeshInfoImpl.hpp>
#include <HelpFunctions.hpp>
#include <Loaders/DDSTextureLoader.h>
#include <vector>

#include <d3d11_1.h>

namespace DoremiEngine
{
    namespace Graphic
    {
        ModelLoader::ModelLoader() {}


        ModelLoader::~ModelLoader() {}

        bool ModelLoader::LoadQuad(MeshInfo* o_meshInfo, ID3D11DeviceContext* p_deviceContext, ID3D11Device* p_device)
        {
            VertexBasic tQuad[] = {{-0.5f, 0.5f, 0.0f, 0.0f}, // 1 //Lilla boxen
                                   {-0.5f, -0.5, 0.0f, 1.0f}, // 2//Framsidan
                                   {0.5f, -0.5f, 1.0f, 1.0f}, // 3
                                   {-0.5f, 0.5, 0.0f, 0.0f}, // 1//Framsidan
                                   {0.5f, 0.5f, 1.0f, 0.0f}, // 4
                                   {0.5f, -0.5f, 1.0f, 1.0f}}; // 3

            int size = sizeof(tQuad);

            o_meshInfo->SetVerticeCount(ARRAYSIZE(tQuad));
            D3D11_BUFFER_DESC bd;
            ZeroMemory(&bd, sizeof(bd));

            bd.Usage = D3D11_USAGE_DYNAMIC;
            bd.ByteWidth = size;
            bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            ID3D11Buffer* buffer;
            HRESULT res = p_device->CreateBuffer(&bd, NULL, &buffer);
            CheckHRESULT(res, "Error when creating mesh buffer");
            D3D11_MAPPED_SUBRESOURCE tMS;
            p_deviceContext->Map(buffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMS);
            memcpy(tMS.pData, tQuad, size);
            p_deviceContext->Unmap(buffer, NULL);
            o_meshInfo->SetBufferHandle(buffer);
            o_meshInfo->SetFileName("Quad");
            return true;
        }

        bool ModelLoader::LoadSphere(MeshInfo* o_meshInfo, ID3D11DeviceContext* p_deviceContext, ID3D11Device* p_device, int p_latLines, int p_longLines)
        {
            int t_numSphereVertices;
            int t_numSphereFaces;
            t_numSphereVertices = ((p_latLines - 2) * p_longLines) + 2;
            t_numSphereFaces = ((p_latLines - 3) * (p_longLines)*2) + (p_longLines * 2);

            float sphereYaw = 0.0f;
            float spherePitch = 0.0f;

            std::vector<Vertex> vertices(t_numSphereVertices);
            using namespace DirectX;
            XMVECTOR currVertPos = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

            vertices[0].position = XMFLOAT3(0.0f, 0.0f, 1.0f);

            for(DWORD i = 0; i < p_latLines - 2; ++i)
            {
                spherePitch = (i + 1) * (3.14 / (p_latLines - 1));
                XMMATRIX t_rotationx = XMMatrixRotationX(spherePitch);
                XMMATRIX t_rotationy;
                for(DWORD j = 0; j < p_longLines; ++j)
                {
                    sphereYaw = j * (6.28 / (p_longLines));
                    t_rotationy = XMMatrixRotationZ(sphereYaw);
                    currVertPos = XMVector3TransformNormal(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), (t_rotationx * t_rotationy));
                    currVertPos = XMVector3Normalize(currVertPos);
                    vertices[i * p_longLines + j + 1].position.x = XMVectorGetX(currVertPos);
                    vertices[i * p_longLines + j + 1].position.y = XMVectorGetY(currVertPos);
                    vertices[i * p_longLines + j + 1].position.z = XMVectorGetZ(currVertPos);
                }
            }
            vertices[t_numSphereVertices - 1].position = XMFLOAT3(0.0f, 0.0f, -1.0f);
            int size = sizeof(Vertex) * vertices.size();

            o_meshInfo->SetVerticeCount(vertices.size());
            D3D11_BUFFER_DESC bd;
            ZeroMemory(&bd, sizeof(bd));

            bd.Usage = D3D11_USAGE_DYNAMIC;
            bd.ByteWidth = size;
            bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            ID3D11Buffer* buffer;
            HRESULT res = p_device->CreateBuffer(&bd, NULL, &buffer);
            CheckHRESULT(res, "Error when creating mesh buffer");
            D3D11_MAPPED_SUBRESOURCE tMS;
            p_deviceContext->Map(buffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMS);
            memcpy(tMS.pData, &vertices[0], size);
            p_deviceContext->Unmap(buffer, NULL);
            o_meshInfo->SetBufferHandle(buffer);

            std::vector<DWORD> indices(t_numSphereFaces * 3);

            int k = 0;
            for(DWORD l = 0; l < p_longLines - 1; ++l)
            {
                indices[k] = 0;
                indices[k + 1] = l + 1;
                indices[k + 2] = l + 2;
                k += 3;
            }

            indices[k] = 0;
            indices[k + 1] = p_longLines;
            indices[k + 2] = 1;
            k += 3;

            for(DWORD i = 0; i < p_latLines - 3; ++i)
            {
                for(DWORD j = 0; j < p_longLines - 1; ++j)
                {
                    indices[k] = i * p_longLines + j + 1;
                    indices[k + 1] = i * p_longLines + j + 2;
                    indices[k + 2] = (i + 1) * p_longLines + j + 1;

                    indices[k + 3] = (i + 1) * p_longLines + j + 1;
                    indices[k + 4] = i * p_longLines + j + 2;
                    indices[k + 5] = (i + 1) * p_longLines + j + 2;

                    k += 6; // next quad
                }

                indices[k] = (i * p_longLines) + p_longLines;
                indices[k + 1] = (i * p_longLines) + 1;
                indices[k + 2] = ((i + 1) * p_longLines) + p_longLines;

                indices[k + 3] = ((i + 1) * p_longLines) + p_longLines;
                indices[k + 4] = (i * p_longLines) + 1;
                indices[k + 5] = ((i + 1) * p_longLines) + 1;

                k += 6;
            }

            for(DWORD l = 0; l < p_longLines - 1; ++l)
            {
                indices[k] = t_numSphereVertices - 1;
                indices[k + 1] = (t_numSphereVertices - 1) - (l + 1);
                indices[k + 2] = (t_numSphereVertices - 1) - (l + 2);
                k += 3;
            }

            indices[k] = t_numSphereVertices - 1;
            indices[k + 1] = (t_numSphereVertices - 1) - p_longLines;
            indices[k + 2] = t_numSphereVertices - 2;

            size = sizeof(DWORD) * indices.size();

            o_meshInfo->SetIndexCount(indices.size());
            ZeroMemory(&bd, sizeof(bd));

            bd.Usage = D3D11_USAGE_DEFAULT;
            bd.ByteWidth = size;
            bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
            bd.CPUAccessFlags = 0;
            bd.MiscFlags = 0;
            ID3D11Buffer* indexBuffer;
            D3D11_SUBRESOURCE_DATA iinitData;
            iinitData.pSysMem = &indices[0];
            res = p_device->CreateBuffer(&bd, &iinitData, &indexBuffer);

            CheckHRESULT(res, "Error when creating mesh buffer");
            /*D3D11_MAPPED_SUBRESOURCE tMS1;
            p_deviceContext->Map(indexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMS1);
            memcpy(tMS1.pData, &indices[0], size);
            p_deviceContext->Unmap(indexBuffer, NULL);*/

            o_meshInfo->SetIndexBufferHandle(indexBuffer);
            o_meshInfo->SetFileName("Sphere");


            return true;
        }
        bool ModelLoader::LoadMesh(MeshInfo* o_meshInfo, const std::string& p_fileName, ID3D11DeviceContext* p_deviceContext, ID3D11Device* p_device)
        {
            VertexBasic tQuad[] = {
                {-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f}, // 1 //Lilla boxen
                {-0.5f, -0.5, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f}, // 2//Framsidan
                {0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f}, // 3
                {-0.5f, 0.5, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f}, // 1//Framsidan
                {0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f}, // 4
                {0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f}, // 3

                {-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f}, // 4
                {-0.5f, -0.5, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f}, // 5  Baksidan
                {0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f}, // 6
                {-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f}, // 4  Baksidan
                {0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}, // 7
                {0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f}, // 6


                {-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f}, // ovanpå 8
                {-0.5f, 0.5, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f}, //// 9
                {0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f}, ////10
                {-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f}, // ovanpå 8
                {0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f}, // 11
                {0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f}, ////10


                {-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f}, // under 12
                {-0.5f, -0.5, -0.5f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f}, //// 13
                {0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f}, ////14
                {-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f}, // under 12
                {0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f}, // 15
                {0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f}, ////14

                {-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f}, // vänster 16
                {-0.5f, -0.5, -0.5f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f}, //// 17
                {-0.5f, -0.5f, 0.5f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f}, ////18
                {-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f}, // vänster 16
                {-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f}, // 19
                {-0.5f, -0.5f, 0.5f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f}, ////18

                {0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f}, // höger 20
                {0.5f, -0.5, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f}, //// 21
                {0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f}, ////22
                {0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f}, // höger 20
                {0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f}, // 23
                {0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f}, ////22
            };
            int size = sizeof(tQuad);

            o_meshInfo->SetVerticeCount(ARRAYSIZE(tQuad));
            D3D11_BUFFER_DESC bd;
            ZeroMemory(&bd, sizeof(bd));

            bd.Usage = D3D11_USAGE_DYNAMIC;
            bd.ByteWidth = size;
            bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            ID3D11Buffer* buffer;
            HRESULT res = p_device->CreateBuffer(&bd, NULL, &buffer);
            CheckHRESULT(res, "Error when creating mesh buffer");
            D3D11_MAPPED_SUBRESOURCE tMS;
            p_deviceContext->Map(buffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMS);
            memcpy(tMS.pData, tQuad, size);
            p_deviceContext->Unmap(buffer, NULL);
            o_meshInfo->SetBufferHandle(buffer);
            o_meshInfo->SetFileName(p_fileName);
            return true;
        }

        int ModelLoader::LoadMaterial(const std::string& p_materialName) { return -1; }

        ID3D11ShaderResourceView* ModelLoader::LoadTexture(const std::string& p_fileName, ID3D11Device* p_device)
        {
            ID3D11ShaderResourceView* returnView;
            std::wstring tempName = StringToWstring(p_fileName);
            HRESULT res = DirectX::CreateDDSTextureFromFile(p_device, tempName.c_str(), nullptr, &returnView);
            if(!CheckHRESULT(res, "Couldnt load texture from file: " + p_fileName))
            {
                return nullptr;
            }
            return returnView;
        }
    }
}