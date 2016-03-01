#pragma once
#ifndef TRANSFORM_H
#define TRANSFORM_H
#endif


#include "Entity.h"
#include "Mesh.h"
#include "CameraObj.h"
#include "Light.h"

class Transform : public Entity{
	struct TransformCBufferData
	{
		XMFLOAT4X4 world;
	};
public:
	void *transformDataP = nullptr; ///pointer to the current values, används endast för att ta bort all gammal data i ett svep
	char *name;
	char *parentName; //när jag laddar till fil så ladda namnet från parent variabeln istället, annars finns det risk att parentNamnet är ändrat med Renamed grejen
	TransformData transformData;

	Transform *parent = nullptr; //använd parenten och hämta dess transformation
	TransformCBufferData transformCBufferData;
	ID3D11Buffer *transformCBuffer;

	//kommer ha en av dessa, placera dem i olika vektorer!
	Mesh *mesh = nullptr;
	Light *light = nullptr;
	CameraObj *camera = nullptr; //fick döpa om den till obj pga oskeeaaar

	Transform(ID3D11Device* gd, ID3D11DeviceContext* gdc)
	{
		this->gDevice = gd; //freea dessa inte här, görs i main duuh
		this->gDeviceContext = gdc;
		CreateTransformCBuffer();
	}
	Transform()
	{}
	~Transform()
	{ //kanske deleta saker som mesh, men möjligt vi skickar separata deletes för dem, detsamma gäller children
		//delete(name);
		free(transformDataP);
	}
	void UpdateCBuffer();
	void CreateTransformCBuffer();

	void EmptyVariables(){
		free(transformDataP);
	}
private:
	ID3D11Device* gDevice = nullptr;
	ID3D11DeviceContext* gDeviceContext = nullptr;
};