#include "CameraObj.h"
#include "Transform.h"

void CameraObj::UpdateCBuffer(UINT screenWidth, UINT screenHeight)
{
    TransformData tDataTemp = transform->transformData;

    // XMVECTOR pos = XMVectorSet(tDataTemp.pos.x, tDataTemp.pos.y, tDataTemp.pos.z, 1.0f);
    // XMVECTOR rotTemp = XMVectorSet(tDataTemp.rot.x, tDataTemp.rot.y, tDataTemp.rot.z, tDataTemp.rot.w);


    ////Load the stuff
    XMFLOAT4 rotQuad = XMFLOAT4(tDataTemp.rot.x, tDataTemp.rot.y, tDataTemp.rot.z, tDataTemp.rot.w); // använd denna sen
    // XMFLOAT4 rotQuad = XMFLOAT4(0, 0, -1, 0); //hårkodad
    XMVECTOR rotQuadVec = XMLoadFloat4(&rotQuad);
    rotQuadVec = XMVector4Normalize(rotQuadVec);
    XMFLOAT3 pos = XMFLOAT3(tDataTemp.pos.x, tDataTemp.pos.y, tDataTemp.pos.z);
    // XMFLOAT3 pos = XMFLOAT3(0, 0, 200);//hårkodad
    XMVECTOR posVec = XMLoadFloat3(&pos);

    ////Load standard vectors
    XMFLOAT3 startUp = XMFLOAT3(0, 1, 0);
    XMVECTOR startUpVec = XMLoadFloat3(&startUp);
    XMFLOAT3 startTar = XMFLOAT3(0, 0, 1);
    XMVECTOR startTarVec = XMLoadFloat3(&startTar);

    XMMATRIX rotMatrix = XMMatrixRotationQuaternion(rotQuadVec);
    startUpVec = XMVector3Transform(startUpVec, rotMatrix);
    startTarVec = XMVector3Transform(startTarVec, rotMatrix);

    XMFLOAT3 derpTar;
    XMStoreFloat3(&derpTar, startTarVec);


    XMMATRIX cameraMat = XMMatrixLookToLH(posVec, startTarVec, startUpVec);


    // XMVECTOR rot = XMVector3Rotate(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f), rotTemp); //+ positionen eller nått sånt, se denna
    // XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    // XMMATRIX view = XMMatrixLookAtRH(pos, rot, up);


    XMMATRIX projection;
    if(cameraData.isOrtho == 0)
    {
        float aspect = (float)screenWidth / (float)screenHeight;
        projection = XMMatrixPerspectiveFovLH(cameraData.hAngle, // cameraData.hAngle,
                                              aspect, // aspect ratio?
                                              1.0f, 4000);
    }
    else
    {
        float fovinv = (float)screenHeight / (float)screenWidth;
        projection = XMMatrixOrthographicLH(cameraData.hAngle, cameraData.hAngle * fovinv, 1.0f, 4000.0f);
    }

    XMMATRIX view = cameraMat;
    XMStoreFloat4x4(&cameraCBufferData.view, XMMatrixTranspose(view));
    XMStoreFloat4x4(&cameraCBufferData.projection, XMMatrixTranspose(projection));
    cameraCBufferData.cameraPos[0] = transform->transformData.pos.x;
    cameraCBufferData.cameraPos[1] = transform->transformData.pos.y;
    cameraCBufferData.cameraPos[2] = transform->transformData.pos.z;
    cameraCBufferData.cameraPos[3] = 1;

    gDeviceContext->UpdateSubresource(cameraCbuffer, 0, NULL, &cameraCBufferData, 0, 0);
}

void CameraObj::CreateCBuffer()
{
    D3D11_BUFFER_DESC cbDesc = {0};
    cbDesc.ByteWidth = sizeof(CameraCBufferData); // kolla så den är 16 byte alligned sen!!
    cbDesc.Usage = D3D11_USAGE_DEFAULT;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    // cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbDesc.MiscFlags = 0;
    cbDesc.StructureByteStride = 0;

    // Fill in the subresource data.
    D3D11_SUBRESOURCE_DATA InitData;
    InitData.pSysMem = &cameraCBufferData; // ger den startvärde, default, använd updatesubresource sen
    InitData.SysMemPitch = 0;
    InitData.SysMemSlicePitch = 0;

    // Create the buffer.
    gDevice->CreateBuffer(&cbDesc, &InitData, &cameraCbuffer);
}