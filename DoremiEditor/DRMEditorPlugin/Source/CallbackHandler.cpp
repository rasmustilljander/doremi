#include <DoremiEditor\DRMEditorPlugin\Include\CallbackHandler.hpp>
#include <DoremiEditor\DRMEditorPlugin\Include\NodeHandler.hpp>
#include <DoremiEditor\DRMEditorPlugin\Include\MessageHandler.hpp>
#include <DoremiEditor\DRMEditorPlugin\Include\ApplicationContext.hpp>
#include <DoremiEditor\DRMEditorPlugin\Include\Filemapping.hpp>
#include <string>

namespace DoremiEditor
{
    namespace Plugin
    {
        MCallbackIdArray CallbackHandler::m_callbackIDArray;
        NodeHandler* CallbackHandler::s_nodeHandler = nullptr;
        MessageHandler* CallbackHandler::s_messageHandler = nullptr;
        M3dView CallbackHandler::m_modelPanel;
        float CallbackHandler::m_updateFrequency = 0.0f;
        CallbackHandler::CallbackHandler() { m_updateFrequency = 0.2f; }
        CallbackHandler::~CallbackHandler()
        {
            MMessage::removeCallbacks(m_callbackIDArray);
            m_callbackIDArray.clear();
        }

        void CallbackHandler::Initialize()
        {
            s_nodeHandler = ApplicationContext::GetInstance().GetNodeHandler();
            s_messageHandler = ApplicationContext::GetInstance().GetMessageHandler();
            m_callbackIDArray.append(MDGMessage::addNodeAddedCallback(CallbackHandler::cb_newNode));
            m_callbackIDArray.append(MTimerMessage::addTimerCallback(m_updateFrequency, CallbackHandler::cb_timer));
        }

