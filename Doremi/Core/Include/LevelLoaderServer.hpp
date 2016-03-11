#pragma once
#include <Doremi/Core/Include/LevelLoader.hpp>
#include <DirectXMath.h>

namespace Doremi
{
    namespace Core
    {
        class LevelLoaderServer : public LevelLoader
        {
        public:
            explicit LevelLoaderServer(const DoremiEngine::Core::SharedContext& p_sharedContext);

            virtual ~LevelLoaderServer();

            void LoadLevel(const std::string& p_fileName);

            void LoadCharacter(const std::string& p_fileName);

        protected:
            /**
                Returns if we should build physics or not
            */
            bool BuildComponents(int p_entityId, int p_meshCouplingID, std::vector<DoremiEngine::Graphic::Vertex>& p_vertexBuffer) override;


        private:
        };
    }
}