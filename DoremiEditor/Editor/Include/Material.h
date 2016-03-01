#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H
#endif

#include "Entity.h"

class Material : public Entity{
public:
	struct MaterialCBufferData {
		float diffuse;
		float color[3];
		float ambColor[3];
		float specColor[3];
		float specCosine;
		float specEccentricity;
		float specRollOff;
		UINT hasTexture;
		float padding[2];

		MaterialCBufferData()
		{
			diffuse = 0;
			color[0] = color[1] = color[2] = 0;
			ambColor[0] = ambColor[1] = ambColor[2] = 0;
			specColor[0] = specColor[1] = specColor[2] = 0;
			specCosine = 0;
			specEccentricity = 0;
			specRollOff = 0;
			hasTexture = 0;
			padding[0] = padding[1] = 0;
		}
	};
	
	ID3D11Device * gDevice = nullptr;
	ID3D11DeviceContext * gDeviceContext = nullptr;

	void *materialDataP = nullptr; //pointer to the current values, för att ta bort messaget som varit mallocat
	char *name;
	char *textureName; //store filpathsen, även om jag har texturen så kommer vi behöva pathsen för fileformatet sen!
	char *bumpMapName;
	char *specularMapName;
	char *glowMapName;
	char dummyName[100]; //dummy variable som används som default namn, behöver inte deallokeras
	MaterialData materialData;
	MaterialCBufferData materialCBufferData;
	ID3D11Buffer *materialCbuffer = nullptr; //här ligger den storade materialdatan

	//char *textureName;
	ID3D11Resource *diffuseTexture = nullptr;
	ID3D11ShaderResourceView *diffuseTextureView = nullptr;

	ID3D11Resource *bumpTexture = nullptr;
	ID3D11ShaderResourceView *bumpTextureView = nullptr;

	ID3D11Resource *specularTexture = nullptr;
	ID3D11ShaderResourceView *specularTextureView = nullptr;

	ID3D11Resource *glowTexture = nullptr;
	ID3D11ShaderResourceView *glowTextureView = nullptr;

	Material(ID3D11Device *gDevice, ID3D11DeviceContext *gDevC){
		this->gDevice = gDevice;
		this->gDeviceContext = gDevC;

		name = dummyName; //sätter den till dummy namnet bara för att ha ett default
		textureName = dummyName;
		bumpMapName = dummyName;
		specularMapName = dummyName;
		glowMapName = dummyName;
	}
	~Material(){
		//delete(name); den är statiskt allokerad
		materialCbuffer->Release();
		free(materialDataP);

		diffuseTexture->Release();
		diffuseTextureView->Release();
        bumpTexture->Release();
        bumpTextureView->Release();
		specularTexture->Release();
		specularTextureView->Release();
        glowTexture->Release();
        glowTextureView->Release();
	}
	//skapa constantbuffer här???
	void UpdateCBuffer();
	void CreateCBuffer();
	void CreateTexture(char* filePath, ID3D11Resource *&texture, ID3D11ShaderResourceView *&textureView);

	void EmptyVariables(){
		free(materialDataP);
		//materialCbuffer->Release(); //inte säkert jag vill detta, kanske remapa istället! updatesubresource??
	}

};