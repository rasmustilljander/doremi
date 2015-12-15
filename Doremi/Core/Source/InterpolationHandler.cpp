#include <InterpolationHandler.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <EntityComponent/Components/TransformComponent.hpp>

namespace Doremi
{
    namespace Core
    {
        InterpolationHandler* InterpolationHandler::m_singleton = nullptr;

        InterpolationHandler* InterpolationHandler::GetInstance()
        {
            if(m_singleton == nullptr)
            {
                m_singleton = new InterpolationHandler();
            }
            return m_singleton;
        }

        InterpolationHandler::InterpolationHandler() {}

        InterpolationHandler::~InterpolationHandler() {}

        void InterpolationHandler::InterpolateFrame(double p_alpha)
        {
            EntityHandler& EntityHandler = EntityHandler::GetInstance();

            // All objects that is sent over network with transform should be inteprolated
            uint32_t Mask = (int)ComponentType::Transform | (int)ComponentType::NetworkObject;

            uint32_t NumberOfEntities = EntityHandler.GetLastEntityIndex();


            for(size_t EntityID = 0; EntityID < NumberOfEntities; EntityID++)
            {
                if(EntityHandler.HasComponents(EntityID, Mask))
                {
                    TransformComponent* DrawTransform = EntityHandler.GetComponentFromStorage<TransformComponent>(EntityID);
                    TransformComponentPrevious* PrevTransform = EntityHandler.GetComponentFromStorage<TransformComponentPrevious>(EntityID);
                    TransformComponentNext* NextTransform = EntityHandler.GetComponentFromStorage<TransformComponentNext>(EntityID);

                    // Load values for position
                    DirectX::XMVECTOR PreviousPositionVector = DirectX::XMLoadFloat3(&PrevTransform->position);
                    DirectX::XMVECTOR NextPositionVector = DirectX::XMLoadFloat3(&NextTransform->position);
                    DirectX::XMVECTOR FinalPositionVector;

                    // Load values for orientation
                    DirectX::XMVECTOR PrevOrientationVector = DirectX::XMLoadFloat4(&PrevTransform->rotation);
                    DirectX::XMVECTOR NextOrientationVector = DirectX::XMLoadFloat4(&NextTransform->rotation);
                    DirectX::XMVECTOR FinalOrientationVector;

                    // Calculate interpolation for position
                    FinalPositionVector = DirectX::XMVectorAdd(DirectX::XMVectorScale(PreviousPositionVector, (1.0f - p_alpha)),
                                                               DirectX::XMVectorScale(NextPositionVector, p_alpha));

                    // Calculate interpolation for orientation
                    FinalOrientationVector = DirectX::XMQuaternionSlerp(PrevOrientationVector, NextOrientationVector, p_alpha);
                    // FinalOrientationVector = DirectX::XMVectorAdd(DirectX::XMVectorScale(PrevOrientationVector, (1.0f - p_alpha)),
                    // DirectX::XMVectorScale(NewOrientationVector, p_alpha));

                    // Set values for position and orientation
                    DirectX::XMStoreFloat3(&DrawTransform->position, FinalPositionVector);
                    DirectX::XMStoreFloat4(&DrawTransform->rotation, FinalOrientationVector);
                }
            }
        }
    }
}