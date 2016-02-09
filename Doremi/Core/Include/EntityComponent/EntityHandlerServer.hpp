#pragma once
#include <Doremi/Core/Include/EntityComponent/EntityHandler.hpp>


namespace Doremi
{
    namespace Core
    {
        struct Event;
        /**
        TODOCM doc
        */
        class EntityHandlerServer : public EntityHandler
        {
        public:
            /**
                TODOCM doc
            */
            static void StartupEntityHandlerServer();

            /**
                Creates an entity according to the provided blueprint id
                Also returns EntityID of newly created entity
            */
            int CreateEntity(Blueprints p_blueprintID) override;

            /**
                Creates an entity according to the provided blueprint id
                Also returns EntityID of newly created entity
            */
            int CreateEntity(Blueprints p_blueprintID, DirectX::XMFLOAT3 p_position) override;

            /**
                Creates an entity according to the provided blueprint id
                Also returns EntityID of newly created entity
            */
            int CreateEntity(Blueprints p_blueprintID, DirectX::XMFLOAT3 p_position, DirectX::XMFLOAT4 p_orientation) override;

            /**
                Removes the entire entity at the specific location
            */
            void RemoveEntity(int p_entityID) override;


        private:
            /**
                TODOCM doc
            */
            EntityHandlerServer();

            /**
                TODOCM doc
            */
            ~EntityHandlerServer();
        };
    }
}