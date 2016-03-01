#include "Material.h"
#include "DDSTextureLoader.h"

void Material::UpdateCBuffer()
{
    // updatesubresource med den nya materialData
    // hämtar all data som behövs från materialData och store:ar den in constantbufferstructen
    materialCBufferData.diffuse = materialData.diffuse;
    for(int i = 0; i < 3; i++)
    {
        materialCBufferData.color[i] = materialData.color[i];
    }
    for(int i = 0; i < 3; i++)
    {
        materialCBufferData.ambColor[i] = materialData.ambColor[i];
    }
    for(int i = 0; i < 3; i++)
    {
        materialCBufferData.specColor[i] = materialData.specColor[i];
    }
    materialCBufferData.specCosine = materialData.specCosine;
    materialCBufferData.specEccentricity = materialData.specEccentricity;
    materialCBufferData.specRollOff = materialData.specRollOff;

    gDeviceContext->UpdateSubresource(materialCbuffer, 0, NULL, &materialCBufferData, 0, 0);
    // gDeviceContext->PSSetConstantBuffers(1, 1, &materialCbuffer); sätts i render
}
void Material::CreateCBuffer()
{
    D3D11_BUFFER_DESC cbDesc = {0};
    cbDesc.ByteWidth = sizeof(MaterialCBufferData); // kolla så den är 16 byte alligned sen!!
    cbDesc.Usage = D3D11_USAGE_DEFAULT;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    // cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbDesc.MiscFlags = 0;
    cbDesc.StructureByteStride = 0;

    // Fill in the subresource data.
    D3D11_SUBRESOURCE_DATA InitData;
    InitData.pSysMem = &materialCBufferData; // ger den startvärde, default, använd updatesubresource sen
    InitData.SysMemPitch = 0;
    InitData.SysMemSlicePitch = 0;

    // Create the buffer.
    gDevice->CreateBuffer(&cbDesc, &InitData, &materialCbuffer);
}

void Material::CreateTexture(char* filePath, ID3D11Resource*& texture, ID3D11ShaderResourceView*& textureView)
{
    if(filePath != nullptr && filePath[0] != 0)
    {
        FILE* fp = nullptr;

        fp = fopen(filePath, "r");

        CoInitialize(NULL);
        char* filePathName = filePath; // viktigt så att den inte ändrar den gamla
        CorrectName(filePathName);
        // Convert to a wchar_t*
        size_t origsize = strlen(filePathName) + 1;
        const size_t newsize = MAX_NAME_SIZE;
        size_t convertedChars = 0;
        wchar_t wcstring[newsize];
        mbstowcs_s(&convertedChars, wcstring, origsize, filePathName, _TRUNCATE);
        wcscat_s(wcstring, L"");
        // wcout << wcstring << endl;

        /*std::string tempString(filePath);
        std::wstring tempWString = tempString;
        const wchar_t *filePathWchar;
        filePathWchar = tempWString.c_str();*/
        // HRESULT br = CreateWICTextureFromFile(gDevice, gDeviceContext, filePathWchar, nullptr, &textureView, 0);
        HRESULT hr = CreateDDSTextureFromFile(gDevice, wcstring, &texture, &textureView, 0, nullptr); // felaktigt filnamn
        std::cout << "Bajs";
    }
}