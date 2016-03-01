#pragma once
#ifndef CAMERAOBJ_H
#define CAMERAOBJ_H
#endif

#include "Entity.h"
//#include "Transform.h"

class Transform; //forward declaration, den ska bara känna till att det finns en class som heter Transform, denna pekar på transform och transform pekar på denna

class CameraObj : public Entity{
	struct CameraCBufferData {
		XMFLOAT4X4 view;
		XMFLOAT4X4 projection;
		float cameraPos[4];
	};

public:
	ID3D11Device * gDevice = nullptr;
	ID3D11DeviceContext * gDeviceContext = nullptr;

	void *cameraDataP = nullptr;
	char *name;
	CameraData cameraData;

	Transform *transform = nullptr; //så jag alltid kan komma åt transformvärdet direkt
	CameraCBufferData cameraCBufferData;
	ID3D11Buffer *cameraCbuffer = nullptr; //här ligger den storade cameradatan

	//XMFLOAT4X4 mview;
	//XMFLOAT4X4 mprojection;

	CameraObj(ID3D11Device *gDevice, ID3D11DeviceContext *gDevC) {
		this->gDevice = gDevice;
		this->gDeviceContext = gDevC;
		//CreateCameraCBuffer();
	}
	~CameraObj(){
		free(cameraDataP);
		cameraCbuffer->Release();
	}

	void UpdateCBuffer(UINT screenWidth, UINT screenHeight);
	void CreateCBuffer();

	void EmptyVariables() {
		free(cameraDataP);
		//cameraCbuffer->Release(); //inte säkert jag vill detta, kanske remapa istället! updatesubresource
	}
};