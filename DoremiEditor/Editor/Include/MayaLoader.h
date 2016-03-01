#pragma once
#ifndef MAYALOADER_H
#define MAYALOADER_H
#endif


#include <vector>
#include <Windows.h>
#include <ostream>
#include <iostream>
#include <string>

#include "Transform.h"
#include "Material.h"
#include "Mutex.h"
#include "FileHandler.h"
#include "Cam.h"

using namespace std;
//const int MAX_NAME_SIZE = 100; la den i ObjectData så att alla kan komma åt den
const UINT MAX_NR_LIGHTS = 256;
class MayaLoader{
	
public:	
	MayaLoader(ID3D11Device* gd, ID3D11DeviceContext* gdc, UINT screenWidth, UINT screenHeight);
	MayaLoader();
	~MayaLoader();
	void DrawScene();
	void SaveScene();

	//void StartSyncing(); //startar hela processen
	void CreateFileMaps(unsigned int messageFilemapSize);
	void InitVariables();

	void TryReadAMessage();

	void ReadTransform(int i); //parameter för edited/added/removed
	void ReadMesh(int i);
	void ReadMeshData(size_t offSetStart); //bara så det inte ska bli lika mkt kod
	void ReadLight(int i);
	void ReadMaterial(int i);
	void ReadCamera(int i);
	void ReadName(); //mallocen måste deallokeras oxå!!

private:
	//externa grejer
	ID3D11Device* gDevice = nullptr;
	ID3D11DeviceContext* gDeviceContext = nullptr;
	UINT screenWidth, screenHeight;

	ID3D11SamplerState *wrap_Sampstate;
	ID3D11SamplerState *clamp_Sampstate;
	//*camera*********camera*
	bool UpdateCameraValues(); //returnerar ifall en kamera finns, om den inte finns så vill man nog inte rendera alls
	struct CameraCBufferData
	{
		XMFLOAT4X4 view;
		XMFLOAT4X4 projection;
		
	};
	CameraCBufferData defaultCameraCBufferData;
	ID3D11Buffer* cDefaultCameraConstantBuffer = nullptr;
	//*camera*********camera*

	//*light**********light**
	struct LightCBufferDataArray
	{
		LightCBufferData lightDatas[MAX_NR_LIGHTS];
		UINT NumLights;
		float pad[3];
	};
	LightCBufferDataArray lightCBufferDataArray;
	ID3D11Buffer *lightCbufferArray = nullptr;
	void CreateLightCBufferArray();
	void UpdateLightCBufferArray();
	//*light**********light**

	FileHandler *fileHandler = nullptr;
	//lokala-isch grejer
	//data om filemapen
	struct FilemapInfo{
		size_t head_ByteOffset; //offset in bytes from beginning of the shared memory
		size_t tail_ByteOffset; //offset in bytes from beginning of the shared memory
		size_t non_accessmemoryOffset; //memory in beginning of file thats no touchy, so that head and tail won't get to each other
		//size_t totalConsumers;
		size_t messageFilemap_Size;
		char outFileName[MAX_NAME_SIZE];
		FilemapInfo(){
			head_ByteOffset = 0;
			tail_ByteOffset = 0;
			non_accessmemoryOffset = 10;
			//totalConsumers = 0;
			messageFilemap_Size = 1024; //storleken på filemapen med meddelanden
		}

	};
	FilemapInfo fileMapInfo;

	void SetFilemapInfoValues(size_t headPlacement, size_t tailPlacement, size_t nonAccessMemoryPlacement, size_t messageFileMapTotalSize); //sätt negativa värden om den inte ska ändras!
	HANDLE hMessageFileMap = nullptr;
	LPVOID mMessageData = nullptr;
	unsigned int mSize = 1 << 13; //2^15, denna sätts vid skapelsen av filemapen, parameter värdet till funktionen
	//will hold information about where heads and tails are for example, aswell as where the free memory is in the other shared memory
	HANDLE hInfoFileMap = nullptr;
	LPVOID mInfoData = nullptr;
	unsigned int mInfoSize = 1 << 6;

	size_t localTail = 0; //hur långt ifrån starten i bytes den är på filemapen med meddelanden
	int delay = 10; //hur länge den ska sleepa mellan varje läsnings försök

	struct MessageHeader{
		MessageHeader(){
			nodeType = 0;
			messageType = 0;
			msgConfig = 0;
			byteTotal = 0;
			byteSize = 0;
			bytePadding = 0;
			//nameElementSize = 0;
		}

