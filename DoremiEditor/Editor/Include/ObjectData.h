#pragma once
#ifndef OBJECTDATA_H
#define OBJECTDATA_H
#endif

//CRUCIAL
#include <windows.h>
#include <DirectXMath.h>
#include <DirectXMathMatrix.inl>
#include <DirectXMathVector.inl>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <memory>
//#include <SimpleMath.h>

using namespace DirectX;
//using namespace DirectX::SimpleMath;

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

const int MAX_NAME_SIZE = 100;

struct NameMaxNameStruct {
	char name[100];
};

struct Float2{
	float u, v;
	Float2(){}
	Float2(float fu, float fv){
		u = fu;
		v = fv;
	}

	void Set(float fu, float fv){
		u = fu;
		v = fv;
	}
};

struct Float3{
	float x, y, z;
	Float3(){
		x = 0;
		y = 0;
		z = 0;
	}
	Float3(float fx, float fy, float fz){
		x = fx;
		y = fy;
		z = fz;
	}

	void Set(float fx, float fy, float fz){
		x = fx;
		y = fy;
		z = fz;
	}
};

struct Float4{
	float x, y, z, w;
	Float4(){
		x = 0;
		y = 0;
		z = 0;
		w = 1; //hmm?
	}
	Float4(float fx, float fy, float fz, float fw){
		x = fx;
		y = fy;
		z = fz;
		w = fw;
	}

	void Set(float fx, float fy, float fz, float fw){
		x = fx;
		y = fy;
		z = fz;
		w = fw;
	}
};

struct Int3{
	int x, y, z;
	Int3(){}
	Int3(int fx, int fy, int fz){
		x = fx;
		y = fy;
		z = fz;
	}

	void Set(int fx, int fy, int fz){
		x = fx;
		y = fy;
		z = fz;
	}
};
struct Int2{
	int u, v;
	Int2(){}
	Int2(int fu, int fv){
		u = fu;
		v = fv;
	}

	void Set(int fu, int fv){
		u = fu;
		v = fv;
	}
};

struct Vertex{
	Float3 pos, nor;
	Float2 uv;

	Vertex(){}
	~Vertex(){}
	Vertex(float px, float py, float pz, float nx, float ny, float nz, float cu, float cv){
		pos.Set(px, py, pz);
		nor.Set(nx, ny, nz);
		uv.Set(cu, cv);
	}
	
};
struct IndexV{
	int posI, norI, uvI;

	IndexV(){}
	~IndexV(){}
	IndexV(int p, int n, int u){
		posI = p;
		norI = n;
		uvI = u;
	}

};

struct CustomAttributes
{
    bool isRendered;
    bool isBBox;
    bool isCollider;
    bool isInteractable;
    int checkPointID;
    int spawnPointID;
    int startOrEndPoint;
    bool isAIground;

    bool frequencyAffected;
    float interactableRange;
    Float3 interactableStartPos;
    Float3 interactableEndPos;
    Float2 interactableOffset;
    float interactableStartSpeed;
    float interactableEndSpeed;
    bool isPotentialFieldCollidable;
    int potentialFieldID;
    int potentialFieldNeighbour1;
    int potentialFieldNeighbour2;
    int potentialFieldNeighbour3;
    int potentialFieldNeighbour4;
    int potentialFieldNeighbour5;

    bool isSpawner;
    int spawnMax;
    int spawnMaxAlive;
    int spawnTypeBlueprint;
    float spawnFrequency;

    bool isStatic;
    int physicsType;

    int typeBlueprint;

    bool isDangerous;
};
struct TransformData
{
	Float3 pos;
	Float4 rot;
	Float3 scale;
	CustomAttributes attributes;
};

struct CameraData{
	int			isOrtho;
	float		target[3];
	float		upVector[3];
	float		rightVector[3];
	float		hAngle; //hor-FOV
};

struct MeshData{
	int nrPos, nrNor, nrUV, nrI;
	int triangleCount;
	Float3 *positions;
	Float3 *normals;
	Float2 *uvs;

	int *indexPositions;
	int *indexNormals;
	int *indexUVs;
	//int *indexTriangles;
	int *trianglesPerFace;
	//Material *material; //pekar på ett specifikt material i en vektor av material
	~MeshData(){
		delete(positions);
		delete(normals);
		delete(uvs);
		
		delete(indexPositions);
		delete(indexNormals);
		delete(indexUVs);
		delete(trianglesPerFace);
	}
};

