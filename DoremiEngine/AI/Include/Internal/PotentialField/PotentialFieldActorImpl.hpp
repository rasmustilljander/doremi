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

            void SetPosition(const DirectX::XMFLOAT3& p_position) override { m_position = p_position; };
            void SetCharge(const float& p_charge) override { m_charge = p_charge; };
            void SetRange(const float& p_range) override { m_range = p_range; };
            void SetStatic(const bool& p_static) override { m_static = p_static; };
            // void AddOccupiedQuad(DirectX::XMFLOAT2 p_occupiedQuad);//nytt TODOEA
            // void RemoveOccupiedQuad(DirectX::XMFLOAT2 p_occupiedQuad);//nytt TODOEA
            const DirectX::XMFLOAT3& GetPosition() const { return m_position; };
            const float& GetCharge() const { return m_charge; };
            const float& GetRange() const { return m_range; };
            const std::vector<DirectX::XMFLOAT2> GetOccupiedQuads() const { return m_occupiedQuads; }; // nytt TODOEA
            private:
            std::vector<DirectX::XMFLOAT2> m_occupiedQuads; // nytt TODOEA //SKa det vara griddet eller positionen. ( måste vara griddet dumt o
            // konverta flera gånger.)
            float m_range;
            float m_charge;
            DirectX::XMFLOAT3 m_position;
            bool m_static;
        };
    }
}
