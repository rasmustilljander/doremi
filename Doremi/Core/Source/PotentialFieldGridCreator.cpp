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

            XMFLOAT2 quadSize = op_field->GetQuadSize();
            size_t gridSizeX = op_field->GetNumberOfQuadsWidth();
            size_t gridSizeZ = op_field->GetNumberOfQuadsHeight();
            for(size_t x = 0; x < gridSizeX; ++x)
            // thread this place TODOKO
            {
                for(size_t z = 0; z < gridSizeZ; ++z)
                {
                    XMFLOAT3 sweepOrigin = op_field->GetGridQuadPosition(x, z);
                    XMFLOAT3 boxHalfExtents = XMFLOAT3(quadSize.x * 0.5f, 2, quadSize.y * 0.5);
                    // m_sharedContext.GetPhysicsModule().GetRigidBodyManager().AddBoxBodyStatic(MAX_NUM_ENTITIES + 1, sweepOrigin, XMFLOAT4(0, 0, 0,
                    // 1), boxHalfExtents, materialID);
                    std::vector<int> sweepHits = m_sharedContext.GetPhysicsModule().GetRayCastManager().OverlapBoxMultipleHits(sweepOrigin, boxHalfExtents);

                    size_t numberOfHits = sweepHits.size();
                    for(size_t i = 0; i < numberOfHits; ++i)
                    {
                        int objectID = sweepHits[i];

                        if(EntityHandler::GetInstance().HasComponents(objectID, (int)ComponentType::PotentialField))
                        {
                            PotentialFieldComponent* pfComp = EntityHandler::GetInstance().GetComponentFromStorage<PotentialFieldComponent>(objectID);
                            if(!pfComp->isField && pfComp->ChargedActor != nullptr)
                            {
                                pfComp->ChargedActor->AddOccupiedQuad(XMINT2(x, z));
                                op_field->AddActor(pfComp->ChargedActor);
                                break; // If more than on collision occures, fuck it >D
                            }
                        }
                    }
                }
            }
            // Någon fysikclass ska in
            op_field->Update();

            m_sharedContext.GetAIModule().GetPotentialFieldSubModule().SaveFieldToFile(*op_field, op_field->GetName());
            // op_field->
        }
    }
}