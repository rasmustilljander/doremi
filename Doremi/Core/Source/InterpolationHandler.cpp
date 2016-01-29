#include <InterpolationHandler.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <EntityComponent/Components/TransformComponent.hpp>
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/CharacterControlManager.hpp>
#include <PlayerHandler.hpp>
#include <SequenceMath.hpp>
#include <iostream>
#include <algorithm>
#include <Utility/Timer/Include/Measure/MeasureTimer.hpp>

namespace Doremi
{
    namespace Core
    {
        InterpolationHandler* InterpolationHandler::m_singleton = nullptr;

        void InterpolationHandler::StartInterpolationHandler(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            if(m_singleton == nullptr)
            {
                m_singleton = new InterpolationHandler(p_sharedContext);
            }
            else
            {
                std::runtime_error("Duplicate startup called on InterpolationHandler.");
            }
        }

        InterpolationHandler* InterpolationHandler::GetInstance()
        {
            if(m_singleton == nullptr)
            {
                std::runtime_error("InterpolationHandler called GetInstance befor startup.");
            }
            return m_singleton;
        }
        // TODOXX snapshotdelay must be less then sequencedleay in inputhandlerserver
        InterpolationHandler::InterpolationHandler(const DoremiEngine::Core::SharedContext& p_sharedContext)
            : m_sharedContext(p_sharedContext), m_snapshotSequenceReal(0), m_snapshotDelay(5)
        {
        }

        InterpolationHandler::~InterpolationHandler() {}

