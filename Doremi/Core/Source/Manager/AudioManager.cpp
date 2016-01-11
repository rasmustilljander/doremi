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
#include <Doremi/Core/Include/AudioHandler.hpp>
#include <DoremiEngine/Physics/Include/RigidBodyManager.hpp>
#include <Doremi/Core/Include/InputHandlerClient.hpp>
#include <Doremi/Core/Include/PlayerHandler.hpp>

#include <DirectXMath.h>
// Third party

// Standard
#include <iostream>

using namespace std;

namespace Doremi
{
    namespace Core
    {
        AudioManager::AudioManager(const DoremiEngine::Core::SharedContext& p_sharedContext) : Manager(p_sharedContext)
        {
            EventHandler::GetInstance()->Subscribe(EventType::Example, this);
        }

        AudioManager::~AudioManager() {}


        void AudioManager::Update(double p_dt)
        {
            // First get the module
            DoremiEngine::Audio::AudioModule& t_audioModule = m_sharedContext.GetAudioModule();
            // Loop through all entities
            size_t length = EntityHandler::GetInstance().GetLastEntityIndex();
            bool t_isPlaying = false;
            for(size_t i = 0; i < length; i++)
            {
                // Check that the current entity has the relevant components
                if(EntityHandler::GetInstance().HasComponents(i, (int)ComponentType::AudioActive))
                {
                    // Get component
                    AudioActiveComponent* t_audio = EntityHandler::GetInstance().GetComponentFromStorage<AudioActiveComponent>(i);
                    t_isPlaying = t_audioModule.GetChannelPlaying(t_audio->channelID);
                    if(!t_isPlaying)
                    {
                        EntityHandler::GetInstance().RemoveComponent(i, (int)ComponentType::AudioActive);
                    }

                    /**
                    TODOLH Test the functions below
                    */
                    /*if (EntityHandler::GetInstance().HasComponents(i, (int)ComponentType::Transform) && t_isPlaying)
                    {
                        TransformComponent* t_trans = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(i);
                        t_audioModule.SetSoundPositionAndVelocity(t_trans->position.x, t_trans->position.y, t_trans->position.z, 0, 0, 0, t_audio->channelID);
                    }*/
                    /*if (EntityHandler::GetInstance().HasComponents(i, (int)ComponentType::Player) && t_isPlaying)
                    {
                        TransformComponent* t_trans = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(i);
                        t_audioModule.SetListenerPos(t_trans->position.x, t_trans->position.y, t_trans->position.z)
                    }*/
                }
                if (EntityHandler::GetInstance().HasComponents(i, (int)ComponentType::FrequencyAffected))
                {
                    RigidBodyComponent* t_rigidComp = EntityHandler::GetInstance().GetComponentFromStorage<RigidBodyComponent>(i);
                    float t_freq = AudioHandler::GetInstance()->GetFrequency();
                    m_sharedContext.GetPhysicsModule().GetRigidBodyManager().AddForceToBody(
                        t_rigidComp->p_bodyID, XMFLOAT3(0, t_freq * 3, 0)); /**Far from complete TODOLH bör inte liogga i audio manager heller*/
                }
            }
            // m_dominantFrequency = AudioHandler::GetInstance()->GetFrequency();
            m_dominantFrequency = AudioHandler::GetInstance()->GetRepeatableSoundFrequency();
            // std::cout << "Freq = " << m_dominantFrequency << std::endl; /**TODOLH ta bort när debugging är klart*/

            InputHandlerClient* inputHandler = (InputHandlerClient*)PlayerHandler::GetInstance()->GetDefaultInputHandler();

            if(inputHandler != nullptr)
            {
                // Check Input
                if(inputHandler->CheckForOnePress((int)UserCommandPlaying::StartRepeatableAudioRecording) && !m_gunReloadButtonDown)
                {
                    AudioHandler::GetInstance()->StartRepeatableRecording();
                    m_gunReloadButtonDown = true;
                    m_timeThatGunButtonIsDown = 0;
                }
                else if(inputHandler->CheckBitMaskInputFromGame((int)UserCommandPlaying::StartRepeatableAudioRecording) && m_gunReloadButtonDown)
                {
                    m_timeThatGunButtonIsDown += p_dt;
                }
                else if(!inputHandler->CheckBitMaskInputFromGame((int)UserCommandPlaying::StartRepeatableAudioRecording) && m_gunReloadButtonDown)
                {
                    m_gunReloadButtonDown = false;
                    AudioHandler::GetInstance()->SetGunButtonDownTime(m_timeThatGunButtonIsDown);
                }
                if(inputHandler->CheckForOnePress((int)UserCommandPlaying::PlayRepeatableAudioRecording))
                {
                    AudioHandler::GetInstance()->PlayRepeatableRecordedSound();
                }
                else
                {
                    // Do Nothing
                }
            }


            t_audioModule.Update();
        }
        void AudioManager::OnEvent(Event* p_event)
        {
            // Check to see what event was received and do something with it (Might be changed to callback functions instead)
            switch(p_event->eventType)
            {
                case EventType::Example:
                    // Cast the event to the correct format
                    ExampleEvent* t_event = (ExampleEvent*)p_event;
                    int t_intFromEvent = t_event->myInt;
                    break;
            }
        }
    }
}
