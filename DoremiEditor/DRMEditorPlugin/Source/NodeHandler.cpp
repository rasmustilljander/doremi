#include <DoremiEditor\DRMEditorPlugin\Include\NodeHandler.hpp>
#include <DoremiEditor\DRMEditorPlugin\Include\ApplicationContext.hpp>
#include <DoremiEditor\DRMEditorPlugin\Include\MessageHandler.hpp>
#include <DoremiEditor\DRMEditorPlugin\Include\MessageBuilder.hpp>

namespace DoremiEditor
{
    namespace Plugin
    {
        NodeHandler::NodeHandler()
        {
            m_attributeVector.push_back(AttributeDescription("drmRender", "bool", "1"));
            m_attributeVector.push_back(AttributeDescription("drmBBox", "bool", "0"));
            m_attributeVector.push_back(AttributeDescription("drmCollidable", "bool", "1"));
            m_attributeVector.push_back(AttributeDescription("drmInteractable", "bool", "0"));
            m_attributeVector.push_back(AttributeDescription("drmCheckPointID", "long", "-1"));
            m_attributeVector.push_back(AttributeDescription("drmSpawnPointID", "long", "-1"));
            m_attributeVector.push_back(AttributeDescription("drmLevelStartEnd", "long", "0"));
            m_attributeVector.push_back(AttributeDescription("drmAIground", "bool", "0"));
            m_attributeVector.push_back(AttributeDescription("drmFrequencyAffected", "bool", "0"));
            m_attributeVector.push_back(AttributeDescription("drmInteractableRange", "double", "40"));
            m_attributeVector.push_back(AttributeDescription("drmInteractableStartPos", "double3", "0 0 0"));
            m_attributeVector.push_back(AttributeDescription("drmInteractableEndPos", "double3", "0 0 0"));
            m_attributeVector.push_back(AttributeDescription("drmInteractableOffset", "double2", "0 0"));
            m_attributeVector.push_back(AttributeDescription("drmInteractableStartSpeed", "double", "1"));
            m_attributeVector.push_back(AttributeDescription("drmInteractableEndSpeed", "double", "1"));
            m_attributeVector.push_back(AttributeDescription("drmIsPotentialFieldCollidable", "bool", "1"));
            m_attributeVector.push_back(AttributeDescription("drmPotentialFieldID", "long", "-1"));
            m_attributeVector.push_back(AttributeDescription("drmPotentialFieldNeighbour1", "long", "-1"));
            m_attributeVector.push_back(AttributeDescription("drmPotentialFieldNeighbour2", "long", "-1"));
            m_attributeVector.push_back(AttributeDescription("drmPotentialFieldNeighbour3", "long", "-1"));
            m_attributeVector.push_back(AttributeDescription("drmPotentialFieldNeighbour4", "long", "-1"));
            m_attributeVector.push_back(AttributeDescription("drmPotentialFieldNeighbour5", "long", "-1"));
            m_attributeVector.push_back(AttributeDescription("drmIsSpawner", "bool", "0"));
            m_attributeVector.push_back(AttributeDescription("drmSpawnsMax", "long", "1"));
            m_attributeVector.push_back(AttributeDescription("drmSpawnsMaxAlive", "long", "1"));
            m_attributeVector.push_back(AttributeDescription("drmSpawnTypeBlueprint", "long", "-1"));
            m_attributeVector.push_back(AttributeDescription("drmSpawnFrequency", "double", "0"));
            m_attributeVector.push_back(AttributeDescription("drmIsStatic", "bool", "1"));
            m_attributeVector.push_back(AttributeDescription("drmPhysicsType", "long", "-1"));
            m_attributeVector.push_back(AttributeDescription("drmTypeBlueprint", "long", "0"));
            m_attributeVector.push_back(AttributeDescription("drmIsDangerous", "bool", "0"));
            m_testBool = false;
        }
        NodeHandler::~NodeHandler() {}
        void NodeHandler::Initialize()
        {
            m_messageHandler = ApplicationContext::GetInstance().GetMessageHandler();
            m_messageBuilder = ApplicationContext::GetInstance().GetMessageBuilder();
        }
        void NodeHandler::AddCustomAttributesTransform(MFnTransform& p_transform)
        {
            try
            {
                for(std::vector<AttributeDescription>::size_type i = 0; i != m_attributeVector.size(); i++)
                {
                    MStatus res;
                    MPlug testPlug = p_transform.findPlug(MString(m_attributeVector[i].name.c_str()), &res);
                    if(res)
                    {
                        // FileMapping::printInfo("BBox exists!");
                        if(!testPlug.isKeyable())
                        {
                            MString myCommand2 = "setAttr -e -keyable true " + p_transform.name() + "." + MString(m_attributeVector[i].name.c_str());
                            MGlobal::executeCommandOnIdle(myCommand2);
                        }
                    }
                    else
                    {
                        if(m_attributeVector[i].type.find("3") != std::string::npos)
                        {
                            MString myCommand = "addAttr -ln \"" + MString(m_attributeVector[i].name.c_str()) + "\" -at " +
                                                MString(m_attributeVector[i].type.c_str()) + " " + p_transform.name();
                            MGlobal::executeCommandOnIdle(myCommand);
                            myCommand = "addAttr -ln \"" + MString(m_attributeVector[i].name.c_str()) + "X\" -at double -p " +
                                        MString(m_attributeVector[i].name.c_str()) + " " + p_transform.name();
                            MGlobal::executeCommandOnIdle(myCommand);
                            myCommand = "addAttr -ln \"" + MString(m_attributeVector[i].name.c_str()) + "Y\" -at double -p " +
                                        MString(m_attributeVector[i].name.c_str()) + " " + p_transform.name();
                            MGlobal::executeCommandOnIdle(myCommand);
                            myCommand = "addAttr -ln \"" + MString(m_attributeVector[i].name.c_str()) + "Z\" -at double -p " +
                                        MString(m_attributeVector[i].name.c_str()) + " " + p_transform.name();
                            MGlobal::executeCommandOnIdle(myCommand);
                            myCommand = "setAttr -type double3 " + p_transform.name() + "." + m_attributeVector[i].name.c_str() + " " +
                                        m_attributeVector[i].value.c_str();
                            MGlobal::executeCommandOnIdle(myCommand);
                        }
                        else if(m_attributeVector[i].type.find("2") != std::string::npos)
                        {
                            MString myCommand = "addAttr -ln \"" + MString(m_attributeVector[i].name.c_str()) + "\" -at " +
                                                MString(m_attributeVector[i].type.c_str()) + " " + p_transform.name();
                            MGlobal::executeCommandOnIdle(myCommand);
                            myCommand = "addAttr -ln \"" + MString(m_attributeVector[i].name.c_str()) + "X\" -at double -p " +
                                        MString(m_attributeVector[i].name.c_str()) + " " + p_transform.name();
                            MGlobal::executeCommandOnIdle(myCommand);
                            myCommand = "addAttr -ln \"" + MString(m_attributeVector[i].name.c_str()) + "Y\" -at double -p " +
                                        MString(m_attributeVector[i].name.c_str()) + " " + p_transform.name();
                            MGlobal::executeCommandOnIdle(myCommand);
                            myCommand = "setAttr -type double2 " + p_transform.name() + "." + m_attributeVector[i].name.c_str() + " " +
                                        m_attributeVector[i].value.c_str();
                            MGlobal::executeCommandOnIdle(myCommand);
                        }
                        else
                        {
                            MString myCommand = "addAttr -ln \"" + MString(m_attributeVector[i].name.c_str()) + "\" -at " +
                                                MString(m_attributeVector[i].type.c_str()) + " -dv " + MString(m_attributeVector[i].value.c_str()) +
                                                " " + p_transform.name();
                            MGlobal::executeCommandOnIdle(myCommand);
                            MString myCommand2 = "setAttr -e -keyable true " + p_transform.name() + "." + MString(m_attributeVector[i].name.c_str());
                            MGlobal::executeCommandOnIdle(myCommand2);
                        }
                    }
                }
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Cath: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        void NodeHandler::PrintVectorInfo()
        {
            int totalCount, transformCount, meshCount, camCount, lightCount, materialCount;
            transformCount = m_transformVector.size();
            meshCount = m_meshVector.size();
            camCount = m_cameraVector.size();
            lightCount = m_lightVector.size();
            materialCount = m_materialVector.size();
            totalCount = transformCount + meshCount + camCount + lightCount + materialCount;
            PrintInfo("NODE COUNTS");
            PrintInfo("Transforms: " + MString() + transformCount);
            PrintInfo("Meshes: " + MString() + meshCount);
            PrintInfo("Cameras: " + MString() + camCount);
            PrintInfo("Lights: " + MString() + lightCount);
            PrintInfo("Materials: " + MString() + materialCount);
            PrintInfo("TOTAL: " + MString() + totalCount);
        }
        void NodeHandler::AddTransformNode(const MFnTransform& p_transform)
        {
            try
            {
                // MFnTransform t_transform(p_transform.object());
                std::string transformName = p_transform.name().asChar();
                std::string parentName;
                // TODOJW
                // Determine if this is even necessary at this stage
                /*   if(p_transform.parent(0).hasFn(MFn::kTransform))
                   {
                       MFnTransform t_transformParent(p_transform.parent(0));
                       parentName = t_transformParent.name().asChar();
                   }
                   else
                   {
                       parentName = "";
                   }*/
                TransformInfo transformInfo{transformName, ""};
                // If no entries are added already, no need to loop.
                if(m_transformVector.size() == 0)
                {
                    m_transformVector.push_back(transformInfo);
                    // m_messageBuilder->GetTransformData(transformName);
                    m_messageHandler->AddMessage(transformName, NodeType::nTransform, MessageType::msgAdded, parentName);
                }
                // Otherwise loop through vector to prevent duplicates
                else
                {
                    bool nodeExists = false;
                    for(std::vector<TransformInfo>::size_type i = 0; i != m_transformVector.size(); ++i)
                    {
                        if(strcmp(transformName.c_str(), m_transformVector.at(i).nodeName.c_str()) == 0)
                        {
                            nodeExists = true;
                            break;
                        }
                        else
                        {
                            // Node doesn't exist in vectors and will be added in the next step.
                        }
                    }
                    if(!nodeExists)
                    {
                        // Add node to vector
                        m_transformVector.push_back(transformInfo);
                        m_messageHandler->AddMessage(transformName, NodeType::nTransform, MessageType::msgAdded, parentName);
                        PrintDebug("Added transform ( " + MString(transformName.c_str()) + " ) with parent ( " + MString(parentName.c_str()) + " )");
                    }
                    else
                    {
                        PrintDebug("Transform " + MString(transformName.c_str()) + " already exists!");
                    }
                }
                PrintVectorInfo();
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Cath: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        void NodeHandler::AddMeshNode(const MFnMesh& p_mesh)
        {
            try
            {
                std::string meshName = p_mesh.name().asChar();
                std::vector<std::string> parentNames;
                MeshInfo meshInfo;
                meshInfo.nodeName = meshName;
                if(!p_mesh.isIntermediateObject())
                {

                    if(p_mesh.parentCount() > 1)
                    {
                        for(int i = 0; i < p_mesh.parentCount(); ++i)
                        {
                            if(p_mesh.parent(i).hasFn(MFn::kTransform))
                            {
                                MFnTransform t_parent(p_mesh.parent(i));
                                parentNames.push_back(t_parent.name().asChar());
                                meshInfo.transformCount++;
                            }
                        }
                        meshInfo.transformName = parentNames;
                    }
                    else if(p_mesh.parent(0).hasFn(MFn::kTransform))
                    {
                        MFnTransform t_transform(p_mesh.parent(0));
                        parentNames.push_back(t_transform.name().asChar());
                        meshInfo.transformName = parentNames;
                        meshInfo.transformCount = 1;
                    }
                    else
                    {
                        // Do nothing if no transform is available. Can be changed to add an empty string instead.
                    }
                    if(m_meshVector.size() == 0)
                    {
                        m_meshVector.push_back(meshInfo);
                        m_messageHandler->AddMessage(meshInfo.nodeName, NodeType::nMesh, MessageType::msgAdded);
                        // m_messageBuilder->GetMeshData(meshName);
                        PrintDebug("Added mesh ( " + MString(meshName.c_str()) + " ) with parent ( " + MString(meshInfo.transformName.at(0).c_str()) + " )");
                    }
                    else
                    {
                        bool nodeExists = false;
                        for(std::vector<MeshInfo>::size_type i = 0; i < m_meshVector.size(); ++i)
                        {
                            if(strcmp(meshName.c_str(), m_meshVector.at(i).nodeName.c_str()) == 0)
                            {
                                nodeExists = true;
                                break;
                            }
                        }
                        if(!nodeExists)
                        {
                            m_meshVector.push_back(meshInfo);
                            m_messageHandler->AddMessage(meshInfo.nodeName, NodeType::nMesh, MessageType::msgAdded);
                            // m_messageBuilder->GetMeshData(meshName);
                            PrintDebug("Added mesh ( " + MString(meshName.c_str()) + " ) with parent ( " + MString(meshInfo.transformName.at(0).c_str()) + " )");
                        }
                        else
                        {
                            PrintDebug("Mesh " + MString(meshName.c_str()) + " already exists!");
                        }
                    }
                    PrintVectorInfo();
                }
                else
                {
                    PrintError(p_mesh.name() + " is INTERMEDIATE");
                }
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Cath: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        void NodeHandler::AddCameraNode(const MFnCamera& p_camera)
        {
            try
            {
                MFnCamera t_camera(p_camera.object());
                std::string cameraName = t_camera.name().asChar();
                std::string parentName;
                if(t_camera.parent(0).hasFn(MFn::kTransform))
                {
                    MFnTransform t_transformParent(t_camera.parent(0));
                    parentName = t_transformParent.name().asChar();
                }
                else
                {
                    parentName = "";
                }
                CameraInfo cameraInfo{cameraName, parentName};
                // If no entries are added already, no need to loop.
                if(m_cameraVector.size() == 0)
                {
                    m_cameraVector.push_back(cameraInfo);
                    // m_messageBuilder->GetTransformData(transformName);
                    m_messageHandler->AddMessage(cameraName, NodeType::nCamera, MessageType::msgAdded, parentName);
                }
                // Otherwise loop through vector to prevent duplicates
                else
                {
                    bool nodeExists = false;
                    for(std::vector<CameraInfo>::size_type i = 0; i != m_cameraVector.size(); ++i)
                    {
                        if(strcmp(cameraName.c_str(), m_cameraVector.at(i).nodeName.c_str()) == 0)
                        {
                            nodeExists = true;
                            break;
                        }
                        else
                        {
                            // Node doesn't exist in vectors and will be added in the next step.
                        }
                    }
                    if(!nodeExists)
                    {
                        // Add node to vector
                        m_cameraVector.push_back(cameraInfo);
                        m_messageHandler->AddMessage(cameraName, NodeType::nCamera, MessageType::msgAdded, parentName);
                        PrintDebug("Added camera ( " + MString(cameraName.c_str()) + " ) with parent ( " + MString(parentName.c_str()) + " )");
                    }
                    else
                    {
                        PrintDebug("Camera " + MString(cameraName.c_str()) + " already exists!");
                    }
                }
                PrintVectorInfo();
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Cath: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        void NodeHandler::AddMaterialNode(const MFnDependencyNode& p_material)
        {
            try
            {
                MaterialInfo t_materialInfo;
                t_materialInfo.nodeName = p_material.name().asChar();
                if(m_materialVector.size() == 0)
                {
                    m_materialVector.push_back(t_materialInfo);
                    m_messageHandler->AddMessage(t_materialInfo.nodeName, NodeType::nMaterial, MessageType::msgAdded);
                    PrintDebug("Added material: " + p_material.name());
                }
                else
                {
                    bool nodeExists = false;
                    for(std::vector<MaterialInfo>::size_type i = 0; i < m_materialVector.size(); ++i)
                    {
                        if(strcmp(t_materialInfo.nodeName.c_str(), m_materialVector.at(i).nodeName.c_str()) == 0)
                        {
                            nodeExists = true;
                            break;
                        }
                    }
                    if(nodeExists)
                    {
                        PrintWarning("Material node " + p_material.name() + " already exists!");
                    }
                    else
                    {
                        m_materialVector.push_back(t_materialInfo);
                        PrintDebug("Added material: " + p_material.name());
                        m_messageHandler->AddMessage(t_materialInfo.nodeName, NodeType::nMaterial, MessageType::msgAdded);
                    }
                }
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Cath: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        void NodeHandler::AddLightNode(const MFnLight& p_light)
        {
            try
            {
                std::string lightName = p_light.name().asChar();
                LightInfo lightInfo;
                lightInfo.nodeName = lightName;
                if(m_lightVector.size() == 0)
                {
                    /// m_messageBuilder->GetLightData(lightInfo.nodeName);
                    m_lightVector.push_back(lightInfo);
                    m_messageHandler->AddMessage(lightName, NodeType::nLight, MessageType::msgAdded);
                    PrintDebug("Added light ( " + MString(lightName.c_str()) + " )");
                }
                else
                {
                    bool nodeExists = false;
                    for(std::vector<LightInfo>::size_type i = 0; i < m_lightVector.size(); ++i)
                    {
                        if(strcmp(lightName.c_str(), m_lightVector.at(i).nodeName.c_str()) == 0)
                        {
                            nodeExists = true;
                            break;
                        }
                    }
                    if(!nodeExists)
                    {
                        m_lightVector.push_back(lightInfo);
                        // m_messageBuilder->GetLightData(lightInfo.nodeName);
                        m_messageHandler->AddMessage(lightName, NodeType::nLight, MessageType::msgAdded);
                        PrintDebug("Added light ( " + MString(lightName.c_str()) + " )");
                    }
                    else
                    {
                        PrintDebug("Light " + MString(lightInfo.nodeName.c_str()) + " already exists");
                    }
                }
                PrintVectorInfo();
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Cath: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }

        void NodeHandler::ChangeParentName(MFnTransform& p_transform, const std::string p_oldName)
        {
            try
            {
                MStatus result;
                std::string t_newParentName = p_transform.name().asChar();
                PrintError("KOMMMER VI HIT? " + MString() + p_transform.childCount());
                for(size_t t = 0; t < p_transform.childCount(); ++t)
                {
                    if(p_transform.child(t).hasFn(MFn::kTransform))
                    {
                        MFnTransform t_transform(p_transform.child(t), &result);

                        for(std::vector<TransformInfo>::size_type i; i < m_transformVector.size(); ++i)
                        {
                            if(strcmp(p_oldName.c_str(), m_transformVector.at(i).parentName.c_str()) == 0)
                            {
                                PrintDebug("Changed parent name for " + t_transform.name() + " ( " + GetNameStrings(p_oldName, t_newParentName) +
                                           " )");
                                m_transformVector.at(i).parentName = t_newParentName;
                            }
                        }
                    }
                    else if(p_transform.child(t).hasFn(MFn::kMesh))
                    {
                        MFnMesh t_mesh(p_transform.child(t), &result);
                        // PrintError("1111111111111111111111111");
                        for(std::vector<MeshInfo>::size_type i; i < m_meshVector.size(); ++i)
                        {
                            PrintError("1111111111111111111111111");
                            for(std::vector<std::string>::size_type o; o < m_meshVector.at(i).transformName.size(); ++o)
                            {
                                PrintError("222222222222222222222222");
                                /*if (strcmp(p_oldName.c_str(), m_meshVector.at(i).transformName.at(o).c_str()) == 0)
                                {
                                PrintDebug("Changed parent name for " + t_mesh.name() + " ( " + GetNameStrings(p_oldName, t_newParentName) + " )");
                                m_meshVector.at(i).transformName.at(o) = t_newParentName;
                                }*/
                            }
                            /*if (strcmp(p_oldName.c_str(), m_transformVector.at(i).parentName.c_str()) == 0)
                            {
                            PrintDebug("Changed parent name for " + t_mesh.name() + " ( " + GetNameStrings(p_oldName, t_newParentName) + " )");
                            m_transformVector.at(i).parentName = t_newParentName;
                            }*/
                        }
                    }
                    else if(p_transform.child(t).hasFn(MFn::kCamera))
                    {
                        MFnCamera t_camera(p_transform.child(t), &result);

                        for(std::vector<CameraInfo>::size_type i; i < m_cameraVector.size(); ++i)
                        {
                            if(strcmp(p_oldName.c_str(), m_cameraVector.at(i).transformName.c_str()) == 0)
                            {
                                PrintDebug("Changed parent name for " + t_camera.name() + " ( " + GetNameStrings(p_oldName, t_newParentName) + " )");
                                m_cameraVector.at(i).transformName = t_newParentName;
                            }
                        }
                    }
                    else if(p_transform.child(t).hasFn(MFn::kLight))
                    {
                        MFnLight t_light(p_transform.child(t), &result);

                        for(std::vector<LightInfo>::size_type i; i < m_lightVector.size(); ++i)
                        {
                            if(strcmp(p_oldName.c_str(), m_lightVector.at(i).transformName.c_str()) == 0)
                            {
                                PrintDebug("Changed parent name for " + t_light.name() + " ( " + GetNameStrings(p_oldName, t_newParentName) + " )");
                                m_lightVector.at(i).transformName = t_newParentName;
                            }
                        }
                    }
                }
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Cath: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        void NodeHandler::NodeNameChangeTransform(MFnTransform& p_transform, const std::string p_oldName)
        {
            try
            {
                MStatus result;
                std::string t_oldName = p_oldName;
                std::string t_newName = p_transform.name().asChar();
                for(std::vector<TransformInfo>::size_type i = 0; i < m_transformVector.size(); ++i)
                {
                    if(strcmp(p_oldName.c_str(), m_transformVector.at(i).nodeName.c_str()) == 0)
                    {
                        m_transformVector.at(i).nodeName = t_newName;
                        m_messageHandler->AddMessage(p_transform.name().asChar(), NodeType::nTransform, MessageType::msgRenamed, p_oldName);
                        // ChangeParentName(p_transform, t_oldName);
                        PrintDebug("Transform name changed in vector" + GetNameStrings(p_oldName, m_transformVector.at(i).nodeName));
                    }
                }
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Cath: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        void NodeHandler::NodeNameChangeMesh(MFnMesh& p_mesh, const std::string p_oldName)
        {
            try
            {
                MStatus result;
                PrintInfo("Trans");
                std::string t_newName = p_mesh.name().asChar();
                for(std::vector<MeshInfo>::size_type i = 0; i < m_meshVector.size(); ++i)
                {
                    if(strcmp(p_oldName.c_str(), m_meshVector.at(i).nodeName.c_str()) == 0)
                    {
                        m_meshVector.at(i).nodeName = t_newName;
                        m_messageHandler->AddMessage(p_mesh.name().asChar(), NodeType::nMesh, MessageType::msgRenamed, p_oldName);
                        PrintInfo("Mesh name changed in vector" + GetNameStrings(p_oldName, m_meshVector.at(i).nodeName));
                    }
                }
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Cath: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        void NodeHandler::NodeNameChangeCamera(MFnCamera& p_camera, const std::string p_oldName)
        {
            try
            {
                MStatus result;
                PrintInfo("Trans");
                std::string t_newName = p_camera.name().asChar();
                for(std::vector<CameraInfo>::size_type i = 0; i < m_cameraVector.size(); ++i)
                {
                    if(strcmp(p_oldName.c_str(), m_cameraVector.at(i).nodeName.c_str()) == 0)
                    {
                        m_cameraVector.at(i).nodeName = t_newName;
                        m_messageHandler->AddMessage(p_camera.name().asChar(), NodeType::nMesh, MessageType::msgRenamed, p_oldName);
                        PrintInfo("Camera name changed in vector" + GetNameStrings(p_oldName, m_cameraVector.at(i).nodeName));
                    }
                }
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Cath: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        void NodeHandler::NodeNameChangeLight(MFnLight& p_light, const std::string p_oldName)
        {
            try
            {
                MStatus result;
                PrintInfo("Trans");
                std::string t_newName = p_light.name().asChar();
                for(std::vector<LightInfo>::size_type i = 0; i < m_lightVector.size(); ++i)
                {
                    if(strcmp(p_oldName.c_str(), m_lightVector.at(i).nodeName.c_str()) == 0)
                    {
                        m_lightVector.at(i).nodeName = t_newName;
                        m_messageHandler->AddMessage(p_light.name().asChar(), NodeType::nMesh, MessageType::msgRenamed, p_oldName);
                        PrintInfo("Light name changed in vector" + GetNameStrings(p_oldName, m_lightVector.at(i).nodeName));
                    }
                }
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Cath: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        void NodeHandler::NodeNameChangeMaterial(MFnDependencyNode& p_material, const std::string p_oldName)
        {
            try
            {
                MStatus result;
                PrintInfo("Trans");
                std::string t_newName = p_material.name().asChar();
                for(std::vector<MaterialInfo>::size_type i = 0; i < m_materialVector.size(); ++i)
                {
                    if(strcmp(p_oldName.c_str(), m_materialVector.at(i).nodeName.c_str()) == 0)
                    {
                        m_materialVector.at(i).nodeName = t_newName;
                        m_messageHandler->AddMessage(p_material.name().asChar(), NodeType::nMesh, MessageType::msgRenamed, p_oldName);
                        PrintInfo("Material name changed in vector" + GetNameStrings(p_oldName, m_materialVector.at(i).nodeName));
                    }
                }
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Cath: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        void NodeHandler::NodeDeletedTransform(const std::string p_nodeName)
        {
            try
            {
                MStatus result;
                for(std::vector<TransformInfo>::size_type i = 0; i < m_transformVector.size(); ++i)
                {

                    if(strcmp(p_nodeName.c_str(), m_transformVector.at(i).nodeName.c_str()) == 0)
                    {
                        if(m_messageHandler->AddMessage(p_nodeName, NodeType::nTransform, MessageType::msgDeleted))
                        {
                            m_transformVector.erase(m_transformVector.begin() + i);
                        }
                    }
                }
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Cacth: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        void NodeHandler::NodeDeletedMesh(const std::string p_nodeName)
        {
            try
            {
                MStatus result;
                for(std::vector<MeshInfo>::size_type i = 0; i < m_meshVector.size(); ++i)
                {

                    if(strcmp(p_nodeName.c_str(), m_meshVector.at(i).nodeName.c_str()) == 0)
                    {
                        if(m_messageHandler->AddMessage(p_nodeName, NodeType::nMesh, MessageType::msgDeleted))
                        {
                            m_meshVector.erase(m_meshVector.begin() + i);
                        }
                    }
                }
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Cacth: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        void NodeHandler::NodeDeletedCamera(const std::string p_nodeName)
        {
            try
            {
                MStatus result;
                for(std::vector<CameraInfo>::size_type i = 0; i < m_cameraVector.size(); ++i)
                {

                    if(strcmp(p_nodeName.c_str(), m_cameraVector.at(i).nodeName.c_str()) == 0)
                    {
                        if(m_messageHandler->AddMessage(p_nodeName, NodeType::nCamera, MessageType::msgDeleted))
                        {
                            m_cameraVector.erase(m_cameraVector.begin() + i);
                        }
                    }
                }
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Cacth: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        void NodeHandler::NodeDeletedLight(const std::string p_nodeName)
        {
            try
            {
                MStatus result;
                for(std::vector<LightInfo>::size_type i = 0; i < m_lightVector.size(); ++i)
                {

                    if(strcmp(p_nodeName.c_str(), m_lightVector.at(i).nodeName.c_str()) == 0)
                    {
                        if(m_messageHandler->AddMessage(p_nodeName, NodeType::nLight, MessageType::msgDeleted))
                        {
                            m_lightVector.erase(m_lightVector.begin() + i);
                        }
                    }
                }
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Cacth: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        void NodeHandler::NodeDeletedMaterial(const std::string p_nodeName)
        {
            try
            {
                MStatus result;
                for(std::vector<MaterialInfo>::size_type i = 0; i < m_materialVector.size(); ++i)
                {

                    if(strcmp(p_nodeName.c_str(), m_materialVector.at(i).nodeName.c_str()) == 0)
                    {
                        if(m_messageHandler->AddMessage(p_nodeName, NodeType::nMaterial, MessageType::msgDeleted))
                        {
                            m_materialVector.erase(m_materialVector.begin() + i);
                        }
                    }
                }
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }

        bool NodeHandler::FindSavedTransform(std::string p_nodeName)
        {
            try
            {
                bool result = false;
                for(std::vector<TransformInfo>::size_type i = 0; i < m_transformVector.size(); ++i)
                {
                    if(strcmp(p_nodeName.c_str(), m_transformVector.at(i).nodeName.c_str()) == 0)
                    {
                        result = true;
                    }
                }
                return result;
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }

        bool NodeHandler::FindSavedMesh(std::string p_nodeName)
        {
            try
            {
                bool result = false;
                for(std::vector<MeshInfo>::size_type i = 0; i < m_meshVector.size(); ++i)
                {
                    if(strcmp(p_nodeName.c_str(), m_meshVector.at(i).nodeName.c_str()) == 0)
                    {
                        result = true;
                    }
                }
                return result;
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        bool NodeHandler::FindSavedCamera(std::string p_nodeName)
        {
            try
            {
                bool result = false;
                for(std::vector<CameraInfo>::size_type i = 0; i < m_cameraVector.size(); ++i)
                {
                    if(strcmp(p_nodeName.c_str(), m_cameraVector.at(i).nodeName.c_str()) == 0)
                    {
                        result = true;
                    }
                }
                return result;
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        bool NodeHandler::FindSavedLight(std::string p_nodeName)
        {
            try
            {
                bool result = false;
                for(std::vector<LightInfo>::size_type i = 0; i < m_lightVector.size(); ++i)
                {
                    if(strcmp(p_nodeName.c_str(), m_lightVector.at(i).nodeName.c_str()) == 0)
                    {
                        result = true;
                    }
                }
                return result;
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        bool NodeHandler::FindSavedMaterial(std::string p_nodeName)
        {
            try
            {
                bool result = false;
                for(std::vector<MaterialInfo>::size_type i = 0; i < m_materialVector.size(); ++i)
                {
                    if(strcmp(p_nodeName.c_str(), m_materialVector.at(i).nodeName.c_str()) == 0)
                    {
                        result = true;
                    }
                }
                return result;
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
    }
}