#include "TA files/Mesh.h"
void Mesh::CreateBuffers()
{
    CreateIndices();
    CreateVertices();

    CreateVertexBuffer();
    CreateIndexBuffer();
}

void Mesh::EmptyVariables()
{
    // free(name); //LOOOL namnen är fukkin statiska biatch?! :P INTE NU LÄNGRE! malloc?
    free(meshDataP);
    delete(meshData); // meshData ligger i meshDataP, behövs detta??
}

void Mesh::EmptyBuffers()
{
}

void Mesh::CreateIndices()
{
    nrIndecies = meshData->nrI;
    if(indecies != nullptr) delete[] indecies;
    if(indexList != nullptr) delete[] indexList;

    indecies = new IndexV[nrIndecies];
    indexList = new UINT[nrIndecies]; // listan som görs en indexbuffer av


    IndexV tempI;
    for(int i = 0; i < nrIndecies; i++)
    {
        tempI.posI = meshData->indexPositions[i];
        tempI.norI = meshData->indexNormals[i];
        tempI.uvI = meshData->indexUVs[i];
        indecies[i] = tempI;
    }

    // for (int i = 0; i < nrIndecies; i = i + 3)
    //{
    //	tempI.posI = meshData->indexPositions[i+2];
    //	tempI.norI = meshData->indexNormals[i+2];
    //	tempI.uvI = meshData->indexUVs[i+2];
    //	indecies[i] = tempI;

    //	tempI.posI = meshData->indexPositions[i+1];
    //	tempI.norI = meshData->indexNormals[i+1];
    //	tempI.uvI = meshData->indexUVs[i+1];
    //	indecies[i+1] = tempI;

    //	tempI.posI = meshData->indexPositions[i];
    //	tempI.norI = meshData->indexNormals[i];
    //	tempI.uvI = meshData->indexUVs[i];
    //	indecies[i+2] = tempI;
    //}

    for(int i = 0; i < nrIndecies; i++)
    {
        indexList[i] = indecies[i].posI;
    }
}

void Mesh::CreateVertices()
{
    nrVertices = meshData->nrNor;
    if(vertices != nullptr) delete[] vertices;

    vertices = new Vertex[nrIndecies];
    Vertex tempV;

    // for (int i = 0; i < nrVertices; i++)
    //{
    //	tempV.pos = meshData->positions[i];
    //	tempV.nor = meshData->normals[i];
    //	tempV.uv = meshData->uvs[i];

    //	vertices[i] = tempV;
    //}
    // for (int i = 0; i < nrIndecies; i++){
    //
    //	tempV.pos = meshData->positions[indecies[i].posI];
    //	tempV.nor = meshData->normals[indecies[i].norI];
    //	tempV.uv = meshData->uvs[indecies[i].uvI];

    //	vertices[i] = tempV;
    //}

    for(int i = 0; i < nrIndecies; i = i + 3)
    {
        tempV.pos = meshData->positions[indecies[i + 2].posI];
        tempV.pos.z = tempV.pos.z * -1.0f;
        tempV.nor = meshData->normals[indecies[i + 2].norI];
        tempV.nor.z = tempV.nor.z * -1.0f;
        tempV.uv = meshData->uvs[indecies[i + 2].uvI];
        tempV.uv.v = 1.0f - tempV.uv.v;

        vertices[i] = tempV;

        tempV.pos = meshData->positions[indecies[i + 1].posI];
        tempV.pos.z = tempV.pos.z * -1.0f;
        tempV.nor = meshData->normals[indecies[i + 1].norI];
        tempV.nor.z = tempV.nor.z * -1.0f;
        tempV.uv = meshData->uvs[indecies[i + 1].uvI];
        tempV.uv.v = 1.0f - tempV.uv.v;

        vertices[i + 1] = tempV;

        tempV.pos = meshData->positions[indecies[i].posI];
        tempV.pos.z = tempV.pos.z * -1.0f;
        tempV.nor = meshData->normals[indecies[i].norI];
        tempV.nor.z = tempV.nor.z * -1.0f;
        tempV.uv = meshData->uvs[indecies[i].uvI];
        tempV.uv.v = 1.0f - tempV.uv.v;

        vertices[i + 2] = tempV;
    }
}

void Mesh::CreateVertexBuffer() // får skapa vertexarrayen oxå!!!!! hämta all data från meshData
{
    //// VertexBuffer description
    //D3D11_BUFFER_DESC bufferDesc = {0};
    //memset(&bufferDesc, 0, sizeof(bufferDesc));
    //bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    //bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    //bufferDesc.ByteWidth = sizeof(Vertex) * nrIndecies;
    //bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    //D3D11_SUBRESOURCE_DATA data;
    //data.pSysMem = vertices;
    //data.SysMemPitch = 0;
    //data.SysMemSlicePitch = 0;
    //hr = gDevice->CreateBuffer(&bufferDesc, &data, &vertexBuffer);
}

void Mesh::CreateIndexBuffer()
{
    //// Fill in a buffer description.
    //D3D11_BUFFER_DESC bufferDesc;
    //bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    //bufferDesc.ByteWidth = sizeof(UINT) * nrIndecies;
    //bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    //bufferDesc.CPUAccessFlags = 0;
    //bufferDesc.MiscFlags = 0;

    //// Define the resource data.
    //D3D11_SUBRESOURCE_DATA InitData;
    //InitData.pSysMem = indexList;
    //InitData.SysMemPitch = 0;
    //InitData.SysMemSlicePitch = 0;

    //// Create the buffer with the device.
    //hr = gDevice->CreateBuffer(&bufferDesc, &InitData, &indexBuffer);
}

void Mesh::RemapVertexBuffer()
{
    CreateIndices(); // antar jag behöver dessa här!!
    CreateVertices();
    //D3D11_MAPPED_SUBRESOURCE mappedResource;
    //ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

    //gDeviceContext->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    ////	Update the vertex buffer here.
    //memcpy(mappedResource.pData, vertices, sizeof(Vertex) * nrIndecies);
    ////	Reenable GPU access to the vertex buffer data.
    //gDeviceContext->Unmap(vertexBuffer, 0);
}