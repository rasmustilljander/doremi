#pragma once
#include <Doremi/Core/Include/LevelLoader.hpp>
#include <DirectXMath.h>
#include <map>

namespace DoremiEngine
{
    namespace Graphic
    {
        class SkeletalInformation;
        struct SkeletalVertex;
        struct AnimationClip;
    }
}

namespace Doremi
{
    namespace Core
    {
        class LevelLoaderClient : public LevelLoader
        {
        public:
            LevelLoaderClient(const DoremiEngine::Core::SharedContext& p_sharedContext);

            virtual ~LevelLoaderClient();

            void LoadLevel(const std::string& p_fileName);

            CharacterDataNames LoadCharacter(const std::string& p_fileName);

            CharacterDataNames LoadSkeletalCharacter(const std::string& p_fileName, DoremiEngine::Graphic::SkeletalInformation& p_upperBodySkeletalInformation,
                                                     DoremiEngine::Graphic::SkeletalInformation& p_lowerBodySkeletalInformation);

        protected:
            bool BuildComponents(int p_entityId, int p_meshCouplingID, std::vector<DoremiEngine::Graphic::Vertex>& p_vertexBuffer) override;
            void BuildLights();
            void LoadFileInternal(const std::string& p_fileName);

        private:
            struct TransformInformation
            {
                DirectX::XMFLOAT3 position;
                DirectX::XMFLOAT4 quaternion;
                DirectX::XMFLOAT3 euler;
                DirectX::XMFLOAT3 scale;
            };
            struct AnimationInformation
            {
                std::string name;
                int startFrame;
                int endFrame;
                int prioPart;
                float maxTime;
                bool loop;
            };
            std::vector<DoremiEngine::Graphic::SkeletalVertex>
            LoadSkeletalMesh(std::ifstream& ifs, const int& p_nrOfMeshes, const std::map<std::string, DirectX::XMFLOAT4X4>& p_transformMap);
            std::map<std::string, DirectX::XMFLOAT4X4> LoadSkeletalTransforms(std::ifstream& ifs, const int& p_nrOfTransforms);
            void LoadJointAnimations(std::ifstream& ifs, const int& p_nrOfJoints, const int& p_nrOfAnimations,
                                     std::vector<DoremiEngine::Graphic::AnimationClip>& o_upperBodyAnimations,
                                     std::vector<DoremiEngine::Graphic::AnimationClip>& o_lowerBodyAnimations,
                                     std::vector<AnimationInformation> p_animationInformations, std::vector<int>& o_lowerBodyJointHeirarchy,
                                     std::vector<int>& o_upperBodyJointHeirarchy, std::vector<std::string>& o_animationNames);
            std::vector<AnimationInformation> LoadAnimationInformation(std::ifstream& ifs, const int& p_nrOfAnimationInformations);
        };
    }
}