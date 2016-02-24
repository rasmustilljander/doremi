// Project specific
#include <Manager/AudioManager.hpp>
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Audio/Include/AudioModule.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <EntityComponent/Components/ExampleComponent.hpp>
#include <EntityComponent/Components/Example2Component.hpp>
#include <EntityComponent/Components/AudioActiveComponent.hpp>
#include <EntityComponent/Components/TransformComponent.hpp>
#include <EntityComponent/Components/RigidBodyComponent.hpp>
#include <EventHandler/EventHandler.hpp>
#include <EventHandler/Events/ExampleEvent.hpp>
#include <EventHandler/Events/DamageTakenEvent.hpp>
#include <Doremi/Core/Include/AudioHandler.hpp>
#include <DoremiEngine/Physics/Include/RigidBodyManager.hpp>
#include <Doremi/Core/Include/InputHandlerClient.hpp>
#include <Doremi/Core/Include/PlayerHandler.hpp>
#include <Doremi/Core/Include/EventHandler/Events/PlaySoundEvent.hpp>

#include <DirectXMath.h>
// Third party

// Standard
#include <iostream>
#include <stdint.h>

using namespace std;

namespace Doremi
{
    namespace Core
    {
        AudioManager::AudioManager(const DoremiEngine::Core::SharedContext& p_sharedContext) : Manager(p_sharedContext, "AudioManager")
        {
            EventHandler::GetInstance()->Subscribe(EventType::Example, this);
            EventHandler::GetInstance()->Subscribe(EventType::PlaySound, this);
            EventHandler::GetInstance()->Subscribe(EventType::DamageTaken, this);
            m_gunReloadButtonDown = false;
            m_timeThatGunButtonIsDown = 0;
        }

        AudioManager::~AudioManager() {}


