#include <InterpolationHandler.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <EntityComponent/Components/TransformComponent.hpp>
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

        InterpolationHandler::InterpolationHandler() : m_snapshotSequenceReal(0), m_snapshotDelay(4) {}

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
                std::cout << "SOMETHING WRONG: " << m_NumOfSequencesToInterpolate << std::endl;
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

        uint8_t sequence_difference(uint8_t s1, uint8_t s2, unsigned int max)
        {
            // If s1 is larger and the difference is less then half
            // meaning if they're not wrapped, the differnce should be less then half for it to be more recent
            // or
            //  If s2 is the larger one, and their difference is larger then half
            // meaning if they're wrapped, the difference should be more then half for it to be wrapped
            if(s1 > s2)
            {
                return s1 - s2;
            }
            else if(s2 > s1) // TODOCM check if this really is right
            {
                return max + 1 - s2 + s1;
            }
            else
            {
                std::cout << "Snapshot sequence is same, something is wrong" << std::endl; // TODOCM remove maybe
                return 1;
            }
        }

        /**
        TODOCM make sure to give cred for this func! and other stuff ofc
        http://gafferongames.com/networking-for-game-programmers/reliability-and-flow-control/
        */
        bool sequence_more_recent(uint8_t s1, uint8_t s2, uint32_t max)
        {
            // If s1 is larger and the difference is less then half
            // meaning if they're not wrapped, the differnce should be less then half for it to be more recent
            // or
            //  If s2 is the larger one, and their difference is larger then half
            // meaning if they're wrapped, the difference should be more then half for it to be wrapped

            return (s1 > s2) && (s1 - s2 <= max / 2) || (s2 > s1) && (s2 - s1 > max / 2);
        }

        void InterpolationHandler::UpdateInterpolationTransforms()
        {
            // If the snapshot we're currently interpolating towards is more recent
            // then the one that we should be interpolating towards, we skip
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
                    if (m_DelayedSnapshots.size() == 1)
                    {
                        int a = 3;
                    }

                    // Loop to the last item (excluding it in the loop because we want to use it if it's the oldest
                    for (iter = m_DelayedSnapshots.rbegin(); iter != iterEnd; ++iter)
                    {
                        if (sequence_more_recent((*iter)->SnapshotSequence, m_snapshotSequenceReal - 1, 255))
                        {
                            // If we find somone that is the same or infront of us
                            break;
                        }
                        else if(m_DelayedSnapshots.size() > 1)
                        {
                            // If older, we remove it
                            std::list<Snapshot*>::iterator tempIter = m_DelayedSnapshots.erase(--iter.base());
                            iter = std::list<Snapshot*>::reverse_iterator(tempIter);
                        }
                        else
                        {
                            break;
                        }
                    }

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
                    
                    if (m_NumOfSequencesToInterpolate > 150)
                    {
                        int a = 3;
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
                    std::cout << "Lost more then two snapshots?" << std::endl;
                    m_NumOfSequencesToInterpolate = 1;
                }

                //if(m_DelayedSnapshots.size())
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
                std::cout << "Doing a long interpolation..." << std::endl;
            }
            m_snapshotSequenceReal++;
            std::cout << "NumOfBufferedSnapshots: " << m_DelayedSnapshots.size() << std::endl;
        }

        void InterpolationHandler::SetSequence(uint8_t p_sequence)
        {
            std::cout << "Setting new snapshotsequence" << std::endl;
            m_snapshotSequenceReal = p_sequence - m_snapshotDelay;
            m_snapshotSequenceUsed = p_sequence - m_snapshotDelay;
        }


        void InterpolationHandler::QueueSnapshot(Snapshot* p_newSnapshot)
        {
            // If we get n' old snapshot
            if(sequence_more_recent(m_snapshotSequenceUsed, p_newSnapshot->SnapshotSequence, 255)) // TODOCM check if it should be equal less
            {
                std::cout << "Throwing snapshot" << std::endl;
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
    }
}