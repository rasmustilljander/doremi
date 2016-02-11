#pragma once
#include <Interface/SubModule/PotentialFieldSubModule.hpp>
#include <vector>
namespace DoremiEngine
{
    namespace AI
    {
        class PotentialFieldSubModuleImpl : public PotentialFieldSubModule
        {
        public:
            PotentialFieldSubModuleImpl();
            virtual ~PotentialFieldSubModuleImpl();
            PotentialField* CreateNewField(const float& p_width, const float& p_height, const int& p_numberOfQuadsWidth,
                                           const int& p_numberOfQuadsHeight, const DirectX::XMFLOAT3& p_center) override;
            PotentialGroup* CreateNewPotentialGroup() override;
            PotentialFieldActor* CreateNewActor(const DirectX::XMFLOAT3& p_position, const float& p_charge, const float& p_range,
                                                const bool& p_static, const AIActorType& p_actorType) override;
            void AttachActor(PotentialField& o_field, PotentialFieldActor* p_actor) override;
            PotentialField* FindBestPotentialField(const DirectX::XMFLOAT3& p_position) override;
            void AddActorToEveryPotentialField(PotentialFieldActor* p_actor) override;

        private:
            std::vector<PotentialField*> m_fields;
        };
    }
}
