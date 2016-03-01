#pragma once
#ifndef MESH_H
#define MESH_H
#endif

//CRUCIAL
#include <windows.h>
#include <DirectXMath.h>
#include <DirectXMathMatrix.inl>
#include <DirectXMathVector.inl>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <memory>
//#include <SimpleMath.h>

using namespace DirectX;
//using namespace DirectX::SimpleMath;

#include "TA files/Entity.h"
#include "TA files/Material.h"

class Mesh : public Entity{
public:
	void *meshDataP = nullptr; //pointer to the current values, används endast för att ta bort all gammal data i ett svep
	char* name; //det riktiga namnet finns i Transformen
	int numberOfTransforms;
	std::vector<char*> transformNames;
	char* materialName;
	int meshID = 0;
	MeshData *meshData; //pekare till denna för att den har massa egna pekare nu, förstör objektet när det ska vara nya värden!
	NameMaxNameStruct *transformNamesStruct; //för att releasa

	int nrVertices;
	int nrIndecies;

	Vertex *vertices = nullptr; //dessa kan inte ligga i meshData då 
	IndexV *indecies = nullptr;
	UINT *indexList = nullptr; //denna som ska skapas en indexbuffer av, man vill ha id för vilken vertis som ska användas och inte vilken pos, nor och uv
	//std::vector<Vertex> vertices;	
	
	Material* material = nullptr;
	//int materialID = 0;;

	Mesh(){
	}
	~Mesh(){

	/*	delete(name);
		delete(meshData);*/
		free(meshDataP);
		delete(meshData); //ta bort denna oxå coz den är en pekare i meshDataP
		delete(transformNamesStruct);

		delete(vertices);
		delete(indecies);
	}
	void CreateBuffers();
	void EmptyVariables(); //används när meshen behöver bygga om värden (namn)
	void EmptyBuffers();
	//void EmptyBuffersAndArrays(); //används när meshen behöver bygga om värden (vertices)
	void CreateVertices();
	void CreateIndices();
	void CreateVertexBuffer(); //dessa använder sig av den aktiva vertices och indecies
	void CreateIndexBuffer();
	void RemapVertexBuffer();


private:

	HRESULT hr;
};
