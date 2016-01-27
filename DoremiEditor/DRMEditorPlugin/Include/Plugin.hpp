#pragma once

// some definitions for the DLL to play nice with Maya
#define NT_PLUGIN
#define REQUIRE_IOSTREAM
#define EXPORT __declspec(dllexport)

#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <queue>

#include <maya/MEvent.h>
#include <maya/MFnPlugin.h>
#include <maya/MFnMesh.h>
#include <maya/MFnTransform.h>
#include <maya/MFloatPointArray.h>
#include <maya/MPointArray.h>
#include <maya/MIntArray.h>
#include <maya/MPoint.h>
#include <maya/MMatrix.h>
#include <maya/MFloatMatrix.h>
#include <maya/MEulerRotation.h>
#include <maya/MVector.h>
#include <maya/MItDag.h>
#include <maya/MFnCamera.h>
#include <maya/M3dView.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MPlugArray.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnLambertShader.h>
#include <maya/MFnBlinnShader.h>
#include <maya/MFnPhongShader.h>
#include <maya/MImage.h>
#include <maya/MFnPointLight.h>
#include <maya/MSelectionList.h>
#include <maya/MItDependencyNodes.h>
#include <maya/MFnDirectionalLight.h>
#include <maya/MFnSpotLight.h>
#include <maya/MFnPointLight.h>
#include <maya/M3dView.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MMatrix.h>
// Wrappers
#include <maya/MGlobal.h>
#include <maya/MCallbackIdArray.h>

// Messages
#include <maya/MMessage.h>
#include <maya/MTimerMessage.h>
#include <maya/MDGMessage.h>
#include <maya/MEventMessage.h>
#include <maya/MPolyMessage.h>
#include <maya/MNodeMessage.h>
#include <maya/MDagPath.h>
#include <maya/MDagMessage.h>
#include <maya/MUiMessage.h>
#include <maya/MModelMessage.h>
#include <maya/MCameraSetMessage.h>
#include <maya/MLockMessage.h>

#include <FileMap.hpp>

// Commands
#include <maya/MPxCommand.h>

std::vector<std::string> msgTypeVector;

FileMapping fileMap;

M3dView modelPanel;
MDagPath activeCamera;

bool debug;
MCallbackId _CBid;
MCallbackIdArray _CBidArray;
std::vector<MeshInfo> meshVector;
std::vector<TransformInfo> transVector;
std::vector<CameraInfo> camVector;
std::vector<LightInfo> lightVector;
std::vector<MaterialInfo> materialVector;
std::vector<MessageInfo> msgVector;
std::queue<MessageInfo> msgQueue;
void cbReparent(MDagPath& child, MDagPath& parent, void* clientData);
void cbMeshAttribute(MNodeMessage::AttributeMessage msg, MPlug& plug_1, MPlug& plug_2, void* clientData);
void cbMessageTimer(float elapsedTime, float lastTime, void* clientData);
void cbNewNode(MObject& node, void* clientData);
void cbTransformModified(MNodeMessage::AttributeMessage msg, MPlug& plug_1, MPlug& plug_2, void* clientData);
void cbNameChange(MObject& node, const MString& str, void* clientData);
void cbMeshAttributeChange(MNodeMessage::AttributeMessage msg, MPlug& plug_1, MPlug& plug_2, void* clientData);
void cbMaterialAttribute(MNodeMessage::AttributeMessage msg, MPlug& plug_1, MPlug& plug_2, void* clientData);

void mAddNode(char* name, int type, int vertCount = 0, char* childname = nullptr);
void mAddMessage(std::string name, int msgType, int nodeType, std::string oldName = "");
void loadScene();
bool deleteNode();
bool renameNode(MString newName, MString oldName, int type);

MeshInfo outMeshData(std::string name, bool getDynamicData = true);

void outTransFormData(MObject& obj);
void outMeshData(MObject& obj);
void outCameraData(MObject& obj);
void outLightData(MObject& obj);
