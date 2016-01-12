#include <InterpolationHandler.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <EntityComponent/Components/TransformComponent.hpp>
#include <SequenceMath.hpp>
#include <iostream>

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

        InterpolationHandler::InterpolationHandler() : m_snapshotSequenceReal(0), m_snapshotDelay(10) {}

        InterpolationHandler::~InterpolationHandler() {}

        void InterpolationHandler::InterpolateFrame(double p_alpha)
        {
            EntityHandler& EntityHandler = EntityHandler::GetInstance();

            // All objects that is sent over network with transform should be inteprolated
            uint32_t Mask = (int)ComponentType::Transform | (int)ComponentType::NetworkObject;

            uint32_t NumberOfEntities = EntityHandler.GetLastEntityIndex();

            // Compute real alpha
            double realAlpha = (p_alpha + (double)m_SequenceInterpolationOffset) / (double)m_NumOfSequencesToInterpolate;

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
                    FinalPositionVector =
                        DirectX::XMVectorAdd(DirectX::XMVectorScale(PreviousPositionVector, (1.0f - realAlpha)),
                                             DirectX::XMVectorScale(NextPositionVector, realAlpha));

                    // Calculate interpolation for orientation
                    FinalOrientationVector =
                        DirectX::XMQuaternionSlerp(PrevOrientationVector, NextOrientationVector, realAlpha);
                    // FinalOrientationVector = DirectX::XMVectorAdd(DirectX::XMVectorScale(PrevOrientationVector, (1.0f - p_alpha)),
                    // DirectX::XMVectorScale(NewOrientationVector, p_alpha));

                    // Set values for position and orientation
                    DirectX::XMStoreFloat3(&DrawTransform->position, FinalPositionVector);
                    DirectX::XMStoreFloat4(&DrawTransform->rotation, FinalOrientationVector);

                    //cout << DrawTransform->position.y << endl; TODOCM remove
                }
            }
        }

        void InterpolationHandler::UpdateInterpolationTransforms()
        {
            // If the snapshot we're currently interpolating towards is more recent
            // then the one that we should be interpolating towards, we skip
            // cout << (int)m_snapshotSequenceReal << " " << (int)m_snapshotSequenceUsed << " "
            //     << ((int)m_snapshotSequenceReal - (int)m_snapshotSequenceUsed) << endl;
            if(sequence_more_recent(m_snapshotSequenceReal, m_snapshotSequenceUsed, 255))
            {
                // Clone so that the changed values are saved
                // TODOCM maybe change method how to update saved snapshot positions here
                CloneShelf<TransformComponentNext, TransformComponentPrevious>();

                m_SequenceInterpolationOffset = 0;

                // If we have any snapshots
                if (m_DelayedSnapshots.size())
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

                    // Loop to the last item (excluding it in the loop because we want to use it if it's the oldest
                    // for (iter = m_DelayedSnapshots.rbegin(); iter != iterEnd; ++iter)
                    //{
                    //    if (sequence_more_recent((*iter)->SnapshotSequence, m_snapshotSequenceReal - 1, 255))
                    //    {
                    //        // If we find somone that is the same or infront of us
                    //        break;
                    //    }
                    //    else if (m_DelayedSnapshots.size() > 1)
                    //    {
                    //        if (amountOfSnapshotBeforCrash == 2)
                    //        {
                    //            int a = 3;
                    //        }

                    //        // If older, we remove it
                    //        std::list<Snapshot*>::iterator tempIter = m_DelayedSnapshots.erase(--iter.base());

                    //        //std::list<Snapshot*>::iterator tempIter = m_DelayedSnapshots.erase(std::next(iter).base());

                    //        iter = std::list<Snapshot*>::reverse_iterator(tempIter);
                    //    }
                    //    else
                    //    {
                    //        break;
                    //    }
                    //}

                    // We somehow need to check if it's the last anyway because of the alpha?
                    Snapshot* SnapshotToUse = m_DelayedSnapshots.back();

                    // std::cout << "New snapshot put" << std::endl;

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
                    for (size_t i = 0; i < SnapshotToUse->NumOfObjects; i++)
                    {
                        *GetComponent<TransformComponentNext>(SnapshotToUse->Objects[i].EntityID) = TransformComponentNext(SnapshotToUse->Objects[i].Component);
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
                }

                // if(m_DelayedSnapshots.size())
                //{
                //    // Pick the snapshot to update with
                //    Snapshot* SnapshotToUse = m_DelayedSnapshots.back();

                //    uint8_t last = m_NumOfSequencesToInterpolate;
                //    // Get the number of sequences to interpolate, will only be more then 1 if we lost any snapshots
                //    m_NumOfSequencesToInterpolate = sequence_difference(SnapshotToUse->SnapshotSequence, m_snapshotSequenceReal - 1, 255);

                //    if (m_NumOfSequencesToInterpolate > 150)
                //    {
                //        int a = 3;
                //    }

                //    // Update the new Next transform array
                //    // TODOCM we assume the client and server have the same entities
                //    // TODOCM we need in some way be sure that if we remove something we remove it befor adding something, example sending rendudant
                //    // data many frames or something
                //    for(size_t i = 0; i < SnapshotToUse->NumOfObjects; i++)
                //    {
                //        *GetComponent<TransformComponentNext>(SnapshotToUse->Objects[i].EntityID) = TransformComponentNext(SnapshotToUse->Objects[i].Component);
                //    }

                //    m_snapshotSequenceUsed = SnapshotToUse->SnapshotSequence;
                //    m_DelayedSnapshots.pop_back();
                //    delete SnapshotToUse;
                //}
                //else // If we dont have any snapshots we will lagg, this is 2 missed packages for now, ask Christian if this might change
                //{
                //    std::cout << "Lost more then two snapshots?" << std::endl;
                //    m_NumOfSequencesToInterpolate = 1;
                //}
            }
            else
            {
                m_SequenceInterpolationOffset++;
                // std::cout << "Doing a long interpolation..." << std::endl;
            }
            m_snapshotSequenceReal++;
            // std::cout << "NumOfBufferedSnapshots: " << m_DelayedSnapshots.size() << std::endl;
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


        void InterpolationHandler::QueuePlayerPositionForCheck(DirectX::XMFLOAT3 p_position)
        {
            // m_PositionStamps.push_front(PositionStamp(m_snapshotSequenceReal, p_position));
        }

        void InterpolationHandler::CheckPositionFromServer(uint32_t p_playerID, DirectX::XMFLOAT3 p_positionToCheck, uint8_t p_sequenceOfPosition)
        {
            // std::map<uint32_t, Player*>::iterator iter = m_playerMap.find(p_playerID);

            ///**
            // Notes:
            // We need to interpolate the players position to a new position => hence why it should be in Interpolation handler.
            // We need to store the playerID to set the position of the player

            //*/

            //// TODOCM remove this hotfix, and use list of inputs to check against, and set a interpolation point
            //// Check if we find the playerID
            // if (iter != m_playerMap.end())
            //{
            //    // If we have character controller
            //    if (Core::EntityHandler::GetInstance().HasComponents(iter->second->m_playerEntityID, (int)ComponentType::CharacterController))
            //    {
            //        // Set position
            //        m_sharedContext.GetPhysicsModule().GetCharacterControlManager().SetPosition(iter->second->m_playerEntityID, p_positionToCheck);
            //        // cout << "set position" << endl;
            //    }
            //}

            // std::list<PositionStamp>::iterator removeStart = m_PositionStamps.begin();
            // std::list<PositionStamp>::iterator removeEnd;

            // std::list<PositionStamp>::iterator iterPos = m_PositionStamps.begin();

            // while (iterPos != m_PositionStamps.end())
            //{
            //    if (p_sequenceOfPosition == iterPos->Sequence)
            //    {
            //        // If we find the one, we compare positions
            //        if (p_positionToCheck.x == iterPos->Position.x &&
            //            p_positionToCheck.y == iterPos->Position.y &&
            //            p_positionToCheck.z == iterPos->Position.z)
            //        {
            //            // Woo same
            //            cout << "same same!" << endl;
            //        }
            //        else
            //        {
            //            // buu nooo
            //            cout << "No same" << endl;
            //        }
            //    }
            //    else if (sequence_more_recent(p_sequenceOfPosition, iterPos->Sequence, 255))
            //    {
            //        // If we're more recent we move forward
            //    }
            //}
        }
    }
}