#include "FileHandler.h"

FileHandler::FileHandler() {}
FileHandler::~FileHandler() {}


void FileHandler::SaveScene(int maxNameSize, char* sceneName, vector<Material*>& materials, vector<Transform*>& allTransforms,
                            vector<Transform*>& allMeshTransforms, vector<Transform*>& allLightTransforms)
{
    // max_Name_Size = maxNameSize;
    int sceneNameSize = CorrectName(sceneName);
    int nrMats = materials.size();
    int nrTransforms = allTransforms.size();
    int nrMeshes = allMeshTransforms.size();
    int nrLights = allLightTransforms.size();

    std::string fileName(sceneName, sceneNameSize - 1); //-1 så att inte \0 kommer med
    fileName.append(".drm");
    lastLoadedFileName = fileName;

    ofs.open(fileName, ofstream::out | ofstream::binary);
    if(ofs.is_open() == true)
    {
        ofs.clear();

        SaveMainHeader(sceneNameSize, sceneName, nrMats, nrTransforms, nrMeshes, nrLights);

        SaveMaterials(nrMats, materials); // måste vara först!
        SaveTransforms(nrTransforms, allTransforms);
        SaveMeshes(nrMeshes, allMeshTransforms);
        SaveLights(nrLights, allLightTransforms);

        ofs.close();

        // LoadScene(); //test bara
    }
}

void FileHandler::SaveMainHeader(int sceneNameSize, char* sceneName, int nrMats, int nrTransforms, int nrMeshes, int nrLights)
{
    ofs.write((char*)&sceneNameSize, sizeof(int));
    ofs.write(sceneName, sizeof(char) * sceneNameSize);

    ofs.write((char*)&nrMats, sizeof(int));
    ofs.write((char*)&nrTransforms, sizeof(int));
    ofs.write((char*)&nrMeshes, sizeof(int));
    ofs.write((char*)&nrLights, sizeof(int));
}

void FileHandler::SaveTransforms(int nrTransforms, vector<Transform*>& allTransforms)
{
    for(int i = 0; i < nrTransforms; i++)
    {
        char* parentName = allTransforms[i]->parentName;
        char* transformName = allTransforms[i]->name;
        int parentNameSize = CorrectName(parentName); // biter av vid nullterminator och returnerar längden på texten
        int transformNameSize = CorrectName(transformName);

        ofs.write((char*)&parentNameSize, sizeof(int));
        ofs.write((char*)&transformNameSize, sizeof(int));
        ofs.write(parentName, sizeof(char) * parentNameSize);
        ofs.write(transformName, sizeof(char) * transformNameSize);

        if(transformNameSize > 0) // den är dynamiskt allokerad!
        {
            delete(transformName);
        }
        if(parentNameSize > 0) // den är dynamiskt allokerad!
        {
            delete(parentName);
        }

        ofs.write((char*)&allTransforms[i]->transformData.pos, sizeof(allTransforms[i]->transformData.pos));
        ofs.write((char*)&allTransforms[i]->transformData.rot, sizeof(allTransforms[i]->transformData.rot));
        ofs.write((char*)&allTransforms[i]->transformData.scale, sizeof(allTransforms[i]->transformData.scale));
        ofs.write((char*)&allTransforms[i]->transformData.attributes, sizeof(CustomAttributes)); // funkar detta? troor deeettt
    }
}

