#include <Doremi/Core/Include/PotentialFieldGridCreator.hpp>
#include <Doremi/Core/Include/EntityComponent/EntityHandler.hpp>
#include <Doremi/Core/Include/EntityComponent/Constants.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/PotentialFieldComponent.hpp>
// Engine
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/PhysicsMaterialManager.hpp>
#include <DoremiEngine/Physics/Include/RayCastManager.hpp>
#include <DoremiEngine/Physics/Include/RigidBodyManager.hpp>
#include <DoremiEngine/AI/Include/Interface/PotentialField/PotentialField.hpp>
#include <DoremiEngine/AI/Include/Interface/PotentialField/PotentialFieldActor.hpp>
#include <DoremiEngine/AI/Include/Interface/SubModule/PotentialFieldSubModule.hpp>
#include <DoremiEngine/AI/Include/AIModule.hpp>

// General
#include <DirectXMath.h>
#include <iostream>

namespace Doremi
{
    namespace Core
    {

        PotentialFieldGridCreator::PotentialFieldGridCreator(const DoremiEngine::Core::SharedContext& p_sharedContext)
            : m_sharedContext(p_sharedContext)
        {
        }

        PotentialFieldGridCreator::~PotentialFieldGridCreator() {}

        void PotentialFieldGridCreator::BuildGridUsingPhysicXAndGrid(DoremiEngine::AI::PotentialField* op_field)
        {
            using namespace DirectX;
            int materialID = m_sharedContext.GetPhysicsModule().GetPhysicsMaterialManager().CreateMaterial(0, 0, 0);
            Core::EntityHandler& t_entityHandler = Core::EntityHandler::GetInstance();

            XMFLOAT2 quadSize = op_field->GetQuadSize();
            std::vector<std::vector<DoremiEngine::AI::PotentialFieldGridPoint>> grid = op_field->GetGrid();
            size_t gridSizeX = grid.size();
            size_t gridSizeZ = grid[0].size();
            float centerGridY = op_field->GetCenter().y;
            for(size_t x = 0; x < gridSizeX; ++x)
            // thread this place TODOKO
            {
                for(size_t z = 0; z < gridSizeZ; ++z)
                {
                    XMFLOAT3 sweepOrigin = XMFLOAT3(grid[x][z].position.x, centerGridY, grid[x][z].position.y);
                    XMFLOAT3 boxHalfExtents = XMFLOAT3(quadSize.x * 0.5f, 2, quadSize.y * 0.5);
                    int myID = MAX_NUM_ENTITIES + (z + x * gridSizeZ);
                    // m_sharedContext.GetPhysicsModule().GetRigidBodyManager().AddBoxBodyStatic(MAX_NUM_ENTITIES + 1, sweepOrigin, XMFLOAT4(0, 0, 0,
                    // 1), boxHalfExtents, materialID);
                    std::vector<int> sweepHits = m_sharedContext.GetPhysicsModule().GetRayCastManager().OverlapBoxMultipleHits(sweepOrigin, boxHalfExtents);

                    // m_sharedContext.GetPhysicsModule().GetRigidBodyManager().AddBoxBodyDynamic(myID, quadCenter, XMFLOAT4(0, 0, 0, 1),
                    //                                                                           XMFLOAT3(quadSize.x * 0.5f, 0.5f, quadSize.y * 0.5f),
                    //                                                                           materialID);
                    // m_sharedContext.GetPhysicsModule().GetRigidBodyManager().SetTrigger(myID, true);
                    // m_sharedContext.GetPhysicsModule().Update(0.017); // is this needed?
                    // std::vector<DoremiEngine::Physics::CollisionPair> collisionPairs = m_sharedContext.GetPhysicsModule().GetTriggerPairs(); //
                    // GetCollisionPairs();

                    // size_t collisionListLength = collisionPairs.size();
                    size_t numberOfHits = sweepHits.size();
                    // int hej = t_entityHandler.GetInstance().GetLastEntityIndex();
                    for(size_t i = 0; i < numberOfHits; ++i)
                    {
                        int objectID = sweepHits[i];

                        if(EntityHandler::GetInstance().HasComponents(objectID, (int)ComponentType::PotentialField))
                        {
                            DoremiEngine::AI::PotentialFieldActor* actor =
                                EntityHandler::GetInstance().GetComponentFromStorage<PotentialFieldComponent>(objectID)->ChargedActor;
                            actor->AddOccupiedQuad(XMINT2(x, z));
                            op_field->AddActor(actor);
                            break; // If more than on collision occures, fuck it >D
                        }
                    }
                }
            }
            // Någon fysikclass ska in
            op_field->Update();
            std::cout << "Field done " << std::endl;
            // op_field->
        }
    }
}