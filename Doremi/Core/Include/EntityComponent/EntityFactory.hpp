#pragma once
// Project Specific
#include <Doremi/Core/Include/EntityComponent/Constants.hpp>

// Standard libraries
#include <string>
#include <map>
#include <vector>
#include <DirectXMath.h>

namespace DoremiEngine
{
    namespace Core
    {
        class SharedContext;
    }
}

using namespace std;
namespace Doremi
{
    namespace Core
    {
        class EntityFactory
        {
        public:
            /**
            MAKE SURE that startup has been called BEFORE calling GetInstance*/
            static EntityFactory* GetInstance();

            /**
            Starts the entity factory*/
            static void StartupEntityFactory(const DoremiEngine::Core::SharedContext& p_sharedContext);

            /**
            TODORT docs
            */
            void ReleaseInstance();

            /**
            TODORT docs
            */
            void Initialize();

            /** Registers a set of components as a blueprint to be used when creating entities of that type*/
            void RegisterEntityTemplate(Blueprints p_blueprintID, EntityBlueprint pComponents);

            /** creates an entity according to the provided blueprint id*/
            EntityID CreateEntity(Blueprints p_blueprintID);

            /**
                TODOCM doc
            */
            EntityID CreateEntity(Blueprints p_blueprintID, DirectX::XMFLOAT3 p_position);

            /**
                TODOCM doc
            */
            EntityID CreateEntity(Blueprints p_blueprintID, DirectX::XMFLOAT3 p_position, DirectX::XMFLOAT4 p_orientation);

            /**
            TODOCM doc
            */
            EntityID CreateEntity(Blueprints p_blueprintID, DirectX::XMFLOAT3 p_position, DirectX::XMFLOAT4 p_orientation, DirectX::XMFLOAT3 p_scale);

            /**
            Clears the entity of all values which cannot be ignored (physical bodies mostly).
            WARNING! Do NOT call this method manually! Only to be used internally by entity
            handler*/
            void ScrapEntity(int p_entityID);


        private:
            EntityFactory(const DoremiEngine::Core::SharedContext& p_sharedContext);
            ~EntityFactory();
            static EntityFactory* mSingleton;
            EntityFactory(EntityFactory const&) = delete;
            void operator=(EntityFactory const&) = delete;

            // This creates all the components
            void CreateComponents(EntityID p_entityID, Blueprints p_blueprintID);

            const DoremiEngine::Core::SharedContext& m_sharedContext;

            std::map<Blueprints, EntityBlueprint> mEntityBlueprints;
        };
    }
}