        void InterpolationHandler::InterpolateFrame(double p_alpha)
        {
            TIME_FUNCTION_START

            EntityHandler& EntityHandler = EntityHandler::GetInstance();

            // All objects that is sent over network with transform should be inteprolated
            uint32_t Mask = (int)ComponentType::Transform | (int)ComponentType::NetworkObject;

            uint32_t NumberOfEntities = EntityHandler.GetLastEntityIndex();

            // Compute real alpha
            float realAlpha =
                (static_cast<float>(p_alpha) + static_cast<float>(m_SequenceInterpolationOffset)) / static_cast<float>(m_NumOfSequencesToInterpolate);

            if(m_NumOfSequencesToInterpolate <= 0)
            {
                // std::cout << "SOMETHING WRONG: " << m_NumOfSequencesToInterpolate << std::endl;
            }

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
                    FinalPositionVector = DirectX::XMVectorAdd(DirectX::XMVectorScale(PreviousPositionVector, (1.0f - realAlpha)),
                                                               DirectX::XMVectorScale(NextPositionVector, realAlpha));

                    // Calculate interpolation for orientation
                    FinalOrientationVector = DirectX::XMQuaternionSlerp(PrevOrientationVector, NextOrientationVector, realAlpha);
                    // FinalOrientationVector = DirectX::XMVectorAdd(DirectX::XMVectorScale(PrevOrientationVector, (1.0f - p_alpha)),
                    // DirectX::XMVectorScale(NewOrientationVector, p_alpha));

                    // Set values for position and orientation
                    DirectX::XMStoreFloat3(&DrawTransform->position, FinalPositionVector);
                    DirectX::XMStoreFloat4(&DrawTransform->rotation, FinalOrientationVector);

                    // cout << DrawTransform->position.y << endl; TODOCM remove
                }
            }
            TIME_FUNCTION_STOP
        }

        void InterpolationHandler::UpdateInterpolationTransforms()
        {
            TIME_FUNCTION_START
            // If the snapshot we're currently interpolating towards is more recent
            // then the one that we should be interpolating towards, we skip
            // cout << (int)m_snapshotSequenceReal << " " << (int)m_snapshotSequenceUsed << " "
            //     << ((int)m_snapshotSequenceReal - (int)m_snapshotSequenceUsed) << endl;

            EntityHandler& entityHandler = EntityHandler::GetInstance();
            size_t NumEntities = entityHandler.GetLastEntityIndex();
            int mask = (int)ComponentType::NetworkObject | (int)ComponentType::Transform;

            for(size_t entityID = 0; entityID < NumEntities; entityID++)
            {
                if(entityHandler.HasComponents(entityID, mask))
                {
                    // Increment the frame by 1
                    GetComponent<TransformComponentSnapshotNext>(entityID)->IncrementFrame();
                }
            }

            if(sequence_more_recent(m_snapshotSequenceReal, m_snapshotSequenceUsed, 255))
            {
                // Clone so that the changed values are saved
                // TODOCM maybe change method how to update saved snapshot positions here


                // SwapShelf<TransformComponentPrevious, TransformComponentNext>();
                CloneShelf<TransformComponentNext, TransformComponentPrevious>();


                m_SequenceInterpolationOffset = 0;

                // If we have any snapshots
                if(m_DelayedSnapshots.size())
                {
                    // Loop through from back to check which one is newest
                    std::list<Snapshot*>::reverse_iterator iter;
                    std::list<Snapshot*>::reverse_iterator iterEnd = m_DelayedSnapshots.rend();

                    std::list<Snapshot*>::iterator iterRemoveStart = m_DelayedSnapshots.end();
                    std::list<Snapshot*>::iterator iterRemoveEnd = m_DelayedSnapshots.end();

                    int AmountOfSnapshots = m_DelayedSnapshots.size();

                    for(iter = m_DelayedSnapshots.rbegin(); iter != iterEnd; ++iter)
                    {
                        if(sequence_more_recent((*iter)->SnapshotSequence, m_snapshotSequenceReal - 1, 255))
                        {
                            // If we find somone that is the same or infront of us
                            break;
                        }
                        else if(m_DelayedSnapshots.size() > 1)
                        {
                            // Remove it
                            iterRemoveStart = --iter.base();
                            AmountOfSnapshots--;
                            if(AmountOfSnapshots == 1)
                            {
                                break;
                            }
                        }
                        else
                        {
                            break;
                        }
                    }

                    m_DelayedSnapshots.erase(iterRemoveStart, iterRemoveEnd);

                    // We somehow need to check if it's the last anyway because of the alpha?
                    Snapshot* SnapshotToUse = m_DelayedSnapshots.back();

                    // If the snapshot we are to use is ahead, we check how far ahead it is, else we just interpolate it one frame
                    if(sequence_more_recent(SnapshotToUse->SnapshotSequence, m_snapshotSequenceReal - 1, 255))
                    {
                        m_NumOfSequencesToInterpolate = sequence_difference(SnapshotToUse->SnapshotSequence, m_snapshotSequenceReal - 1, 255);
                    }
                    else
                    {
                        m_NumOfSequencesToInterpolate = 1;
                    }

                    // Update the new Next transform array
                    // TODOCM we assume the client and server have the same entities
                    // TODOCM we need in some way be sure that if we remove something we remove it befor adding something, example sending rendudant
                    // data many frames or something

                    // What if we loop through all objects with NetworkObjComp, for all objects we DONT find, we get the next by making a diff between
                    // the two?


                    // Sort the objects by ID
                    std::sort(&SnapshotToUse->Objects[0], &SnapshotToUse->Objects[SnapshotToUse->NumOfObjects], SnapObjIDMoreRecent);

                    uint32_t objectCounter = 0;

                    size_t entityID = 0;

                    float extrapolateAlpha = static_cast<float>(m_NumOfSequencesToInterpolate);

                    // If there are any objects in snapshot
                    // TODOCM check if there can be no objects in snapshot, if so remove this check
                    if(SnapshotToUse->NumOfObjects)
                    {
                        EntityID nextID = SnapshotToUse->Objects[objectCounter].EntityID;

                        // Go through all entities
                        for(; entityID < NumEntities; entityID++)
                        {
                            // If ID is in snapshot
                            if(entityID == nextID)
                            {
                                // If netobject and has transform
                                if(entityHandler.HasComponents(entityID, mask))
                                {
                                    // Change old new snapshot to previous snapshot
                                    *GetComponent<TransformComponentSnapshotPrevious>(entityID) =
                                        *(TransformComponentSnapshotPrevious*)GetComponent<TransformComponentSnapshotNext>(entityID);

                                    // Set incomming snapshot data to next to use for interpolation, and snapshot new
                                    *GetComponent<TransformComponentSnapshotNext>(entityID) =
                                        TransformComponentSnapshotNext(SnapshotToUse->Objects[objectCounter].Component);
                                    *GetComponent<TransformComponentNext>(entityID) = TransformComponentNext(SnapshotToUse->Objects[objectCounter].Component);
                                }

                                // Increase counter
                                objectCounter++;

                                // If there are no objects left
                                if(objectCounter == SnapshotToUse->NumOfObjects)
                                {
                                    entityID++;
                                    break;
                                }

                                // Get next ID
                                nextID = SnapshotToUse->Objects[objectCounter].EntityID;
                            }

                            else if(entityHandler.HasComponents(entityID, mask)) // If not ID we extrapolate
                            {
                                TransformComponentNext* next = GetComponent<TransformComponentNext>(entityID);
                                TransformComponentSnapshotNext* snapNext = GetComponent<TransformComponentSnapshotNext>(entityID);
                                TransformComponentSnapshotPrevious* snapPrev = GetComponent<TransformComponentSnapshotPrevious>(entityID);

                                // We extrapolate with distance between the values as how many frames they were,
                                *next = ExtrapolateTransform(snapPrev, snapNext, extrapolateAlpha);
                            }
                        }
                    }


                    // Extrapolate remaining
                    for(; entityID < NumEntities; entityID++)
                    {
                        // If netobject and has transform
                        if(entityHandler.HasComponents(entityID, mask))
                        {
                            TransformComponentNext* next = GetComponent<TransformComponentNext>(entityID);
                            TransformComponentSnapshotNext* snapNext = GetComponent<TransformComponentSnapshotNext>(entityID);
                            TransformComponentSnapshotPrevious* snapPrev = GetComponent<TransformComponentSnapshotPrevious>(entityID);

                            // We extrapolate with distance between the values as how many frames they were,
                            *next = ExtrapolateTransform(snapPrev, snapNext, extrapolateAlpha);
                        }
                    }


                    // We set the sequence we're using and remove it from the list
                    m_snapshotSequenceUsed = SnapshotToUse->SnapshotSequence;
                    m_DelayedSnapshots.pop_back();
                    delete SnapshotToUse;
                }
                else // If we dont have any snapshots we will lagg, this is 2 missed packages for now, ask Christian if this might change
                {
                    // std::cout << "Lost more then two snapshots?" << std::endl;
                    m_NumOfSequencesToInterpolate = 1;

                    // TODOCM NOT TESTED to extrapolate if we don't have anything
                    // Extrapolate remaining
                    for(size_t entityID = 0; entityID < NumEntities; entityID++)
                    {
                        // If netobject and has transform
                        if(entityHandler.HasComponents(entityID, mask))
                        {
                            TransformComponentNext* next = GetComponent<TransformComponentNext>(entityID);
                            TransformComponentSnapshotNext* snapNext = GetComponent<TransformComponentSnapshotNext>(entityID);
                            TransformComponentSnapshotPrevious* snapPrev = GetComponent<TransformComponentSnapshotPrevious>(entityID);

                            // We extrapolate with distance between the values as how many frames they were,
                            *next = ExtrapolateTransform(snapPrev, snapNext, m_NumOfSequencesToInterpolate);
                        }
                    }
                }
            }
            else
            {
                m_SequenceInterpolationOffset++;
                // std::cout << "Doing a long interpolation..." << std::endl;
            }
            m_snapshotSequenceReal++;
            // std::cout << "NumOfBufferedSnapshots: " << m_DelayedSnapshots.size() << std::endl;
            TIME_FUNCTION_STOP
        }

        void InterpolationHandler::SetSequence(uint8_t p_sequence)
        {
            // std::cout << "Setting new snapshotsequence" << std::endl;
            m_snapshotSequenceReal = p_sequence - m_snapshotDelay;
            m_snapshotSequenceUsed = p_sequence - m_snapshotDelay;
        }


        void InterpolationHandler::QueueSnapshot(Snapshot* p_newSnapshot)
        {
            // If we get n' old snapshot
            if(sequence_more_recent(m_snapshotSequenceUsed, p_newSnapshot->SnapshotSequence, 255)) // TODOCM check if it should be equal less
            {
                // std::cout << "Throwing snapshot" << std::endl;
                delete p_newSnapshot;
                return;
            }

            size_t NumberOfSnapshots = m_DelayedSnapshots.size();

            // If there are no saved snapshots
            if(NumberOfSnapshots == 0)
            {
                m_DelayedSnapshots.push_back(p_newSnapshot);
                return;
            }

            // Else we check for all if our sequence is newer(If we got them in wrong order)
            std::list<Snapshot*>::iterator iter;
            for(iter = m_DelayedSnapshots.begin(); iter != m_DelayedSnapshots.end(); ++iter)
            {
                // If the iterator is larger then snapshotID, we push the new snapshot at the back
                if(sequence_more_recent(p_newSnapshot->SnapshotSequence, (*iter)->SnapshotSequence, 255))
                {
                    m_DelayedSnapshots.insert(iter, p_newSnapshot);
                    return;
                }
            }

            // If it was larger then all we put it in front
            m_DelayedSnapshots.push_front(p_newSnapshot);
        }

        uint8_t InterpolationHandler::GetRealSnapshotSequence() { return m_snapshotSequenceReal; }
    }
}