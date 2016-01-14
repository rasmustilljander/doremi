#pragma once
#include <Interface/PotentialField/PotentialFieldActor.hpp>
#include <DirectXMath.h>
#include <vector>
namespace DoremiEngine
{
    namespace AI
    {
        class PotentialFieldActorImpl : public PotentialFieldActor
        {
        public:
            PotentialFieldActorImpl();
            virtual ~PotentialFieldActorImpl();

            void SetPosition(const DirectX::XMFLOAT3& p_position) override;
            void SetCharge(const float& p_charge) override { m_charge = p_charge; };
            void SetRange(const float& p_range) override { m_range = p_range; };
            void SetStatic(const bool& p_static) override { m_static = p_static; };
            void AddOccupiedQuad(const DirectX::XMINT2& p_quad) override;
            const DirectX::XMFLOAT3& GetPosition() const override { return m_position; };
            const float& GetCharge() const override { return m_charge; };
            const float& GetRange() const override { return m_range; };
            const std::vector<DirectX::XMINT2>& GetOccupiedQuads() const override { return m_occupiedQuads; };
            const bool& IsStatic() const override { return m_static; };
            const DirectX::XMINT2& GetClosestOccupied(const DirectX::XMFLOAT3 p_position);

        private:
            std::vector<DirectX::XMINT2> m_occupiedQuads; // TODOKO review if it should be set to enable checking for duplicates
            float m_range;
            float m_charge;
            DirectX::XMFLOAT3 m_position; // TODOKO should maybe not be here, needed in group but not in potential field
            bool m_static;
        };
    }
}
