#pragma once

#ifndef FILEHANDLER_H
#define FILEHANDLER_H
#endif

#include "TA files/Mesh.h"
#include "TA files/Light.h"
#include "TA files/Transform.h"
#include "TA files/Material.h"
#include "TA files/ObjectData.h"
//behöver tillgång till dessa, vill vi flytta fileloadern så måste dessa oxå finnas där

#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

class FileHandler {
public:
	FileHandler();
	~FileHandler();

	ofstream ofs; //skriv till fil
	ifstream ifs; //läs från fil
	string lastLoadedFileName;

	void SaveScene(int maxNameSize, char* sceneName,
		vector<Material*> &materials,
		vector<Transform*> &allTransforms,
		vector<Transform*> &allMeshTransforms,
		vector<Transform*> &allLightTransforms);


	void LoadScene(); //scene class??

private:
	//int CorrectName(char *&referenceName); //tänk på att de sparade namnen kommer ha färre tecken än de som laddas in från maya, men den skapar en ny char array så den skriver inte över gamla variabler

	void SaveMainHeader(int sceneNameSize, char* sceneName, int nrMats, int nrTransforms, int nrMeshes, int nrLights);

	void SaveMaterials(int nrMats, vector<Material*> &materials);
	void SaveTransforms(int nrTransforms, vector<Transform*> &allTransforms);
	void SaveMeshes(int nrMeshes, vector<Transform*> &allMeshTransforms);
	void SaveLights(int nrLights, vector<Transform*> &allLightTransforms);
	//Scene currentScene;
	
};