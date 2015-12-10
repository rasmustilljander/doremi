// Project specific
#include <Manager/AudioManager.hpp>
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Audio/Include/AudioModule.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <EntityComponent/Components/ExampleComponent.hpp>
#include <EntityComponent/Components/Example2Component.hpp>
#include <EntityComponent/Components/AudioActiveComponent.hpp>
#include <EntityComponent/Components/VoiceRecordingComponent.hpp>
#include <EventHandler/EventHandler.hpp>
#include <EventHandler/Events/ExampleEvent.hpp>

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
            EventHandler::GetInstance()->Subscribe(Events::Example, this);
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
                }
                if(EntityHandler::GetInstance().HasComponents(i, (int)ComponentType::VoiceRecording))
                {
                    VoiceRecordingComponent* t_voice = EntityHandler::GetInstance().GetComponentFromStorage<VoiceRecordingComponent>(i);
                    if(t_voice->active)
                    {
                        m_dominantFrequency = t_audioModule.AnalyseSoundSpectrum(t_voice->channelID);
                    }
                    else
                    {
                        unsigned int recordPointer = t_audioModule.GetRecordPointer();
                        if(recordPointer > 9600)
                        {
                            t_voice->active = true;
                            t_audioModule.PlayASound(t_voice->soundID, t_voice->loop, t_voice->channelID);
                        }
                    }
                }
            }
            // std::cout << "Freq = " << m_dominantFrequency << std::endl; /**TODOLH ta bort när debugging är klart*/
            t_audioModule.Update();
        }
        void AudioManager::OnEvent(Event* p_event)
        {
            // Check to see what event was received and do something with it (Might be changed to callback functions instead)
            switch(p_event->eventType)
            {
                case Events::Example:
                    // Cast the event to the correct format
                    ExampleEvent* t_event = (ExampleEvent*)p_event;
                    int t_intFromEvent = t_event->myInt;
                    break;
            }
        }
    }
}