		int nodeType;
		int messageType;
		int msgConfig;
		size_t byteTotal;
		size_t byteSize;
		size_t bytePadding;
		//int nameElementSize; //hur stort är namnet i karaktärer
		//char objectName[100]; //dynamisk eller inte, char compare funktionen vad göra?
	};
	MessageHeader messageHeader;
	bool headerDidFit; //true om den fick plats på denna sidan av filemapen, annars är den på andra, används vid läsning av meddelanden
	
	
	//har kvar vissa av dessa structsen även om de är lite onödiga då alla värden i vissa redan ligger i en full struct, men vill hålla det cleant med namngivning :-)
	struct TransformMessage{
		char objectName[MAX_NAME_SIZE]; //ifall dessa ska ändras till dynamiskt allokerade så kolla FileHandler->CorrectName!!
		char parentName[MAX_NAME_SIZE];
		TransformData transformData;
	};
	struct CameraMessage{
		char objectName[MAX_NAME_SIZE];
		char transformName[MAX_NAME_SIZE];
		CameraData cameraData;
	};
	struct MeshMessage{
		char objectName[MAX_NAME_SIZE];
		int nrOfTransforms;
		NameMaxNameStruct *transformNames; //skriver man såhär? en vector av char arrayer som är MAX_NAME_SIZE stora
		//char transformName[MAX_NAME_SIZE];
		char materialName[MAX_NAME_SIZE];
		int meshID; //kolla ifall denna meshen ska instance draw:as!
		int materialID;
		MeshData *meshData;
	};
	struct MaterialMessage{ //namnet på den ligger i headern sen
		char objectName[MAX_NAME_SIZE];

		char textureName[MAX_NAME_SIZE];
        char glowMapName[MAX_NAME_SIZE];
        char specularMapName[MAX_NAME_SIZE];
        char bumpMapName[MAX_NAME_SIZE];        
        
		int materialType;
		MaterialData materialData;
	};
	struct LightMessage{
		//ljusvärden
		char objectName[MAX_NAME_SIZE];
		char transformName[MAX_NAME_SIZE];
		LightData lightdata;
	};
	//struct NameMessage // ligger i ObjectData nu för easyness
	//{
	//	char name1[MAX_NAME_SIZE]; //det nya namnet vid namechange
	//	char name2[MAX_NAME_SIZE];
	//};

	TransformMessage *transformMessage = nullptr;
	CameraMessage *cameraMessage = nullptr;
	MeshMessage *meshMessage = nullptr;
	MaterialMessage *materialMessage = nullptr;
	LightMessage *lightMessage = nullptr;
	NameMessage *nameMessage = nullptr;

	size_t transformMessage_MaxSize = 512;
	size_t cameraMessage_MaxSize = 512;
	size_t meshMessage_MaxSize = 1024 * 1024 * 4; //kan ju fan inte hårdkodas! (maxstorlek de e luuuugnt)
	size_t materialMessage_MaxSize = 2048;
	size_t lightMessage_MaxSize = 512;
	size_t nameMessage_MaxSize = 256;

	//vector<GameObject*> gameObjects;
	vector<Material*> materials;
	vector<Transform*> allTransforms;
	vector<Transform*> allMeshTransforms;
	vector<Transform*> allLightTransforms;
	vector<Transform*> allCameraTransforms;

	Transform* currentCameraTransform = nullptr;

	void MeshChange(MessageHeader mh, MeshMessage *mm); //lägger till ett nytt objekt om det inte redan finns eller updaterar en gammal, tar hand om den aktualla meshen
	void MeshAdded(MessageHeader mh, MeshMessage *mm);
	void MeshRenamed(MessageHeader mh, NameMessage *mm);

	void TransformAdded(MessageHeader mh, TransformMessage *mm);
	void TransformChange(MessageHeader mh, TransformMessage *mm);
	void TransformDeleted(MessageHeader mh, NameMessage *mm);
	void TransformRenamed(MessageHeader mh, NameMessage *mm);

	void MaterialAdded(MessageHeader mh, MaterialMessage *mm);
	void MaterialChange(MessageHeader mh, MaterialMessage *mm);
	void MaterialDeleted(MessageHeader mh, NameMessage *mm);
	void MaterialRenamed(MessageHeader mh, NameMessage *mm);

	void LightAdded(MessageHeader mh, LightMessage *mm);
	void LightChange(MessageHeader mh, LightMessage *mm);
	void LightRenamed(MessageHeader mh, NameMessage *mm);

	void CameraAdded(MessageHeader mh, CameraMessage *mm);
	void CameraChange(MessageHeader mh, CameraMessage *mm);
	//ingen delete coz på transformen
	void CameraRenamed(MessageHeader mh, NameMessage *mm);
	void CameraSwitch(MessageHeader mh, CameraMessage *mm); //5

};