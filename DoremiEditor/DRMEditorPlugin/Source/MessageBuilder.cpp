#include <DoremiEditor\DRMEditorPlugin\Include\MessageBuilder.hpp>
#include <DoremiEditor\DRMEditorPlugin\Include\ApplicationContext.hpp>
#include <DoremiEditor\DRMEditorPlugin\Include\NodeHandler.hpp>

namespace DoremiEditor
{
    namespace Plugin
    {
        TransformInfo MessageBuilder::GetTransformData(std::string p_nodeName)
        {
            try
            {
                TransformInfo o_data;
                MStatus result;
                MString t_nodeNameMString(p_nodeName.c_str());
                MSelectionList t_selectionList;
                MDagPath t_dagPath;
                if(MGlobal::getSelectionListByName(t_nodeNameMString, t_selectionList))
                {
                    t_selectionList.getDagPath(0, t_dagPath);
                    if(t_dagPath.hasFn(MFn::kTransform))
                    {
                        MFnTransform t_transform(t_dagPath.node(), &result);

                        // Fill Names
                        o_data.nodeName = p_nodeName;
                        if(t_transform.parent(0).hasFn(MFn::kTransform))
                        {
                            MFnTransform t_parent(t_transform.parent(0), &result);
                            o_data.parentName = t_parent.name().asChar();
                        }
                        else
                        {
                            o_data.parentName = "";
                        }

                        // Fill Matrix data
                        MTransformationMatrix t_transformMatrix = t_dagPath.inclusiveMatrix(&result);
                        MVector t_translation = t_transformMatrix.getTranslation(MSpace::kWorld, &result);
                        double t_rotations[4];
                        double t_scale[3];
                        t_transformMatrix.getRotationQuaternion(t_rotations[0], t_rotations[1], t_rotations[2], t_rotations[3]);
                        t_transformMatrix.getScale(t_scale, MSpace::kWorld);

                        o_data.data.translation[0] = t_translation.x;
                        o_data.data.translation[1] = t_translation.y;
                        o_data.data.translation[2] = t_translation.z;

                        o_data.data.rotation[0] = t_rotations[0] * -1.0f;
                        o_data.data.rotation[1] = t_rotations[1] * -1.0f;
                        o_data.data.rotation[2] = t_rotations[2] * -1.0f;
                        o_data.data.rotation[3] = t_rotations[3] * -1.0f;

                        o_data.data.scale[0] = t_scale[0];
                        o_data.data.scale[1] = t_scale[1];
                        o_data.data.scale[2] = t_scale[2];

                        MPlug tempPlug = t_transform.findPlug("drmRender");
                        tempPlug.getValue(o_data.data.attributes.isRendered);
                        tempPlug = t_transform.findPlug("drmBBox");
                        tempPlug.getValue(o_data.data.attributes.isBBox);
                        tempPlug = t_transform.findPlug("drmCollidable");
                        tempPlug.getValue(o_data.data.attributes.isCollider);
                        tempPlug = t_transform.findPlug("drmInteractable");
                        tempPlug.getValue(o_data.data.attributes.isInteractable);
                        tempPlug = t_transform.findPlug("drmCheckPointID");
                        tempPlug.getValue(o_data.data.attributes.checkPointID);
                        tempPlug = t_transform.findPlug("drmSpawnPointID");
                        tempPlug.getValue(o_data.data.attributes.spawnPointID);
                        tempPlug = t_transform.findPlug("drmLevelStartEnd");
                        tempPlug.getValue(o_data.data.attributes.startOrEndPoint);
                        tempPlug = t_transform.findPlug("drmAIground");
                        tempPlug.getValue(o_data.data.attributes.isAIground);

                        tempPlug = t_transform.findPlug("drmFrequencyAffected");
                        tempPlug.getValue(o_data.data.attributes.frequencyAffected);
                        tempPlug = t_transform.findPlug("drmInteractableRange");
                        tempPlug.getValue(o_data.data.attributes.interactableRange);

                        tempPlug = t_transform.findPlug("drmInteractableStartPosX");
                        tempPlug.getValue(o_data.data.attributes.interactableStartPos[0]);
                        tempPlug = t_transform.findPlug("drmInteractableStartPosY");
                        tempPlug.getValue(o_data.data.attributes.interactableStartPos[1]);
                        tempPlug = t_transform.findPlug("drmInteractableStartPosZ");
                        tempPlug.getValue(o_data.data.attributes.interactableStartPos[2]);
                        tempPlug = t_transform.findPlug("drmInteractableEndPosX");
                        tempPlug.getValue(o_data.data.attributes.interactableEndPos[0]);
                        tempPlug = t_transform.findPlug("drmInteractableEndPosY");
                        tempPlug.getValue(o_data.data.attributes.interactableEndPos[1]);
                        tempPlug = t_transform.findPlug("drmInteractableEndPosZ");
                        tempPlug.getValue(o_data.data.attributes.interactableEndPos[2]);
                        tempPlug = t_transform.findPlug("drmInteractableOffsetX");
                        tempPlug.getValue(o_data.data.attributes.interactableOffset[0]);
                        tempPlug = t_transform.findPlug("drmInteractableOffsetY");
                        tempPlug.getValue(o_data.data.attributes.interactableOffset[1]);

                        tempPlug = t_transform.findPlug("drmInteractableStartSpeed");
                        tempPlug.getValue(o_data.data.attributes.interactableStartSpeed);
                        tempPlug = t_transform.findPlug("drmInteractableEndSpeed");
                        tempPlug.getValue(o_data.data.attributes.interactableStartSpeed);

                        tempPlug = t_transform.findPlug("drmIsPotentialFieldCollidable");
                        tempPlug.getValue(o_data.data.attributes.isPotentialFieldCollidable);
                        tempPlug = t_transform.findPlug("drmPotentialFieldID");
                        tempPlug.getValue(o_data.data.attributes.potentialFieldID);
                        tempPlug = t_transform.findPlug("drmPotentialFieldNeighbour1");
                        tempPlug.getValue(o_data.data.attributes.potentialFieldNeighbour1);
                        tempPlug = t_transform.findPlug("drmPotentialFieldNeighbour2");
                        tempPlug.getValue(o_data.data.attributes.potentialFieldNeighbour2);
                        tempPlug = t_transform.findPlug("drmPotentialFieldNeighbour3");
                        tempPlug.getValue(o_data.data.attributes.potentialFieldNeighbour3);
                        tempPlug = t_transform.findPlug("drmPotentialFieldNeighbour4");
                        tempPlug.getValue(o_data.data.attributes.potentialFieldNeighbour4);
                        tempPlug = t_transform.findPlug("drmPotentialFieldNeighbour5");
                        tempPlug.getValue(o_data.data.attributes.potentialFieldNeighbour5);

                        tempPlug = t_transform.findPlug("drmIsSpawner");
                        tempPlug.getValue(o_data.data.attributes.isSpawner);
                        tempPlug = t_transform.findPlug("drmSpawnsMax");
                        tempPlug.getValue(o_data.data.attributes.spawnMax);
                        tempPlug = t_transform.findPlug("drmSpawnsMaxAlive");
                        tempPlug.getValue(o_data.data.attributes.spawnMaxAlive);
                        tempPlug = t_transform.findPlug("drmSpawnTypeBlueprint");
                        tempPlug.getValue(o_data.data.attributes.spawnTypeBlueprint);
                        tempPlug = t_transform.findPlug("drmSpawnFrequency");
                        tempPlug.getValue(o_data.data.attributes.spawnFrequency);

                        tempPlug = t_transform.findPlug("drmIsStatic");
                        tempPlug.getValue(o_data.data.attributes.isStatic);
                        tempPlug = t_transform.findPlug("drmPhysicsType");
                        tempPlug.getValue(o_data.data.attributes.physicsType);

                        tempPlug = t_transform.findPlug("drmTypeBlueprint");
                        tempPlug.getValue(o_data.data.attributes.typeBlueprint);
                        tempPlug = t_transform.findPlug("drmIsDangerous");
                        tempPlug.getValue(o_data.data.attributes.isDangerous);

                        if(ApplicationContext::GetInstance().GetDebugStatus())
                        {
                            MString t_str1 = " Translation( " + MString() + o_data.data.translation[0];
                            t_str1 += " , " + MString() + o_data.data.translation[1];
                            t_str1 += " , " + MString() + o_data.data.translation[2] + " )";
                            PrintDebug(MString(o_data.nodeName.c_str()) + t_str1);

                            MString t_str2 = " Rotation( " + MString() + o_data.data.rotation[0];
                            t_str2 += " , " + MString() + o_data.data.rotation[1];
                            t_str2 += " , " + MString() + o_data.data.rotation[2];
                            t_str2 += " , " + MString() + o_data.data.rotation[3] + " )";
                            PrintDebug(MString(o_data.nodeName.c_str()) + t_str2);

                            MString t_str3 = " Scale( " + MString() + o_data.data.scale[0];
                            t_str3 += " , " + MString() + o_data.data.scale[1];
                            t_str3 += " , " + MString() + o_data.data.scale[2] + " )";
                            PrintDebug(MString(o_data.nodeName.c_str()) + t_str3);
                        }
                        return o_data;
                    }
                    else
                    {
                        PrintError("Incorrect type (not a transform!)");
                    }
                }
                return o_data;
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Cath: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        MeshInfo MessageBuilder::GetMeshData(std::string p_nodeName)
        {
            try
            {
                MStatus result;
                MeshInfo o_data;

                MString t_nodeNameMString(p_nodeName.c_str());
                MSelectionList t_selectionList;
                MDagPath t_dagPath;
                PrintDebug("DEBUGAGAGA + " + MString(p_nodeName.c_str()) + " 11111");
                if(MGlobal::getSelectionListByName(t_nodeNameMString, t_selectionList))
                {
                    t_selectionList.getDagPath(0, t_dagPath);
                    if(t_dagPath.hasFn(MFn::kMesh))
                    {
                        MFnMesh t_mesh(t_dagPath.node(), &result);
                        //
                        // Get Mesh Data
                        //
                        const float* t_vertexList;
                        const float* t_normalsList;
                        MFloatArray t_uArray;
                        MFloatArray t_vArray;
                        MIntArray t_trianglePerPolyCount;
                        MIntArray t_triangleVertexIDs;
                        MIntArray t_triangleNormalIDs;
                        MIntArray t_triangleUVIDs;

                        t_vertexList = t_mesh.getRawPoints(&result);
                        t_normalsList = t_mesh.getRawNormals(&result);
                        t_mesh.getUVs(t_uArray, t_vArray);
                        t_mesh.getTriangles(t_trianglePerPolyCount, t_triangleVertexIDs);
                        PrintDebug("DEBUGAGAGA + " + MString(p_nodeName.c_str()) + " 22222");
                        MItMeshPolygon t_polyIterator(t_dagPath.node(), &result);
                        if(t_polyIterator.hasValidTriangulation())
                        {
                            for(t_polyIterator.reset(); !t_polyIterator.isDone(); t_polyIterator.next())
                            {
                                int t_trianglesThisPoly = 0;
                                t_polyIterator.numTriangles(t_trianglesThisPoly);
                                int uId0, uId1, uId2;
                                if(t_trianglesThisPoly == 1)
                                {
                                    t_triangleNormalIDs.append(t_polyIterator.normalIndex(0, &result));
                                    t_triangleNormalIDs.append(t_polyIterator.normalIndex(1, &result));
                                    t_triangleNormalIDs.append(t_polyIterator.normalIndex(2, &result));

                                    t_polyIterator.getUVIndex(0, uId0);
                                    t_polyIterator.getUVIndex(1, uId1);
                                    t_polyIterator.getUVIndex(2, uId2);
                                    t_triangleUVIDs.append(uId0);
                                    t_triangleUVIDs.append(uId1);
                                    t_triangleUVIDs.append(uId2);
                                }
                                else if(t_trianglesThisPoly == 2)
                                {
                                    t_triangleNormalIDs.append(t_polyIterator.normalIndex(0, &result));
                                    t_triangleNormalIDs.append(t_polyIterator.normalIndex(1, &result));
                                    t_triangleNormalIDs.append(t_polyIterator.normalIndex(3, &result));
                                    t_triangleNormalIDs.append(t_polyIterator.normalIndex(3, &result));
                                    t_triangleNormalIDs.append(t_polyIterator.normalIndex(1, &result));
                                    t_triangleNormalIDs.append(t_polyIterator.normalIndex(2, &result));

                                    t_polyIterator.getUVIndex(0, uId0);
                                    t_polyIterator.getUVIndex(1, uId1);
                                    t_polyIterator.getUVIndex(3, uId2);
                                    t_triangleUVIDs.append(uId0);
                                    t_triangleUVIDs.append(uId1);
                                    t_triangleUVIDs.append(uId2);
                                    t_polyIterator.getUVIndex(3, uId0);
                                    t_polyIterator.getUVIndex(1, uId1);
                                    t_polyIterator.getUVIndex(2, uId2);
                                    t_triangleUVIDs.append(uId0);
                                    t_triangleUVIDs.append(uId1);
                                    t_triangleUVIDs.append(uId2);
                                }
                                else if(t_trianglesThisPoly > 2)
                                {
                                    bool t_firstIteration = true;
                                    int t_index1, t_index2;
                                    MIntArray t_vertsThisPoly;
                                    t_polyIterator.getVertices(t_vertsThisPoly);
                                    int t_midValue = t_vertsThisPoly.length() - 1;
                                    for(int i = 0; i < t_trianglesThisPoly; ++i)
                                    {
                                        if(t_firstIteration)
                                        {
                                            t_triangleNormalIDs.append(t_polyIterator.normalIndex(0, &result));
                                            t_triangleNormalIDs.append(t_polyIterator.normalIndex(1, &result));
                                            t_triangleNormalIDs.append(t_polyIterator.normalIndex(t_midValue - 2, &result));

                                            t_polyIterator.getUVIndex(0, uId0);
                                            t_polyIterator.getUVIndex(1, uId1);
                                            t_polyIterator.getUVIndex(t_midValue, uId2);
                                            t_triangleUVIDs.append(uId0);
                                            t_triangleUVIDs.append(uId1);
                                            t_triangleUVIDs.append(uId2);

                                            t_index1 = 1;
                                            t_index2 = t_midValue;
                                            t_firstIteration = false;
                                        }
                                        else
                                        {
                                            if(t_index2 - 1 >= 0)
                                            {
                                                if(t_index2 - 1 != t_midValue)
                                                {
                                                    t_index2--;
                                                }
                                                else if(t_index2 - 2 >= 0)
                                                {
                                                    t_index2 -= 2;
                                                }
                                                else
                                                {
                                                    t_index2 = t_midValue + 1;
                                                }
                                            }
                                            else
                                            {
                                                t_index2 = t_vertsThisPoly.length() - 1;
                                            }
                                            t_triangleNormalIDs.append(t_polyIterator.normalIndex(t_index1, &result));
                                            t_triangleNormalIDs.append(t_polyIterator.normalIndex(t_index2, &result));
                                            t_triangleNormalIDs.append(t_polyIterator.normalIndex(t_midValue - 2, &result));

                                            t_polyIterator.getUVIndex(t_index1, uId0);
                                            t_polyIterator.getUVIndex(t_index1, uId1);
                                            t_polyIterator.getUVIndex(t_midValue, uId2);
                                            t_triangleUVIDs.append(uId0);
                                            t_triangleUVIDs.append(uId1);
                                            t_triangleUVIDs.append(uId2);

                                            t_index1 = t_index2;
                                            t_index2 -= 2;
                                        }
                                    }
                                }
                                else
                                {
                                    PrintWarning("No triangles in face!");
                                }
                            }
                        }
                        //
                        // Get Materials
                        //
                        PrintDebug("DEBUGAGAGA + " + MString(p_nodeName.c_str()) + " 33333");
                        std::string t_materialName;
                        MObjectArray t_connectedShaders;
                        MIntArray t_shaderIDArray;
                        t_mesh.getConnectedShaders(0, t_connectedShaders, t_shaderIDArray);
                        size_t t_connectionID;
                        for(size_t i = 0; i < t_connectedShaders.length(); ++i)
                        {
                            MFnDependencyNode t_shaderGroupTemp(t_connectedShaders[i], &result);
                            MPlug t_plug_2 = t_shaderGroupTemp.findPlug("surfaceShader", &result);
                            if(result)
                            {
                                t_connectionID = i;
                                break;
                            }
                        }
                        MFnDependencyNode t_shaderGroup(t_connectedShaders[t_connectionID], &result);
                        MPlug t_plug = t_shaderGroup.findPlug("surfaceShader");
                        MPlugArray t_connections;
                        t_plug.connectedTo(t_connections, true, false);
                        int t_shaderID = -1;
                        for(size_t i = 0; i < t_connections.length(); ++i)
                        {
                            if(t_connections[i].node().hasFn(MFn::kLambert))
                            {
                                t_shaderID = i;
                            }
                        }
                        if(t_shaderID >= 0)
                        {
                            MFnDependencyNode t_material(t_connections[t_shaderID].node());
                            t_materialName = t_material.name().asChar();
                        }
                        PrintDebug("DEBUGAGAGA + " + MString(p_nodeName.c_str()) + " 44444");
                        //
                        // Get Parent Names
                        //
                        std::vector<std::string> t_parentNames;
                        size_t t_parentCount = t_mesh.parentCount();
                        for(std::vector<std::string>::size_type i = 0; i < t_parentCount; ++i)
                        {
                            if(t_mesh.parent(i).hasFn(MFn::kTransform))
                            {
                                MFnTransform t_parentTransform(t_mesh.parent(i), &result);
                                t_parentNames.push_back(t_parentTransform.name().asChar());
                            }
                        }
                        //
                        // Fill output struct
                        //
                        PrintDebug("DEBUGAGAGA + " + MString(p_nodeName.c_str()) + " 55555");
                        o_data.transformCount = t_parentCount;
                        o_data.transformName = t_parentNames;
                        o_data.materialID = 0;
                        o_data.materialName = t_materialName;
                        o_data.meshID = 0;
                        o_data.nodeName = p_nodeName;
                        o_data.data.vertCount = t_mesh.numVertices();
                        o_data.data.normalCount = t_mesh.numNormals();
                        o_data.data.UVCount = t_mesh.numUVs();
                        o_data.data.vertices = t_vertexList;
                        o_data.data.normals = t_normalsList;
                        o_data.data.indCount = t_triangleVertexIDs.length();
                        o_data.data.triCount = t_trianglePerPolyCount.length();

                        o_data.data.uv = new float2[t_mesh.numUVs()];
                        for(int i = 0; i < o_data.data.UVCount; i++)
                        {
                            o_data.data.uv[i][0] = t_uArray[i];
                            o_data.data.uv[i][1] = t_vArray[i];
                        }

                        o_data.data.triPerFace = new int[t_trianglePerPolyCount.length()];
                        t_trianglePerPolyCount.get(o_data.data.triPerFace);

                        o_data.data.triIndices = new int[o_data.data.indCount];
                        t_triangleVertexIDs.get(o_data.data.triIndices);

                        o_data.data.norIndices = new int[t_triangleNormalIDs.length()];
                        t_triangleNormalIDs.get(o_data.data.norIndices);

                        o_data.data.UVIndices = new int[t_triangleUVIDs.length()];
                        t_triangleUVIDs.get(o_data.data.UVIndices);
                        PrintDebug("DEBUGAGAGA + " + MString(p_nodeName.c_str()) + " 66666");
                        if(ApplicationContext::GetInstance().GetDebugStatus())
                        {
                            MString namestr = MString(p_nodeName.c_str()) + " ";
                            MString str = "Parents ( ";
                            for(int i = 0; i < o_data.transformCount; ++i)
                            {
                                str += MString(o_data.transformName[i].c_str()) + " ";
                            }
                            str += ")";
                            PrintDebug(namestr + str);
                            PrintDebug(namestr + " Material ( " + MString(o_data.materialName.c_str()) + " )");
                            MString str2 = "Vert# " + MString() + o_data.data.vertCount;
                            str2 += " Normals# " + MString() + o_data.data.normalCount;
                            str2 += " UVs# " + MString() + o_data.data.UVCount;
                            PrintDebug(namestr + str2);
                        }
                    }
                }

                return o_data;
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Cath: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        CameraInfo MessageBuilder::GetCameraData(std::string p_nodeName)
        {
            try
            {
                MStatus result;
                CameraInfo o_data;
                MString t_nodeNameMString(p_nodeName.c_str());
                MSelectionList t_selectionList;
                MDagPath t_dagPath;
                if(MGlobal::getSelectionListByName(t_nodeNameMString, t_selectionList))
                {
                    t_selectionList.getDagPath(0, t_dagPath);
                    if(t_dagPath.hasFn(MFn::kCamera))
                    {
                        MFnCamera t_camera(t_dagPath.node(), &result);
                        std::string t_parentName;
                        if(t_camera.parent(0).hasFn(MFn::kTransform))
                        {
                            MFnTransform t_parentTransform(t_camera.parent(0), &result);
                            if(!result)
                            {
                                PrintWarning("Could not find parent node" + GetNameStrings(p_nodeName, ""));
                            }
                            else
                            {
                                t_parentName = t_parentTransform.name().asChar();
                            }
                        }

                        MFloatMatrix t_projectionMatrix(t_camera.projectionMatrix());
                        MPoint t_position = t_camera.eyePoint(MSpace::Space::kPostTransform);
                        MFloatVector t_direction = t_camera.viewDirection(MSpace::Space::kPostTransform);
                        MFloatVector t_up = t_camera.upDirection(MSpace::Space::kPostTransform);
                        MFloatVector t_right = t_camera.rightDirection(MSpace::Space::kPostTransform);
                        bool t_isOrtho = t_camera.isOrtho();
                        double t_fov = t_camera.verticalFieldOfView();
                        //
                        // Fill data struct
                        //
                        o_data.nodeName = p_nodeName;
                        o_data.transformName = t_parentName;
                        o_data.data.isOrtho = t_isOrtho;
                        if(t_isOrtho)
                        {
                            o_data.data.hAngle = t_camera.orthoWidth();
                        }
                        else
                        {
                            o_data.data.hAngle = t_fov;
                        }
                        for(int i = 0; i < 3; ++i)
                        {
                            o_data.data.rightVector[i] = t_right[i];
                            o_data.data.target[i] = t_direction[i];
                            o_data.data.upVector[i] = t_up[i];
                        }
                        if(ApplicationContext::GetInstance().GetDebugStatus())
                        {
                            PrintDebug(MString(p_nodeName.c_str()) + " Pos(" + MString() + t_position.x + " , " + MString() + t_position.y + " , " +
                                       MString() + t_position.z + ")");
                            PrintDebug(MString(p_nodeName.c_str()) + " Dir(" + MString() + t_direction.x + " , " + MString() + t_direction.y + " , " +
                                       MString() + t_direction.z + ")");
                            PrintDebug(MString(p_nodeName.c_str()) + " Up(" + MString() + t_up.x + " , " + MString() + t_up.y + " , " + MString() + t_up.z + ")");
                            PrintDebug(MString(p_nodeName.c_str()) + " Orthographic: " + MString() + t_isOrtho);
                            PrintDebug(MString(p_nodeName.c_str()) + " Horizontal fov / Orthographic width: " + MString() + o_data.data.hAngle);
                        }
                    }
                }
                return o_data;
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Cath: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        LightInfo MessageBuilder::GetLightData(std::string p_nodeName)
        {
            try
            {
                MStatus result;
                LightInfo o_data;
                MString t_nodeNameMString(p_nodeName.c_str());
                MSelectionList t_selectionList;
                MDagPath t_dagPath;
                if(MGlobal::getSelectionListByName(t_nodeNameMString, t_selectionList))
                {
                    t_selectionList.getDagPath(0, t_dagPath);
                    if(t_dagPath.hasFn(MFn::kLight))
                    {
                        MFnLight t_baseLight(t_dagPath.node());
                        std::string t_parentName;
                        if(t_baseLight.parent(0).hasFn(MFn::kTransform))
                        {
                            MFnTransform t_parentTransform(t_baseLight.parent(0), &result);
                            if(!result)
                            {
                                PrintWarning("Could not find parent node" + GetNameStrings(p_nodeName, ""));
                            }
                            else
                            {
                                t_parentName = t_parentTransform.name().asChar();
                            }
                        }

                        MColor t_diffuseColor = t_baseLight.color();
                        float t_RGBColor[3];
                        t_diffuseColor.get(MColor::MColorType::kRGB, t_RGBColor[0], t_RGBColor[1], t_RGBColor[2]);
                        float t_intensity = t_baseLight.intensity();
                        //
                        // Fill generic data
                        //
                        o_data.nodeName = p_nodeName;
                        o_data.transformName = t_parentName;
                        o_data.data.intensity = t_intensity;
                        std::copy(t_RGBColor, t_RGBColor + 3, o_data.data.colorDiffuse);

                        //
                        // Fill Specific data
                        //
                        if(t_dagPath.hasFn(MFn::kDirectionalLight))
                        {
                            o_data.data.type = 1;
                            MFnDirectionalLight t_directionalLight(t_dagPath, &result);
                            MFloatVector t_direction(t_baseLight.lightDirection(0, MSpace::kWorld, &result));
                            o_data.data.direction[0] = t_direction.x;
                            o_data.data.direction[1] = t_direction.y;
                            o_data.data.direction[2] = t_direction.z;
                        }
                        else if(t_dagPath.hasFn(MFn::kSpotLight))
                        {
                            o_data.data.type = 2;
                            MFnSpotLight t_spotLight(t_dagPath, &result);
                            o_data.data.decayType = t_spotLight.decayRate();
                            o_data.data.coneAngle = t_spotLight.coneAngle();
                            o_data.data.penumAgle = t_spotLight.penumbraAngle();
                            o_data.data.dropOff = t_spotLight.dropOff();
                        }
                        else if(t_dagPath.hasFn(MFn::kPointLight))
                        {
                            o_data.data.type = 3;
                            MFnPointLight t_pointLight(t_dagPath, &result);
                            o_data.data.decayType = t_pointLight.decayRate();
                        }
                    }
                }
                return o_data;
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Cath: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        MaterialInfo MessageBuilder::GetMaterialData(std::string p_nodeName)
        {
            try
            {
                MStatus result;
                MaterialInfo o_data;
                MString t_nodeNameMString(p_nodeName.c_str());
                MSelectionList t_selectionList;
                MDagPath t_dagPath;
                if(MGlobal::getSelectionListByName(t_nodeNameMString, t_selectionList))
                {
                    MObject t_depObj;
                    t_selectionList.getDependNode(0, t_depObj);
                    MFnDependencyNode t_material(t_depObj, &result);

                    float t_diffuse;
                    MPlug t_plug = t_material.findPlug("diffuse", &result);
                    if(result)
                    {
                        t_plug.getValue(t_diffuse);
                    }

                    t_plug = t_material.findPlug("color", &result);
                    if(result)
                    {
                        if(t_plug.isConnected())
                        {
                            MPlugArray t_plugArray;
                            t_plug.connectedTo(t_plugArray, true, false);
                            for(int i = 0; i < t_plugArray.length(); ++i)
                            {
                                MFnDependencyNode t_texture(t_plugArray[i].node());
                                t_plug = t_texture.findPlug("fileTextureName", &result);
                                if(result)
                                {
                                    MString t_texPath = t_plug.asString();
                                    if(t_texPath.length() < 1)
                                    {
                                        PrintWarning(t_material.name() + " Texture path not set!");
                                        o_data.diffuseTexturePath[0] = 0;
                                    }
                                    else if(t_texPath.length() < 100)
                                    {
                                        o_data.diffuseTexturePath = t_texPath.asChar();
                                        PrintInfo(t_material.name() + " texture path: " + t_texPath);
                                        o_data.data.mapMasks |= (int)bitmask::COLORMAP;
                                    }
                                }
                            }
                        }
                        else
                        {
                            o_data.diffuseTexturePath[0] = 0;
                            t_plug = t_material.findPlug("colorR", &result);
                            t_plug.getValue(o_data.data.color[0]);
                            t_plug = t_material.findPlug("colorG", &result);
                            t_plug.getValue(o_data.data.color[1]);
                            t_plug = t_material.findPlug("colorB", &result);
                            t_plug.getValue(o_data.data.color[2]);
                        }
                    }
                    t_plug = t_material.findPlug("incandescence", &result);
                    if(result)
                    {
                        if(t_plug.isConnected())
                        {
                            MPlugArray t_plgArray;
                            t_plug.connectedTo(t_plgArray, true, false);
                            for(int i = 0; i < t_plgArray.length(); ++i)
                            {
                                if(t_plgArray[i].node().apiType() == MFn::kFileTexture)
                                {
                                    MFnDependencyNode glowTexture(t_plgArray[i].node());
                                    t_plug = glowTexture.findPlug("fileTextureName", &result);
                                    MString name = t_plug.asString();
                                    if(name.length() < 1)
                                    {
                                        PrintWarning(t_material.name() + " Glow path not set!");
                                        o_data.glowTexturePath[0] = 0;
                                    }
                                    else if(name.length() < 100)
                                    {
                                        PrintDebug(t_material.name() + " FOUND GLOW MAP " + name);
                                        o_data.glowTexturePath = name.asChar();

                                        o_data.data.mapMasks |= (int)bitmask::GLOWMAP;
                                    }
                                    else
                                    {
                                        PrintWarning(t_material.name() + "Glow texture path name too long");
                                    }
                                }
                            }
                        }
                    }
                    t_plug = t_material.findPlug("specularColor", &result);
                    if(result)
                    {
                        if(t_plug.isConnected())
                        {
                            PrintDebug("IS CONNECTED");
                        }
                        else
                        {
                            t_plug = t_material.findPlug("specularColorR", &result);
                            if(result) t_plug.getValue(o_data.data.specColor[0]);
                            t_plug = t_material.findPlug("specularColorG", &result);
                            if(result) t_plug.getValue(o_data.data.specColor[1]);
                            t_plug = t_material.findPlug("specularColorB", &result);
                            if(result) t_plug.getValue(o_data.data.specColor[2]);
                        }
                    }
                    t_plug = t_material.findPlug("cosinePower", &result);
                    if(result)
                    {
                        t_plug.getValue(o_data.data.specCosine);
                    }
                    t_plug = t_material.findPlug("eccentricity", &result);
                    if(result)
                    {
                        t_plug.getValue(o_data.data.specEccentricity);
                    }
                    t_plug = t_material.findPlug("specularRollOff", &result);
                    if(result)
                    {
                        t_plug.getValue(o_data.data.specRollOff);
                    }

                    PrintDebug(t_material.name() + " Color: " + MString() + o_data.data.color[0]);
                    PrintDebug(t_material.name() + " Specular color: ");
                    PrintDebug(t_material.name() + " Cosine Power: " + MString() + o_data.data.specCosine);
                    PrintDebug(t_material.name() + " Eccentricity: " + MString() + o_data.data.specEccentricity);
                    PrintDebug(t_material.name() + " Specular Roll Off: " + MString() + o_data.data.specRollOff);
                }
                return o_data;
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Cath: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }

        size_t MessageBuilder::GetMeshDataSize(std::string p_nodeName)
        {
            try
            {
                MStatus result;
                MeshInfo o_mesh;

                MString t_nodeNameMString(p_nodeName.c_str());
                MSelectionList t_selectionList;
                MDagPath t_dagPath;

                if(MGlobal::getSelectionListByName(t_nodeNameMString, t_selectionList))
                {
                    t_selectionList.getDagPath(0, t_dagPath);
                    if(t_dagPath.hasFn(MFn::kMesh))
                    {
                        MFnMesh t_mesh(t_dagPath, &result);

                        MIntArray t_trianglePerPolyCount;
                        MIntArray t_triangleVertexIDs;
                        t_mesh.getTriangles(t_trianglePerPolyCount, t_triangleVertexIDs);

                        o_mesh.transformCount = t_mesh.parentCount();
                        o_mesh.data.vertCount = t_mesh.numVertices();
                        o_mesh.data.normalCount = t_mesh.numNormals();
                        o_mesh.data.UVCount = t_mesh.numUVs();
                        o_mesh.data.indCount = t_triangleVertexIDs.length();
                        o_mesh.data.triCount = t_trianglePerPolyCount.length();

                        size_t t_messageSize = 0;
                        t_messageSize += sizeof(char) * 100;
                        t_messageSize += (sizeof(char) * 100) * static_cast<int>(o_mesh.transformCount);
                        t_messageSize += sizeof(char) * 100;
                        t_messageSize += sizeof(int) * 8;
                        t_messageSize += (sizeof(float) * 3) * o_mesh.data.vertCount;
                        t_messageSize += (sizeof(float) * 3) * o_mesh.data.normalCount;
                        t_messageSize += (sizeof(float) * 2) * o_mesh.data.UVCount;
                        t_messageSize += (sizeof(int) * 3) * o_mesh.data.indCount;
                        t_messageSize += sizeof(int) * o_mesh.data.triCount;
                        return t_messageSize;
                    }
                }
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        size_t MessageBuilder::GetTransformDataSize()
        {
            try
            {
                return sizeof(TransformMessage);
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Cath: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        size_t MessageBuilder::GetCameraDataSize()
        {
            try
            {
                return sizeof(CameraMessage);
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Cath: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        size_t MessageBuilder::GetLightDataSize()
        {
            try
            {
                return sizeof(LightMessage);
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Cath: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        size_t MessageBuilder::GetMaterialDataSize()
        {
            try
            {
                return sizeof(MaterialMessage);
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Cath: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
    }
}