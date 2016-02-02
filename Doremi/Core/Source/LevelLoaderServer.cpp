/// Game side
#include <LevelLoaderServer.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <EntityComponent/EntityFactory.hpp>
// Components
#include <EntityComponent/Components/TransformComponent.hpp>
#include <EntityComponent/Components/RenderComponent.hpp>
#include <EntityComponent/Components/PotentialFieldComponent.hpp>
#include <EntityComponent/Components/RigidBodyComponent.hpp>
#include <EntityComponent/Components/TriggerComponent.hpp>
/// Engine side
#include <DoremiEngine/Core/Include/SharedContext.hpp>
// Graphic
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/SubModuleManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/MeshManager.hpp>

// AI
#include <DoremiEngine/AI/Include/AIModule.hpp>
#include <DoremiEngine/AI/Include/Interface/SubModule/PotentialFieldSubModule.hpp>

// Timing
#include <DoremiEngine/Timing/Include/Measurement/TimeMeasurementManager.hpp>

// Physics

/// DEBUG physics TODOJB remove
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/PhysicsMaterialManager.hpp>

#include <DoremiEngine/Physics/Include/RigidBodyManager.hpp>

/// Standard
#include <sstream>
#include <fstream>

namespace Doremi
{
    namespace Core
    {
        LevelLoaderServer::LevelLoaderServer(const DoremiEngine::Core::SharedContext& p_sharedContext) : LevelLoader(p_sharedContext) {}

        LevelLoaderServer::~LevelLoaderServer() {}

        void LevelLoaderServer::LoadLevel(const std::string& p_fileName)
        {
            using namespace std;
            string fileName = m_sharedContext.GetWorkingDirectory() + p_fileName;
            ifstream ifs;
            ifs.open(fileName, ifstream::in | ofstream::binary);
            if(ifs.is_open() == true)
            {
                // testa o läsa lite

                // scene name
                int sceneNameSize;
                ifs.read((char*)&sceneNameSize, sizeof(int));
                char* sceneName = new char[sceneNameSize];
                ifs.read((char*)sceneName, sizeof(char) * sceneNameSize);

                // how much different stuff there is
                int nrMats, nrTransforms, nrMeshes, nrLights;
                ifs.read((char*)&nrMats, sizeof(int));
                ifs.read((char*)&nrTransforms, sizeof(int));
                ifs.read((char*)&nrMeshes, sizeof(int));
                ifs.read((char*)&nrLights, sizeof(int));

                LoadMaterial(ifs, nrMats);
                LoadTransforms(ifs, nrTransforms);
                LoadMeshes(ifs, nrMeshes);
                LoadLights(ifs, nrLights);
                BuildEntities();
                LoadTriggers();
            }
            else
            {
                // TODOXX DO something here?
            }
        }

        void LevelLoaderServer::BuildComponents(int p_entityId, int p_meshCouplingID, std::vector<DoremiEngine::Graphic::Vertex>& p_vertexBuffer)
        {
            const ObjectCouplingInfo& meshCoupling = m_meshCoupling[p_meshCouplingID];
            // Adds transform components to the world
            EntityHandler::GetInstance().AddComponent(p_entityId, (int)ComponentType::Transform);
            TransformComponent* transComp = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(p_entityId);

            transComp->position = m_transforms[meshCoupling.transformName].translation;
            transComp->rotation = m_transforms[meshCoupling.transformName].rotation;
            transComp->scale = m_transforms[meshCoupling.transformName].scale;

            // Adds potential field components to the world
            EntityHandler::GetInstance().AddComponent(p_entityId, (int)ComponentType::PotentialField);
            PotentialFieldComponent* potComp = EntityHandler::GetInstance().GetComponentFromStorage<PotentialFieldComponent>(p_entityId);
            potComp->ChargedActor =
                m_sharedContext.GetAIModule().GetPotentialFieldSubModule().CreateNewActor(transComp->position, 0, 3, true); // TODOKO hardcoded shiet
        }
    }
}