        void AudioManager::Update(double p_dt)
        {
            // First get the module
            DoremiEngine::Audio::AudioModule& t_audioModule = m_sharedContext.GetAudioModule();
            // Loop through all entities
            const size_t length = EntityHandler::GetInstance().GetLastEntityIndex();
            bool t_isPlaying = false;
            // Loop over all enteties to perform various functions on enteties that have sound components
            for(size_t i = 0; i < length; i++)
            {
                // Check if an entity has finished playing its sound. If so remove the component to avoid FMOD "stealing" the channel while entity
                // thinks he stills has his sound on the channel
                if(EntityHandler::GetInstance().HasComponents(i, (int)ComponentType::AudioActive))
                {
                    // Get component
                    AudioActiveComponent* t_audio = EntityHandler::GetInstance().GetComponentFromStorage<AudioActiveComponent>(i);
                    // Looping through the map of soundschannels.
                    // typedef std::map<int, int>::iterator iteratorForLoop;
                    int sizeOfSoundEnumToChannelID = (int)AudioCompEnum::Num_Sounds;
                    int amountOfInactiveChannels = 0;
                    std::vector<int> t_placesToRemove;
                    // for (iteratorForLoop iterator = t_audio->m_soundEnumToChannelID.begin(); iterator != t_audio->m_soundEnumToChannelID.end();
                    // iterator++)
                    for(int k = 0; k < sizeOfSoundEnumToChannelID; ++k)
                    {
                        //++sizeOfSoundEnumToChannelID;
                        if(t_audio->m_soundEnumToChannelID[k] != -1)
                        {
                            t_isPlaying = t_audioModule.GetChannelPlaying(t_audio->m_soundEnumToChannelID[k]);
                            if(!t_isPlaying)
                            {

                                ++amountOfInactiveChannels;
                                t_placesToRemove.push_back(k);
                                // t_audio->m_soundEnumToChannelID.erase(iterator);
                                // EntityHandler::GetInstance().RemoveComponent(i, (int)ComponentType::AudioActive);
                            }
                            else
                            {
                                // Nothing
                            }
                        }
                    }
                    int placeToRemoveSizeForLoop = t_placesToRemove.size();
                    for(size_t k = 0; k < placeToRemoveSizeForLoop; k++)
                    {
                        t_audio->m_soundEnumToChannelID[t_placesToRemove[k]] = -1;
                    }
                    if(sizeOfSoundEnumToChannelID == amountOfInactiveChannels)
                    {

                        EntityHandler::GetInstance().RemoveComponent(i, (int)ComponentType::AudioActive);
                    }
                    else
                    {
                        // Nothing
                    }
                }
                // Check if entity will be affected by the frequencyanalyser.
                if(EntityHandler::GetInstance().HasComponents(i, (int)ComponentType::FrequencyAffected))
                {
                    // RigidBodyComponent* t_rigidComp = EntityHandler::GetInstance().GetComponentFromStorage<RigidBodyComponent>(i);
                    // float t_freq = AudioHandler::GetInstance()->GetFrequency();
                    // m_sharedContext.GetPhysicsModule().GetRigidBodyManager().AddForceToBody(
                    //    t_rigidComp->p_bodyID, XMFLOAT3(0, t_freq * 3, 0)); /**Far from complete TODOLH bör inte liogga i audio manager heller*/
                }
            }
            m_dominantFrequency = AudioHandler::GetInstance()->GetFrequency();
            // m_dominantFrequency = AudioHandler::GetInstance()->GetRepeatableSoundFrequency();
            // std::cout << "Freq = " << m_dominantFrequency << std::endl; /**TODOLH ta bort när debugging är klart*/

            InputHandlerClient* inputHandler = (InputHandlerClient*)PlayerHandler::GetInstance()->GetDefaultInputHandler();

            if(inputHandler != nullptr)
            {
                // Check if player just pressed ä. Then start the process of repeatable audio recording + cutting + analysing + saving to array
                if(inputHandler->CheckForOnePress((int)UserCommandPlaying::StartRepeatableAudioRecording) && !m_gunReloadButtonDown)
                {
                    AudioHandler::GetInstance()->StartRepeatableRecording();
                    m_gunReloadButtonDown = true;
                    m_timeThatGunButtonIsDown = 0;
                }
                // Check if the player is holding down the ä button If so add time to the variable used for cutting soundlength
                else if(inputHandler->CheckBitMaskInputFromGame((int)UserCommandPlaying::StartRepeatableAudioRecording) && m_gunReloadButtonDown)
                {
                    if(m_timeThatGunButtonIsDown < 4.8f)
                    {
                        m_timeThatGunButtonIsDown += p_dt;
                    }
                    else
                    {
                        m_timeThatGunButtonIsDown = 4.8f;
                    }
                }
                // Check if ä button is released to reset the bool and send the time button was down to audio handler.
                else if(!inputHandler->CheckBitMaskInputFromGame((int)UserCommandPlaying::StartRepeatableAudioRecording) && m_gunReloadButtonDown)
                {
                    m_gunReloadButtonDown = false;
                    AudioHandler::GetInstance()->SetGunButtonDownTime(m_timeThatGunButtonIsDown + 0.017);
                }
                // Check if ö button is pressed to play sound
                if(inputHandler->CheckForOnePress((int)UserCommandPlaying::PlayRepeatableAudioRecording))
                {
                    AudioHandler::GetInstance()->PlayRepeatableRecordedSound();
                }
                else
                {
                    // Do Nothing
                }
                AudioHandler::GetInstance()->SetGunButtonDownTime(m_timeThatGunButtonIsDown + 0.017);
            }

            t_audioModule.Update();
        }
        void AudioManager::OnEvent(Event* p_event)
        {
            // Check to see what event was received and do something with it (Might be changed to callback functions instead)
            switch(p_event->eventType)
            {
                case EventType::PlaySound:
                {
                    // Cast the event to the correct format
                    PlaySoundEvent* t_event = (PlaySoundEvent*)p_event;
                    uint32_t t_entityID = t_event->entityID;
                    if(!EntityHandler::GetInstance().HasComponents(t_entityID, (int)ComponentType::Audio))
                    {
                        break;
                    }
                    uint32_t t_soundType = t_event->soundType;
                    if(EntityHandler::GetInstance().HasComponents(t_entityID, (int)ComponentType::AudioActive))
                    {
                        // do nothing
                    }
                    else
                    {
                        EntityHandler::GetInstance().AddComponent(t_entityID, (int)ComponentType::AudioActive);
                    }
                    int;
                    AudioActiveComponent* audioActiveComp = EntityHandler::GetInstance().GetComponentFromStorage<AudioActiveComponent>(t_entityID);
                    AudioComponent* audioComp = EntityHandler::GetInstance().GetComponentFromStorage<AudioComponent>(t_entityID);

                    m_sharedContext.GetAudioModule().PlayASound(audioComp->m_enumToSoundID[t_soundType], false,
                                                                audioActiveComp->m_soundEnumToChannelID[t_soundType]);
                    m_sharedContext.GetAudioModule().SetVolumeOnChannel(audioActiveComp->m_soundEnumToChannelID[t_soundType], 1.0f);


                    // EntityHandler::GetInstance().GetComponentFromStorage<AudioActiveComponent>(t_entityID)->m_soundEnumToChannelID =
                    // m_sharedContext.GetAudioModule().PlayASound(t_soundType);
                    break;
                }
                case EventType::DamageTaken:
                {
                    // A bit stupid... we send a event to our own class to play a sound when we take damage...
                    DamageTakenEvent* t_event = static_cast<DamageTakenEvent*>(p_event);
                    PlaySoundEvent* t_soundEvent = new PlaySoundEvent(t_event->entityId, (int32_t)AudioCompEnum::DamageTaken);
                    EventHandler::GetInstance()->BroadcastEvent(t_soundEvent);
                    break;
                }
            }
        }
    }
}
