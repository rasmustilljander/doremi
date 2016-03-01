#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H
#endif

#include "TA files/Entity.h"

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

	void *materialDataP = nullptr; //pointer to the current values, för att ta bort messaget som varit mallocat
	char *name;
	char *textureName; //store filpathsen, även om jag har texturen så kommer vi behöva pathsen för fileformatet sen!
	char *bumpMapName;
	char *specularMapName;
	char *glowMapName;
	char dummyName[100]; //dummy variable som används som default namn, behöver inte deallokeras
	MaterialData materialData;
	MaterialCBufferData materialCBufferData;

	Material(){

		name = dummyName; //sätter den till dummy namnet bara för att ha ett default
		textureName = dummyName;
		bumpMapName = dummyName;
		specularMapName = dummyName;
		glowMapName = dummyName;
	}
	~Material(){
		//delete(name); den är statiskt allokerad
		free(materialDataP);
	}
	//skapa constantbuffer här???
	void UpdateCBuffer();
	void CreateCBuffer();
	void CreateTexture(char* filePath);

	void EmptyVariables(){
		free(materialDataP);
		//materialCbuffer->Release(); //inte säkert jag vill detta, kanske remapa istället! updatesubresource??
	}

};