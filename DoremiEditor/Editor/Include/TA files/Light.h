#pragma once
#ifndef LIGHT_H
#define LIGHT_H
#endif

#include "TA files/Entity.h"
//#include "Transform.h"

class Transform; //forward declaration, den ska bara känna till att det finns en class som heter Transform, denna pekar på transform och transform pekar på denna

class Light : public Entity{
	
public:
	void *lightDataP = nullptr;
	char *name;
	LightData lightData;

	Transform *transform = nullptr; //så jag alltid kan komma åt transformvärdet direkt
	LightCBufferData lightCBufferData; //datan som skickas in i cbuffern, structen ligger i ObjectData
	
	Light() {
	}
	~Light(){
		free(lightDataP);
		//lightCbuffer->Release();
	}

	void UpdateCBuffer();
	void CreateCBuffer();

	void EmptyVariables() {
		free(lightDataP);
		//delete[] (name);
		//cameraCbuffer->Release(); //inte säkert jag vill detta, kanske remapa istället! updatesubresource
	}
};