        void CallbackHandler::LoadScene()
        {
            try
            {
                if(s_nodeHandler == nullptr)
                {
                    s_nodeHandler = ApplicationContext::GetInstance().GetNodeHandler();
                }
                MStatus result;

                result = M3dView::getM3dViewFromModelPanel("modelPanel4", m_modelPanel);
                m_callbackIDArray.append(MUiMessage::addCameraChangedCallback("modelPanel4", cb_cameraPanels));

                MItDependencyNodes itDep(MFn::kLambert);
                while(!itDep.isDone())
                {
                    if(itDep.thisNode().apiType() == MFn::kLambert)
                    {
                        AddMaterial(itDep.thisNode());
                    }
                    else if(itDep.thisNode().apiType() == MFn::kPhong)
                    {
                        AddMaterial(itDep.thisNode());
                    }
                    else if(itDep.thisNode().apiType() == MFn::kBlinn)
                    {
                        AddMaterial(itDep.thisNode());
                    }
                    else
                    {
                        // Forget about it!
                    }
                    itDep.next();
                }
                MFn::Type filter = MFn::kTransform;
                MItDag itTransforms(MItDag::kDepthFirst, filter, &result);
                if(!result)
                {
                    PrintError("Could not create transform iterator!");
                }
                else
                {
                    for(; !itTransforms.isDone(); itTransforms.next())
                    {
                        MDagPath t_transDagPath;
                        result = itTransforms.getPath(t_transDagPath);
                        MFnTransform t_trans(t_transDagPath, &result);
                        MFnDagNode t_transDagNode(t_transDagPath, &result);
                        int childCount = t_transDagNode.childCount();
                        AddTransform(t_transDagPath.node());
                        /*for(int i = 0; i < childCount; ++i)
                        {
                            MObject t_child(t_trans.child(i));
                            if(t_child.hasFn(MFn::kMesh))
                            {
                                AddMesh(t_child, false);
                            }
                            else if(t_child.hasFn(MFn::kLight))
                            {
                                AddLight(t_child, false);
                            }
                            else if(t_child.hasFn(MFn::kCamera))
                            {
                                AddCamera(t_child);
                            }
                        }*/
                    }
                }
                filter = MFn::kCamera;
                MItDag itCameras(MItDag::kDepthFirst, filter, &result);
                if(!result)
                {
                    PrintError("Could not create mesh iterator!");
                }
                else
                {
                    for(; !itCameras.isDone(); itCameras.next())
                    {
                        MDagPath t_cameraDagPath;
                        result = itCameras.getPath(t_cameraDagPath);
                        AddCamera(t_cameraDagPath.node());
                    }
                }
                MDagPath t_currentCameraPath;
                result = m_modelPanel.getCamera(t_currentCameraPath);
                if(result)
                {
                    MFnCamera t_camera(t_currentCameraPath);
                    s_messageHandler->AddMessage(t_camera.name().asChar(), NodeType::nCamera, MessageType::msgSwitched, "");
                }
                filter = MFn::kMesh;
                MItDag itMeshes(MItDag::kDepthFirst, filter, &result);
                if(!result)
                {
                    PrintError("Could not create mesh iterator!");
                }
                else
                {
                    for(; !itMeshes.isDone(); itMeshes.next())
                    {
                        MDagPath t_meshDagPath;
                        result = itMeshes.getPath(t_meshDagPath);
                        /*MFnMesh t_mesh(t_meshDagPath, &result);
                        MObject t_meshObject(t_mesh.object());
                        AddMesh(t_meshObject, true);*/
                        AddMesh(t_meshDagPath.node(), false);
                    }
                }

                filter = MFn::kLight;
                MItDag itLights(MItDag::kDepthFirst, filter, &result);
                if(!result)
                {
                    PrintError("Could not create mesh iterator!");
                }
                else
                {
                    for(; !itLights.isDone(); itLights.next())
                    {
                        MDagPath t_lightDagPath;
                        result = itLights.getPath(t_lightDagPath);
                        AddLight(t_lightDagPath.node(), false);
                    }
                }
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Cath: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        void CallbackHandler::UnloadScene(bool p_doTransforms, bool p_doMeshes, bool p_doCameras, bool p_doLights, bool p_doMaterials)
        {
            try
            {
                MStatus result;
                if(p_doLights)
                {
                    for(std::vector<LightInfo>::size_type i = 0; i < s_nodeHandler->m_lightVector.size(); ++i)
                    {
                        s_messageHandler->AddMessage(s_nodeHandler->m_lightVector.at(i).nodeName, NodeType::nLight, MessageType::msgDeleted);
                    }
                    s_nodeHandler->m_lightVector.clear();
                }
                if(p_doCameras)
                {
                    for(std::vector<CameraInfo>::size_type i = 0; i < s_nodeHandler->m_cameraVector.size(); ++i)
                    {
                        s_messageHandler->AddMessage(s_nodeHandler->m_cameraVector.at(i).nodeName, NodeType::nCamera, MessageType::msgDeleted);
                    }
                    s_nodeHandler->m_cameraVector.clear();
                }
                if(p_doMeshes)
                {
                    for(std::vector<MeshInfo>::size_type i = 0; i < s_nodeHandler->m_meshVector.size(); ++i)
                    {
                        s_messageHandler->AddMessage(s_nodeHandler->m_meshVector.at(i).nodeName, NodeType::nMesh, MessageType::msgDeleted);
                    }
                    s_nodeHandler->m_meshVector.clear();
                }
                if(p_doTransforms)
                {
                    for(std::vector<TransformInfo>::size_type i = 0; i < s_nodeHandler->m_transformVector.size(); ++i)
                    {
                        s_messageHandler->AddMessage(s_nodeHandler->m_transformVector.at(i).nodeName, NodeType::nTransform, MessageType::msgDeleted);
                    }
                    s_nodeHandler->m_transformVector.clear();
                }
                if(p_doMaterials)
                {
                    for(std::vector<MaterialInfo>::size_type i = 0; i < s_nodeHandler->m_materialVector.size(); ++i)
                    {
                        s_messageHandler->AddMessage(s_nodeHandler->m_materialVector.at(i).nodeName, NodeType::nMaterial, MessageType::msgDeleted);
                    }
                    s_nodeHandler->m_materialVector.clear();
                }
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Cath: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }

        void CallbackHandler::cb_newNode(MObject& p_node, void* p_clientData)
        {
            try
            {
                MStatus result;
                if(s_nodeHandler == nullptr)
                {
                    s_nodeHandler = ApplicationContext::GetInstance().GetNodeHandler();
                }
                if(p_node.hasFn(MFn::kTransform))
                {
                    MFnTransform t_transform(p_node, &result);
                    if(t_transform.childCount() > 0)
                    {
                        AddTransform(p_node);
                    }
                    else
                    {
                        m_callbackIDArray.append(MNodeMessage::addNameChangedCallback(p_node, cb_nameChange));
                        // If the transform has no children, there is at the moment no need to add this node
                    }
                }
                else if(p_node.hasFn(MFn::kMesh))
                {
                    AddMesh(p_node, true);
                }
                else if(p_node.hasFn(MFn::kCamera))
                {
                    AddCamera(p_node);
                }
                else if(p_node.hasFn(MFn::kLight))
                {
                    AddLight(p_node, true);
                }
                else if(p_node.hasFn(MFn::kLambert))
                {
                    AddMaterial(p_node);
                }
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Cath: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        void CallbackHandler::cb_timer(float elapsedTime, float lastTime, void* p_clientData)
        {
            try
            {
                if(ApplicationContext::GetInstance().GetFilemapping()->GetFilemapStatus())
                {
                    s_messageHandler->SendDelayedMessages();
                }
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Cath: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        void CallbackHandler::cb_preRemoveNode(MObject& p_node, void* p_clientData)
        {
            try
            {
                MStatus result;
                if(p_node.hasFn(MFn::kTransform))
                {
                    MFnTransform t_transform(p_node, &result);
                    s_nodeHandler->NodeDeletedTransform(t_transform.name().asChar());
                }
                else if(p_node.hasFn(MFn::kMesh))
                {
                    MFnMesh t_mesh(p_node, &result);
                    s_nodeHandler->NodeDeletedMesh(t_mesh.name().asChar());
                }
                else if(p_node.hasFn(MFn::kCamera))
                {
                    MFnCamera t_camera(p_node, &result);
                    s_nodeHandler->NodeDeletedCamera(t_camera.name().asChar());
                }
                else if(p_node.hasFn(MFn::kLight))
                {
                    MFnLight t_light(p_node, &result);
                    s_nodeHandler->NodeDeletedLight(t_light.name().asChar());
                }
                else if(p_node.hasFn(MFn::kLambert))
                {
                    MFnDependencyNode t_material(p_node, &result);
                    s_nodeHandler->NodeDeletedMaterial(t_material.name().asChar());
                }
                else
                {
                    PrintWarning("Unknown node removed");
                }
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Cath: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        void CallbackHandler::cb_nameChange(MObject& p_node, const MString& p_str, void* p_clientData)
        {
            try
            {
                MStatus result;

                std::string t_oldName = p_str.asChar();
                if(p_node.hasFn(MFn::kTransform))
                {
                    MFnTransform t_trans(p_node, &result);
                    std::string t_newName = t_trans.name().asChar();
                    if(t_oldName.find("__PrenotatoPerDuplicare") != std::string::npos && t_newName.length() > 0)
                    {
                        AddTransform(p_node);
                    }
                    else
                    {
                        s_nodeHandler->NodeNameChangeTransform(t_trans, t_oldName);
                    }
                }
                else if(p_node.hasFn(MFn::kMesh))
                {
                    MFnMesh t_mesh(p_node, &result);
                    std::string t_newName = t_mesh.name().asChar();
                    PrintDebug("Mesh name change ( " + MString(t_oldName.c_str()) + " ) to (" + MString(t_newName.c_str()) + ") ");
                    if(t_oldName.find("__PrenotatoPerDuplicare") != std::string::npos && t_newName.length() > 0)
                    {
                        MMessage::removeCallback(MMessage::currentCallbackId());
                        AddMesh(p_node, false);
                    }
                    else
                    {
                        s_nodeHandler->NodeNameChangeMesh(t_mesh, t_oldName);
                    }
                }
                else if(p_node.hasFn(MFn::kCamera))
                {
                    MFnCamera t_camera(p_node, &result);
                }
                else if(p_node.hasFn(MFn::kLight))
                {
                    MFnLight t_light(p_node, &result);
                    std::string t_newName = t_light.name().asChar();
                    PrintDebug("Light name change ( " + MString(t_oldName.c_str()) + " ) to (" + MString(t_newName.c_str()) + ") ");
                    if(t_oldName.find("__PrenotatoPerDuplicare") != std::string::npos && t_newName.length() > 0)
                    {
                        AddLight(p_node, false);
                    }
                    else
                    {
                        s_nodeHandler->NodeNameChangeLight(t_light, t_oldName);
                    }
                }
                else if(p_node.hasFn(MFn::kLambert))
                {
                    MFnDependencyNode t_material(p_node, &result);
                }
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Cath: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        void CallbackHandler::cb_parentChanges(MDagPath& child, MDagPath& p_parent, void* p_clientData)
        {
            try
            {
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Cath: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        void CallbackHandler::cb_meshEvaluate(MNodeMessage::AttributeMessage p_msg, MPlug& plug_1, MPlug& plug_2, void* p_clientData)
        {
            try
            {
                MStatus result;
                std::string t_plugName(plug_1.name().asChar());
                // Determines if the doublesided attribute has been set. This is to determine whether or not the mesh
                if(t_plugName.find("doubleSided") != std::string::npos && MNodeMessage::AttributeMessage::kAttributeSet)
                {
                    MFnMesh t_mesh(plug_1.node(), &result);
                    if(result)
                    {
                        m_callbackIDArray.append(MNodeMessage::addAttributeChangedCallback(plug_1.node(), cb_meshAttributeChange));
                        MMessage::removeCallback(MNodeMessage::currentCallbackId());
                        s_nodeHandler->AddMeshNode(t_mesh);
                        // s_nodeHandler->PrintVectorInfo();
                    }
                }
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Cath: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        void CallbackHandler::cb_meshAttributeChange(MNodeMessage::AttributeMessage p_msg, MPlug& plug_1, MPlug& plug_2, void* p_clientData)
        {
            try
            {
                MStatus result;
                MFnMesh t_mesh(plug_1.node(), &result);
                if(result)
                {
                    bool exists = false;
                    // for (std::vector<MeshInfo>::size_type i = 0; i<s_nodeHandler->m_mesh)
                    // Might have to put a check if this node exists in the mesh vector
                    if(s_nodeHandler->FindSavedMesh(t_mesh.name().asChar()))
                    {
                        std::string t_plugName = plug_1.name().asChar();
                        if(t_plugName.find("pnts") != std::string::npos && t_plugName.find("[") != std::string::npos)
                        {
                            s_messageHandler->AddMessage(t_mesh.name().asChar(), NodeType::nMesh, MessageType::msgEdited);
                        }
                        else if(plug_2.node().apiType() == MFn::Type::kShadingEngine)
                        {
                            s_messageHandler->AddMessage(t_mesh.name().asChar(), NodeType::nMesh, MessageType::msgEdited);
                            PrintDebug(t_mesh.name() + " Mesh message to be added (shading engine changed)");
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
        void CallbackHandler::cb_meshPolyChange(MObject& p_node, void* p_clientData)
        {
            try
            {
                MStatus result;
                MFnMesh t_mesh(p_node, &result);
                if(result)
                {
                    s_messageHandler->AddMessage(t_mesh.name().asChar(), NodeType::nMesh, MessageType::msgEdited);
                }
                else
                {
                    // Shouldn't happen.
                }
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Cath: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        void CallbackHandler::cb_transformAttributeChange(MNodeMessage::AttributeMessage p_msg, MPlug& plug_1, MPlug& plug_2, void* p_clientData)
        {
            try
            {
                MStatus result;
                if(p_msg & MNodeMessage::AttributeMessage::kAttributeSet)
                {
                    MFnTransform t_transform(plug_1.node(), &result);
                    std::string t_plugName = plug_1.name().asChar();
                    if(result)
                    {
                        if(t_plugName.find(".translate") != std::string::npos || t_plugName.find(".rotate") != std::string::npos ||
                           t_plugName.find(".scale") != std::string::npos || t_plugName.find(".drm") != std::string::npos)
                        {
                            s_messageHandler->AddMessage(t_transform.name().asChar(), NodeType::nTransform, MessageType::msgEdited, "");
                            if(t_transform.childCount() > 0)
                            {

                                MFn::Type filter = MFn::kTransform;
                                MFnDagNode t_dagNodeRoot(plug_1.node());
                                MDagPath t_dagPathRoot;
                                t_dagNodeRoot.getPath(t_dagPathRoot);
                                MItDag t_dagIterator(MItDag::kDepthFirst, filter, &result);
                                t_dagIterator.reset(t_dagPathRoot);

                                for(; !t_dagIterator.isDone(); t_dagIterator.next())
                                {

                                    MFnTransform t_childTransform(t_dagIterator.currentItem(), &result);
                                    if(result)
                                    {
                                        s_messageHandler->AddMessage(t_childTransform.name().asChar(), NodeType::nTransform, MessageType::msgEdited);
                                    }
                                }
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
        void CallbackHandler::cb_cameraPanels(const MString& p_str, MObject& p_node, void* p_clientData)
        {
            try
            {
                if(p_node.hasFn(MFn::kCamera))
                {
                    MFnCamera t_camera(p_node);
                    PrintDebug("Current camera: " + t_camera.name() + " . Old: " + p_str);
                    s_messageHandler->AddDelayedMessage(t_camera.name().asChar(), NodeType::nCamera, MessageType::msgSwitched, "");
                }
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Cath: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        void CallbackHandler::cb_cameraAttributeChange(MNodeMessage::AttributeMessage p_msg, MPlug& plug_1, MPlug& plug_2, void* p_clientData)
        {
            try
            {
                // TODOXX: Detects ALL changes on camera nodes. Would be a good idea to only detect changes on relevant plugs
                MStatus result;
                std::string t_plugName = plug_1.name().asChar();

                MFnCamera t_camera(plug_1.node(), &result);
                if(result)
                {
                    s_messageHandler->AddDelayedMessage(t_camera.name().asChar(), NodeType::nCamera, MessageType::msgEdited, "");
                }
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Cath: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        void CallbackHandler::cb_lightEvaluate(MNodeMessage::AttributeMessage msg, MPlug& plug_1, MPlug& plug_2, void* clientData)
        {
            try
            {
                MStatus result;
                std::string plugName = plug_1.name().asChar();
                MFnLight t_light(plug_1.node(), &result);
                if(result)
                {
                    std::string lightName = t_light.name().asChar();
                    // Makes sure node is finished created by finding out if the name includes a '#' which means the node has not yet
                    // been fully created.
                    if(plugName.find("#") == std::string::npos && MNodeMessage::AttributeMessage::kAttributeSet)
                    {
                        s_nodeHandler->AddLightNode(t_light);
                        m_callbackIDArray.append(MNodeMessage::addAttributeChangedCallback(plug_1.node(), cb_lightAttributeChange));
                        m_callbackIDArray.append(MNodeMessage::addNodePreRemovalCallback(plug_1.node(), cb_preRemoveNode));
                        m_callbackIDArray.append(MNodeMessage::addNameChangedCallback(plug_1.node(), cb_nameChange));
                        MMessage::removeCallback(MMessage::currentCallbackId());
                    }
                }
                else
                {
                    // No action
                }
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Cath: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        void CallbackHandler::cb_lightAttributeChange(MNodeMessage::AttributeMessage p_msg, MPlug& plug_1, MPlug& plug_2, void* p_clientData)
        {
            try
            {
                MFnLight t_light(plug_1.node());
                bool sendMsg = false;
                std::string lightName(t_light.name().asChar());
                std::string plugName(plug_1.name().asChar());
                if(s_nodeHandler->FindSavedLight(lightName))
                {
                    if(p_msg & MNodeMessage::AttributeMessage::kAttributeSet && p_msg != 2052)
                    {
                        MStatus result;
                        if(plugName.find(".intensity") != std::string::npos)
                        {
                            sendMsg = true;
                        }
                        else if(plugName.find(".color") != std::string::npos)
                        {
                            sendMsg = true;
                        }
                        else if(plugName.find(".decayRate") != std::string::npos)
                        {
                            sendMsg = true;
                        }
                        else if(plugName.find(".coneAngle") != std::string::npos)
                        {
                            sendMsg = true;
                        }
                        else if(plugName.find(".dropOff") != std::string::npos)
                        {
                            sendMsg = true;
                        }
                        else if(plugName.find(".penumbraAngle") != std::string::npos)
                        {
                            sendMsg = true;
                        }
                        if(sendMsg)
                        {
                            s_messageHandler->AddMessage(lightName, NodeType::nLight, MessageType::msgEdited);
                            // PrintDebug(t_light.name() + " light attribute changed, message to be added!");
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
        void CallbackHandler::cb_materialAttributeChanges(MNodeMessage::AttributeMessage p_msg, MPlug& plug_1, MPlug& plug_2, void* p_clientData)
        {
            try
            {
                MFnDependencyNode mat(plug_1.node());

                std::string lightName(mat.name().asChar());
                std::string plugName(plug_1.name().asChar());


                if(p_msg & MNodeMessage::AttributeMessage::kAttributeSet && p_msg != 2052)
                {
                    bool sendMsg = false;
                    // outLightData(lightName.asChar());
                    MStatus result;
                    if(plugName.find(".diffuse") != std::string::npos)
                    {
                        sendMsg = true;
                    }
                    else if(plugName.find(".color") != std::string::npos)
                    {
                        sendMsg = true;
                    }
                    else if(plugName.find(".transparency") != std::string::npos)
                    {
                        sendMsg = true;
                    }
                    else if(plugName.find(".ambient") != std::string::npos)
                    {
                        sendMsg = true;
                    }
                    else if(plugName.find(".specularColor") != std::string::npos)
                    {
                        sendMsg = true;
                    }
                    else if(plugName.find(".eccentricity") != std::string::npos)
                    {
                        sendMsg = true;
                    }
                    else if(plugName.find(".specularRollOff") != std::string::npos)
                    {
                        sendMsg = true;
                    }
                    else if(plugName.find(".cosinePower") != std::string::npos)
                    {
                        sendMsg = true;
                    }

                    if(sendMsg)
                    {
                        PrintDebug("Material change " + MString(plugName.c_str()));
                        s_messageHandler->AddMessage(mat.name().asChar(), NodeType::nMaterial, MessageType::msgEdited);
                        // Add a message
                    }
                }
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Cath: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        void CallbackHandler::AddTransform(MObject& p_node)
        {
            MStatus result;
            MFnTransform t_transform(p_node, &result);
            if(result)
            {
                s_nodeHandler->AddCustomAttributesTransform(t_transform);
                m_callbackIDArray.append(MNodeMessage::addNameChangedCallback(p_node, cb_nameChange));
                m_callbackIDArray.append(MNodeMessage::addAttributeChangedCallback(p_node, cb_transformAttributeChange));
                m_callbackIDArray.append(MNodeMessage::addNodePreRemovalCallback(p_node, cb_preRemoveNode));
                s_nodeHandler->AddTransformNode(t_transform);
            }
            else
            {
                PrintError("Could not create MFnTransform from node: " + MString(p_node.apiTypeStr()));
            }
        }
        void CallbackHandler::AddMesh(MObject& p_node, bool p_isNew)
        {
            MStatus result;
            MFnMesh t_mesh(p_node, &result);
            if(t_mesh.isInstanced(false))
            {
                PrintInfo("Mesh " + t_mesh.name() + "is instanced");
            }
            // Makes sure the mesh is not an intermediate node
            if(!t_mesh.isIntermediateObject())
            {
                std::string t_nodeName = t_mesh.name().asChar();
                if(t_nodeName.find("__PrenotatoPerDuplicare") != std::string::npos)
                {
                    m_callbackIDArray.append(MNodeMessage::addNameChangedCallback(p_node, cb_nameChange));
                }
                else
                {
                    // Add Callbacks
                    m_callbackIDArray.append(MNodeMessage::addNameChangedCallback(p_node, cb_nameChange));
                    m_callbackIDArray.append(MPolyMessage::addPolyTopologyChangedCallback(p_node, cb_meshPolyChange));
                    m_callbackIDArray.append(MNodeMessage::addNodePreRemovalCallback(p_node, cb_preRemoveNode));
                    if(p_isNew)
                    {
                        m_callbackIDArray.append(MNodeMessage::addAttributeChangedCallback(p_node, cb_meshEvaluate));
                    }
                    if(result)
                    {
                        // Set quadsplit to left. Ensures that the internal triangulation works properly.
                        MString myCommand = "setAttr -e " + t_mesh.name() + ".quadSplit 0";
                        MGlobal::executeCommandOnIdle(myCommand);
                        m_callbackIDArray.append(MNodeMessage::addAttributeChangedCallback(p_node, cb_meshAttributeChange));
                        s_nodeHandler->AddMeshNode(t_mesh);
                        // TODOJW: Investigate and add duplicate/instancing callbacks.
                    }
                    else
                    {
                        PrintError("Could not create MFnMesh from node: " + MString(p_node.apiTypeStr()));
                    }
                }
            }
        }
        void CallbackHandler::AddCamera(MObject& p_node)
        {
            try
            {
                MStatus result;
                MFnCamera t_camera(p_node, &result);
                if(result)
                {
                    m_callbackIDArray.append(MNodeMessage::addAttributeChangedCallback(p_node, cb_cameraAttributeChange));
                    s_nodeHandler->AddCameraNode(t_camera);
                }
                else
                {
                    // Do nada
                }
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Cath: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        void CallbackHandler::AddLight(MObject& p_node, bool p_isNew)
        {
            try
            {
                MStatus result;
                MFnLight t_light(p_node, &result);
                std::string t_nodeName = t_light.name().asChar();
                if(t_nodeName.find("__PrenotatoPerDuplicare") != std::string::npos)
                {
                    m_callbackIDArray.append(MNodeMessage::addNameChangedCallback(p_node, cb_nameChange));
                }
                else if(result)
                {
                    if(p_node.hasFn(MFn::kDirectionalLight) || p_node.hasFn(MFn::kSpotLight) || p_node.hasFn(MFn::kPointLight))
                    {
                        if(p_isNew)
                        {
                            m_callbackIDArray.append(MNodeMessage::addAttributeChangedCallback(p_node, cb_lightEvaluate));
                        }
                        else
                        {
                            m_callbackIDArray.append(MNodeMessage::addAttributeChangedCallback(p_node, cb_lightAttributeChange));
                            m_callbackIDArray.append(MNodeMessage::addNodePreRemovalCallback(p_node, cb_preRemoveNode));
                            m_callbackIDArray.append(MNodeMessage::addNameChangedCallback(p_node, cb_nameChange));
                            s_nodeHandler->AddLightNode(t_light);
                        }
                    }
                }
                else
                {
                    // Do nothing maybe?
                }
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Cath: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        void CallbackHandler::AddMaterial(MObject& p_node)
        {
            try
            {
                MStatus result;
                MFnDependencyNode t_material(p_node);
                m_callbackIDArray.append(MNodeMessage::addAttributeChangedCallback(p_node, cb_materialAttributeChanges));
                m_callbackIDArray.append(MNodeMessage::addNodePreRemovalCallback(p_node, cb_preRemoveNode));
                m_callbackIDArray.append(MNodeMessage::addNameChangedCallback(p_node, cb_nameChange));
                s_nodeHandler->AddMaterialNode(t_material);
                PrintDebug(t_material.name() + " material found");
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Cath: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
    }
}