void FileHandler::SaveMeshes(int nrMeshes, vector<Transform*>& allMeshTransforms)
{
    for(int i = 0; i < nrMeshes; i++)
    {

        // headerSTART****
        int nrOfTransforms = allMeshTransforms[i]->mesh->numberOfTransforms;
        ofs.write((char*)&nrOfTransforms, sizeof(int));

        for(int t = 0; t < nrOfTransforms; t++) // loopa igenom alla denna meshens transforms
        {
            char* transformName = allMeshTransforms[i]->mesh->transformNames[t]; // hade nog egentligen kunnat använda transformnamnet då det är
            // samma, men consistency u know
            int transformNameSize = CorrectName(transformName);
            ofs.write((char*)&transformNameSize, sizeof(int));
            ofs.write(transformName, sizeof(char) * transformNameSize);

            if(transformNameSize > 0) // den är dynamiskt allokerad!
            {
                delete(transformName);
            }
        }

        char* meshName = allMeshTransforms[i]->mesh->name;
        int meshNameSize = CorrectName(meshName); // biter av vid nullterminator och returnerar längden på texten

        ofs.write((char*)&meshNameSize, sizeof(int));
        ofs.write(meshName, sizeof(char) * meshNameSize);

        if(meshNameSize > 0) // den är dynamiskt allokerad!
        {
            delete(meshName);
        }
        /*ofs.write((char*)&allMeshTransforms[i]->mesh->nrVertices, sizeof(int));
        ofs.write((char*)&allMeshTransforms[i]->mesh->nrIndecies, sizeof(int));
        */
        char* materialName = allMeshTransforms[i]->mesh->materialName;
        int materialNameSize = CorrectName(materialName);

        ofs.write((char*)&materialNameSize, sizeof(int));
        ofs.write(materialName, sizeof(char) * materialNameSize);

        if(materialNameSize > 0) // den är dynamiskt allokerad!
        {
            delete(materialName);
        }
        // headerEND****

        // messageSTART****
        ofs.write((char*)&allMeshTransforms[i]->mesh->meshID, sizeof(int));

        ofs.write((char*)&allMeshTransforms[i]->mesh->meshData->nrPos, sizeof(int));
        ofs.write((char*)&allMeshTransforms[i]->mesh->meshData->nrNor, sizeof(int));
        ofs.write((char*)&allMeshTransforms[i]->mesh->meshData->nrUV, sizeof(int));
        ofs.write((char*)&allMeshTransforms[i]->mesh->meshData->nrI, sizeof(int));
        ofs.write((char*)&allMeshTransforms[i]->mesh->meshData->triangleCount, sizeof(int));

        ofs.write((char*)allMeshTransforms[i]->mesh->meshData->positions,
                  sizeof(Float3) * allMeshTransforms[i]->mesh->meshData->nrPos); //?? pekare till float3s
        ofs.write((char*)allMeshTransforms[i]->mesh->meshData->normals, sizeof(Float3) * allMeshTransforms[i]->mesh->meshData->nrNor);
        ofs.write((char*)allMeshTransforms[i]->mesh->meshData->uvs, sizeof(Float2) * allMeshTransforms[i]->mesh->meshData->nrUV);

        ofs.write((char*)allMeshTransforms[i]->mesh->meshData->indexPositions, sizeof(int) * allMeshTransforms[i]->mesh->meshData->nrI);
        ofs.write((char*)allMeshTransforms[i]->mesh->meshData->indexNormals, sizeof(int) * allMeshTransforms[i]->mesh->meshData->nrI);
        ofs.write((char*)allMeshTransforms[i]->mesh->meshData->indexUVs, sizeof(int) * allMeshTransforms[i]->mesh->meshData->nrI);
        ofs.write((char*)allMeshTransforms[i]->mesh->meshData->trianglesPerFace, sizeof(int) * allMeshTransforms[i]->mesh->meshData->triangleCount);
        // messageEND****
    }
}

void FileHandler::SaveMaterials(int nrMats, vector<Material*>& materials)
{
    for(int i = 1; i < nrMats; i++)
    { // läs inte default materialet!
        char* materialName = materials[i]->name;
        int materialNameSize = CorrectName(materialName);

        ofs.write((char*)&materialNameSize, sizeof(int));
        ofs.write(materialName, sizeof(char) * materialNameSize);

        if(materialNameSize > 0) // den är dynamiskt allokerad!
        {
            delete(materialName);
        }
        ofs.write((char*)&materials[i]->materialData, sizeof(MaterialData));

        // textures!!
        char* diffuseTextureName = materials[i]->textureName;
        // int diffuseTextureSize = CorrectName(diffuseTextureName);
        int diffuseTextureSize = CutOffFilePath(diffuseTextureName);

        ofs.write((char*)&diffuseTextureSize, sizeof(int));
        ofs.write(diffuseTextureName, sizeof(char) * diffuseTextureSize);


        if(diffuseTextureSize > 0) // den är dynamiskt allokerad!
        {
            delete(diffuseTextureName);
        }

        // glow
        char* glowTextureName = materials[i]->glowMapName;
        // int diffuseTextureSize = CorrectName(diffuseTextureName);
        int glowTextureSize = CutOffFilePath(glowTextureName);

        ofs.write((char*)&glowTextureSize, sizeof(int));
        ofs.write(glowTextureName, sizeof(char) * glowTextureSize);


        if(glowTextureSize > 0) // den är dynamiskt allokerad!
        {
            delete(glowTextureName);
        }
    }
}