struct MaterialData{
	MaterialData()
	{
		mapMasks = 0;
		diffuse = 0;
		color[0] = color[1] = color[2] = 0.5f;
		ambColor[0] = ambColor[1] = ambColor[2] = 0.0f;
		specColor[0] = specColor[1] = specColor[2] = 0.0f;
		specCosine = specEccentricity = specRollOff = 0;
	}
	int mapMasks;
	float diffuse;
	float color[3];
	float ambColor[3];
	float specColor[3];
	float specCosine;
	float specEccentricity;
	float specRollOff;
};

struct LightData{
	int type; //0 = def, 1 = dir, 2 = spot, 3 = point
	int decayType; //0 = none, 1 = linear, 2 = quadratic (l/d**v)
	float intensity;
	Float3 colorDiffuse;
	Float3 direction;
	float dropOff;
	float coneAngle;
	float penumAgle;

};

struct LightCBufferData { //ett ljus
	UINT type; //0 = def, 1 = dir, 2 = spot, 3 = point
	Float3 attenuation; //0 = none, 1 = linear, 2 = quadratic (l/d**v)
	float intensity;
	Float3 colorDiffuse;
	float coneAngle;
	Float3 direction;
	float penumAgle;
	Float3 position;
};

//struct LightCBufferData { //ett ljus
//	UINT type; //0 = def, 1 = dir, 2 = spot, 3 = point
//	UINT decayType; //0 = none, 1 = linear, 2 = quadratic (l/d**v)
//	float intensity;
//	Float3 colorDiffuse;
//	Float3 direction;
//	float dropOff;
//	float coneAngle;
//	float penumAgle;
//	//Float3 position;
//};

struct NameMessage //ligger här coz reasons
{
	char name1[MAX_NAME_SIZE]; //det nya namnet vid namechange
	char name2[MAX_NAME_SIZE];
};

static int CorrectName(char *&referenceName) { //kör tills nollbyten och biter av resterande chars
	char *tempName = nullptr;//tänk på att de sparade namnen kommer ha färre tecken än de som laddas in från maya, men den skapar en ny char array så den skriver inte över gamla variabler
	int nameSize = 0;
	if (referenceName != nullptr && referenceName[0] != 0)
	{
		for (int i = 0; i < MAX_NAME_SIZE; i++) 
		{		
			nameSize++; //här ??
			if (referenceName[i] == 0) //nullterminator!!!!!!!!!!!
			{
				break;
			}
		}

		tempName = new char[nameSize];
		for (int i = 0; i < nameSize; i++) {
			tempName[i] = referenceName[i];
		}

		//delete(referenceName); haha arrayen som den pekar på är ju fan statisk!
		//referenceName = new char[nameSize];
		referenceName = tempName;
		return nameSize;
	}
	else
	{
		return 0;
	}
}

static int CorrectName(char *&referenceName, bool removeOldName) { //kör tills nollbyten och biter av resterande chars
	char *tempName = nullptr;//tänk på att de sparade namnen kommer ha färre tecken än de som laddas in från maya, men den skapar en ny char array så den skriver inte över gamla variabler
	int nameSize = 0;
	if (referenceName != nullptr && referenceName[0] != 0)
	{
		for (int i = 0; i < MAX_NAME_SIZE; i++)
		{
			nameSize++; //här ??
			if (referenceName[i] == 0) //nullterminator!!!!!!!!!!!
			{
				break;
			}
		}

		tempName = new char[nameSize];
		for (int i = 0; i < nameSize; i++) {
			tempName[i] = referenceName[i];
		}

		if (removeOldName == true)
		{
			delete(referenceName);
			referenceName = nullptr;
		}
		referenceName = tempName;
		return nameSize;
	}
	else
	{
		//CorrectName(referenceName); ??
		return 0;
	}
}

static int CutOffFilePath(char *&referenceName)
{
	if (referenceName[0] != 0)
	{
		CorrectName(referenceName);
		char *tempName = referenceName;
		char *name = referenceName;

		
		tempName = strtok(name, "/\\");
		while (tempName != NULL)
		{
			name = tempName; //spara den tidigare iterationen, varför funkar detta utan att allokera nytt minne?? den borde ju ha exakt samma värde som name?
			tempName = strtok(NULL, "/\\"); //cutta nästa del
		}

		//delete(referenceName); //den blev ny allokerad i CorrectName funktionen, riskyyyyy ifall någ
		//referenceName = name;

        int nameSizeTemp = CorrectName(name);
        char* newName = new char[nameSizeTemp];
        for (int i = 0; i < nameSizeTemp; i++)
        {
            newName[i] = name[i];
        }
        delete(referenceName); //den blev ny allokerad i CorrectName funktionen, kommer den ta bort hela arrayen eller bara den cuttade?
        referenceName = newName;

		int nameSize = CorrectName(referenceName);
		//cut tempMaterialName by
		return nameSize;
	}
	else return 0;
}
