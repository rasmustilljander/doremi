#pragma once
#include <Doremi/Core/Include/LevelLoader.hpp>

namespace Doremi
{
    namespace Core
    {
        class LevelLoaderServer : public LevelLoader
        {
        public:
            LevelLoaderServer(const DoremiEngine::Core::SharedContext& p_sharedContext);

            virtual ~LevelLoaderServer();

            void LoadLevel(const std::string& p_fileName);

            void LoadCharacter(const std::string& p_fileName);

        protected:
            void BuildComponents(int p_entityId, int p_meshCouplingID, std::vector<DoremiEngine::Graphic::Vertex>& p_vertexBuffer) override;

        private:
            void CreatePotentialfieldAroundMesh(const std::vector<DoremiEngine::Graphic::Vertex>& p_vertexBuffer,
                                                const DoremiEditor::Core::TransformData& p_transformationData);
        };
    }
}