void FileHandler::SaveLights(int nrLights, vector<Transform*>& allLightTransforms)
{
    for(int i = 0; i < nrLights; i++)
    {
        char* transformName = allLightTransforms[i]->name;
        char* lightName = allLightTransforms[i]->light->name;
        int transformNameSize = CorrectName(transformName);
        int lightNameSize = CorrectName(lightName);

        ofs.write((char*)&transformNameSize, sizeof(int));
        ofs.write((char*)&lightNameSize, sizeof(int));
        ofs.write(transformName, sizeof(char) * transformNameSize);
        ofs.write(lightName, sizeof(char) * lightNameSize);

        if(transformNameSize > 0) // den är dynamiskt allokerad!
        {
            delete(transformName);
        }
        if(lightNameSize > 0) // den är dynamiskt allokerad!
        {
            delete(lightName);
        }
        // lightdata!!
        ofs.write((char*)&allLightTransforms[i]->light->lightData, sizeof(LightData));
    }
}


void FileHandler::LoadScene()
{
    string testFileName;
    if(lastLoadedFileName.empty() == false)
    {
        testFileName = lastLoadedFileName;
    }
    else
    {
        testFileName = "test.drm";
    }

    ifs.open(testFileName, ifstream::in | ofstream::binary);
    if(ifs.is_open() == true)
    {
        // testa o läsa lite

        int sceneNameSize;
        ifs.read((char*)&sceneNameSize, sizeof(int));
        char* sceneName = new char[sceneNameSize];
        ifs.read((char*)sceneName, sizeof(char) * sceneNameSize);

        int nrMats, nrTransforms, nrMeshes, nrLights;
        ifs.read((char*)&nrMats, sizeof(int));
        ifs.read((char*)&nrTransforms, sizeof(int));
        ifs.read((char*)&nrMeshes, sizeof(int));
        ifs.read((char*)&nrLights, sizeof(int));

        // ladda material
        for(int i = 1; i < nrMats; i++) // defualt material, så kör inte hela nrMats
        {
            int materialNameSize;
            ifs.read((char*)&materialNameSize, sizeof(int));
            char* materialName = new char[materialNameSize];
            ifs.read((char*)materialName, sizeof(char) * materialNameSize);

            MaterialData materialData;
            ifs.read((char*)&materialData.mapMasks, sizeof(int));
            ifs.read((char*)&materialData.diffuse, sizeof(float));
            ifs.read((char*)&materialData.color, sizeof(float) * 3);
            ifs.read((char*)&materialData.ambColor, sizeof(float) * 3);
            ifs.read((char*)&materialData.specColor, sizeof(float) * 3);
            ifs.read((char*)&materialData.specCosine, sizeof(float));
            ifs.read((char*)&materialData.specEccentricity, sizeof(float));
            ifs.read((char*)&materialData.specRollOff, sizeof(float));

            int diffuseTextureNameSize;
            ifs.read((char*)&diffuseTextureNameSize, sizeof(int));
            char* diffuseTextureName = new char[diffuseTextureNameSize];
            ifs.read((char*)diffuseTextureName, sizeof(char) * diffuseTextureNameSize);
        }
        // ladda transforms
        for(int i = 0; i < nrTransforms; i++)
        {
            int parentNameSize;
            int transformNameSize;

            ifs.read((char*)&parentNameSize, sizeof(int));
            ifs.read((char*)&transformNameSize, sizeof(int));

            char* parentName = new char[parentNameSize];
            char* transformName = new char[transformNameSize];

            ifs.read((char*)parentName, sizeof(char) * parentNameSize);
            ifs.read((char*)transformName, sizeof(char) * transformNameSize);

            TransformData transformData;

            ifs.read((char*)&transformData.pos, sizeof(float) * 3);
            ifs.read((char*)&transformData.rot, sizeof(float) * 4); // quaternion tror jag
            ifs.read((char*)&transformData.scale, sizeof(float) * 3);
        }
        // ladda meshes
        for(int i = 0; i < nrMeshes; i++)
        {
            int transformNameSize;
            int meshNameSize;

            ifs.read((char*)&transformNameSize, sizeof(int));
            ifs.read((char*)&meshNameSize, sizeof(int));

            char* transformName = new char[transformNameSize];
            char* meshName = new char[meshNameSize];

            ifs.read((char*)transformName, sizeof(char) * transformNameSize);
            ifs.read((char*)meshName, sizeof(char) * meshNameSize);


            int materialNameSize;
            ifs.read((char*)&materialNameSize, sizeof(int));

            char* materialName = new char[materialNameSize];
            ifs.read((char*)materialName, sizeof(char) * materialNameSize);

            // messageSTART****
            int meshID;
            ifs.read((char*)&meshID, sizeof(int));

            MeshData meshData;

            ifs.read((char*)&meshData.nrPos, sizeof(int));
            ifs.read((char*)&meshData.nrNor, sizeof(int));
            ifs.read((char*)&meshData.nrUV, sizeof(int));
            ifs.read((char*)&meshData.nrI, sizeof(int));
            ifs.read((char*)&meshData.triangleCount, sizeof(int));

            meshData.positions = new Float3[meshData.nrPos];
            meshData.normals = new Float3[meshData.nrNor];
            meshData.uvs = new Float2[meshData.nrUV];

            meshData.indexPositions = new int[meshData.nrI];
            meshData.indexNormals = new int[meshData.nrI];
            meshData.indexUVs = new int[meshData.nrI];
            meshData.trianglesPerFace = new int[meshData.triangleCount];

            ifs.read((char*)&meshData.positions, sizeof(Float3) * meshData.nrPos);
            ifs.read((char*)&meshData.normals, sizeof(Float3) * meshData.nrNor);
            ifs.read((char*)&meshData.uvs, sizeof(Float2) * meshData.nrUV);

            ifs.read((char*)&meshData.indexPositions, sizeof(int) * meshData.nrI);
            ifs.read((char*)&meshData.indexNormals, sizeof(int) * meshData.nrI);
            ifs.read((char*)&meshData.indexUVs, sizeof(int) * meshData.nrI);
            ifs.read((char*)&meshData.trianglesPerFace, sizeof(int) * meshData.triangleCount);
        }

        for(int i = 0; i < nrLights; i++)
        {
            int transformNameSize;
            int lightNameSize;

            ifs.read((char*)&transformNameSize, sizeof(int));
            ifs.read((char*)&lightNameSize, sizeof(int));

            char* transformName = new char[transformNameSize];
            char* lightName = new char[lightNameSize];

            ifs.read((char*)transformName, sizeof(char) * transformNameSize);
            ifs.read((char*)lightName, sizeof(char) * lightNameSize);

            LightData lightData;

            ifs.read((char*)&lightData.type, sizeof(int));
            ifs.read((char*)&lightData.decayType, sizeof(int));
            ifs.read((char*)&lightData.intensity, sizeof(float));
            ifs.read((char*)&lightData.colorDiffuse, sizeof(Float3));
            ifs.read((char*)&lightData.direction, sizeof(Float3));
            ifs.read((char*)&lightData.dropOff, sizeof(float));
            ifs.read((char*)&lightData.coneAngle, sizeof(float));
            ifs.read((char*)&lightData.penumAgle, sizeof(float));
        }
    }
}


// int FileHandler::CorrectName(char *&referenceName) { //kör tills nollbyten och biter av resterande chars
//	char *tempName = nullptr;
//	int nameSize = 0;
//	if (referenceName != nullptr)
//	{
//		for (int i = 0; i < MAX_NAME_SIZE; i++) {
//			if (referenceName[i] == 0) { //nullterminator!!!!!!!!!!!
//				break;
//			}
//			nameSize++; //här ??
//		}
//
//		tempName = new char[nameSize];
//		for (int i = 0; i < nameSize; i++) {
//			tempName[i] = referenceName[i];
//		}
//
//		//delete(referenceName); haha arrayen som den pekar på är ju fan statisk!
//		//referenceName = new char[nameSize];
//		referenceName = tempName;
//		return nameSize;
//	}
//	else
//	{
//		return 0;
//	}
//}