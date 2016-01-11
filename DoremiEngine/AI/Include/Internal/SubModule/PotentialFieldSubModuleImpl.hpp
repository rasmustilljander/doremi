#pragma once
#include <Interface/SubModule/PotentialFieldSubModule.hpp>

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
                                           const int& p_numberOfQuadsHeight, const DirectX::XMFLOAT2& p_center) override;
            PotentialGroup* CreateNewPotentialGroup() override;
            PotentialFieldActor* CreateNewActor(const DirectX::XMFLOAT3& p_position, const float& p_charge, const float& p_range, const bool& p_static) override;
            void AttachActor(PotentialField& o_field, PotentialFieldActor* p_actor) override;
            int GetNextStep();
        };
    }
}
