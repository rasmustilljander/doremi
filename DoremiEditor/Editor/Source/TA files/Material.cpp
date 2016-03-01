#include "TA files/Material.h"
#include "TA files/DDSTextureLoader.h"

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
    // gDeviceContext->PSSetConstantBuffers(1, 1, &materialCbuffer); sätts i render
}
void Material::CreateCBuffer()
{
}

void Material::CreateTexture(char* filePath)
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
        std::cout << "Bajs";
    }
}