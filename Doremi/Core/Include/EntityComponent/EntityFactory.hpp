#pragma once
// Project Specific
#include <Doremi/Core/Include/EntityComponent/Constants.hpp>

// Standard libraries
#include <string>
#include <map>
#include <vector>
#include <DirectXMath.h>

using namespace std;
namespace Doremi
{
    namespace Core
    {
        class EntityFactory
        {
        public:
            /**
            TODORT docs
            */
            static EntityFactory* GetInstance();

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


        private:
            EntityFactory();
            ~EntityFactory();
            static EntityFactory* mSingleton;
            EntityFactory(EntityFactory const&) = delete;
            void operator=(EntityFactory const&) = delete;

            std::map<Blueprints, EntityBlueprint> mEntityBlueprints;
        };
    }
}