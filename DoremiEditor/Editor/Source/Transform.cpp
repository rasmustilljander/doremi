#include "Transform.h"

void Transform::UpdateCBuffer()
{
    // updatesubresource med den nya transformData
    XMMATRIX tempWorld = XMMatrixIdentity();

    XMMATRIX tempScale = XMMatrixIdentity();
    XMMATRIX tempRotation = XMMatrixIdentity();
    XMMATRIX tempPosition = XMMatrixIdentity();

    tempScale = XMMatrixScaling(transformData.scale.x, transformData.scale.y, transformData.scale.z);
    // XMMatrixRotationQuaternion använd en quaternion istället! cool stuff, sen bör det funka
    // transformData.pos.z = transformData.pos.z * -1.0f;
    // transformData.rot.x = transformData.rot.x * -1.0f;
    // transformData.rot.y = transformData.rot.y * -1.0f;
    XMVECTOR rotationQuat = XMVectorSet(transformData.rot.x, transformData.rot.y, transformData.rot.z, transformData.rot.w);
    tempRotation = XMMatrixRotationQuaternion(rotationQuat);
    tempPosition = XMMatrixTranslation(transformData.pos.x, transformData.pos.y, transformData.pos.z);

    tempWorld = tempRotation * tempScale * tempPosition;
    if(parent != nullptr)
    {
        parent->UpdateCBuffer();
        XMMATRIX parentWorld = XMLoadFloat4x4(&parent->transformCBufferData.world);
        tempWorld = tempWorld /** parentWorld*/;
    }

    XMStoreFloat4x4(&transformCBufferData.world, XMMatrixTranspose(tempWorld));
    // transformdata ligger på plats 0, material på 1, osv
    gDeviceContext->UpdateSubresource(transformCBuffer, 0, NULL, &transformCBufferData.world, 0,
                                      0); // skapa en separat struct för transformdata som ska in i shadern, world osv
    gDeviceContext->VSSetConstantBuffers(0, 1, &transformCBuffer);
}

void Transform::CreateTransformCBuffer()
{ // glöm inte parentens skit?
    D3D11_BUFFER_DESC cbDesc = {0};
    cbDesc.ByteWidth = sizeof(TransformCBufferData); // kolla så den är 16 byte alligned sen!!
    cbDesc.Usage = D3D11_USAGE_DEFAULT;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    // cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbDesc.MiscFlags = 0;
    cbDesc.StructureByteStride = 0;

    //// Fill in the subresource data.
    // D3D11_SUBRESOURCE_DATA InitData;
    // InitData.pSysMem = &transformCBufferData; //ger den startvärde, default, använd updatesubresource sen
    // InitData.SysMemPitch = 0;
    // InitData.SysMemSlicePitch = 0;

    // Create the buffer.
    gDevice->CreateBuffer(&cbDesc, 0, &transformCBuffer);
}