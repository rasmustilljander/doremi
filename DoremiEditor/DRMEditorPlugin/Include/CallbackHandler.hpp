#pragma once
#ifndef CALLBACKHANDLER_HPP
#define CALLBACKHANDLER_HPP
#include <DoremiEditor\DRMEditorPlugin\Include\Includes.hpp>
namespace DoremiEditor
{
    namespace Plugin
    {
        class NodeHandler;
        class MessageHandler;
        class Filemapping;
        class CallbackHandler
        {
        private:
        public:
            // Default Constructor
            CallbackHandler();
            // Default Destructor
            ~CallbackHandler();

            static void Initialize();

            static MCallbackIdArray m_callbackIDArray;
            static M3dView m_modelPanel;
            static float m_updateFrequency;

            static NodeHandler* s_nodeHandler;
            static MessageHandler* s_messageHandler;
            /**
            TODOJW : Research Undo/Redo functionality (might not be necessary. possible solution is to provide a GUI button for reloading of objects
            */
            /**
            Function for loading and adding callbacks and functionality to existing nodes in a scene
            */
            static void LoadScene();
            static void UnloadScene(bool p_doTransforms = true, bool p_doMeshes = true, bool p_doCameras = true, bool p_doLights = true, bool p_doMaterials = true);

            /**
            AFFECTS: All nodes
            Callback that registers new nodes (Mesh, transform, light and materials)
            */
            static void cb_newNode(MObject& node, void* clientData);

            /**
            AFFECTS: All nodes
            Timed callback for message updates
            */
            static void cb_timer(float elapsedTime, float lastTime, void* clientData);

            /**
            AFFECTS: All nodes
            Callback that detects a node removal before it has been completely removed so that the node name and type can still be accessed
            */
            static void cb_preRemoveNode(MObject& node, void* clientData);

            /**
            AFFECTS: All nodes
            Detects name changes on nodes
            */
            static void cb_nameChange(MObject& node, const MString& str, void* clientData);

            /**
            AFFECTS: Transforms, Meshes, Lights, Cameras
            Parent changes callback
            TODOJW : Analyze functions from original project as there are two different almost identical functions
            (added/removed). It should be possible to combine these two
            */
            static void cb_parentChanges(MDagPath& child, MDagPath& parent, void* clientData);
            /**
            AFFECTS: Transforms, Meshes, Lights, Cameras
            Parent changes callback
            TODOJW : Analyze functions from original project as there are two different almost identical functions
            (added/removed). It should be possible to combine these two
            */
            static void cb_parentChange(MDagMessage::DagMessage p_msgType, MDagPath& p_child, MDagPath& p_parent, void* p_clientData);


            /**
            AFFECTS: Meshes
            Evaluates if a new mesh is complete
            */
            static void cb_meshEvaluate(MNodeMessage::AttributeMessage msg, MPlug& plug_1, MPlug& plug_2, void* clientData);

            /**
            AFFECTS: Meshes
            Mesh Attribute Changes
            */
            static void cb_meshAttributeChange(MNodeMessage::AttributeMessage msg, MPlug& plug_1, MPlug& plug_2, void* clientData);

            /**
            AFFECTS: Meshes
            Detects changes on mesh topology (extrudes, edge loops etc)
            */
            static void cb_meshPolyChange(MObject& node, void* clientData);

            /**
            AFFECTS: Transforms
            Detects transform changes
            */
            static void cb_transformAttributeChange(MNodeMessage::AttributeMessage msg, MPlug& plug_1, MPlug& plug_2, void* clientData);

            /**
            AFFECTS:  Camera (panels)
            Callback for managing the current camera. Recognizes switches between different cameras
            */
            static void cb_cameraPanels(const MString& str, MObject& node, void* clientData);

            /**
            AFFECTS: Cameras
            Detects camera attribute changes
            */
            static void cb_cameraAttributeChange(MNodeMessage::AttributeMessage msg, MPlug& plug_1, MPlug& plug_2, void* clientData);

            /**
            AFFECTS: Lights
            Evaluates new light
            */
            static void cb_lightEvaluate(MNodeMessage::AttributeMessage msg, MPlug& plug_1, MPlug& plug_2, void* clientData);

            /**
            AFFECTS: Lights
            Detects light changes
            */
            static void cb_lightAttributeChange(MNodeMessage::AttributeMessage msg, MPlug& plug_1, MPlug& plug_2, void* clientData);

            /**
            AFFECTS: Mateörials
            Detects Material changes
            */
            static void cb_materialAttributeChanges(MNodeMessage::AttributeMessage msg, MPlug& plug_1, MPlug& plug_2, void* clientData);


            static void AddTransform(MObject& p_node);
            static void AddMesh(MObject& p_node, bool p_isNew);
            static void AddCamera(MObject& p_node);
            static void AddLight(MObject& p_node, bool p_isNew);
            static void AddMaterial(MObject& p_node);
        };
    }
}

#endif