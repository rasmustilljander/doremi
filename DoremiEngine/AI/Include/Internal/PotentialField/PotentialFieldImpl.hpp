#include <Interface/PotentialField/PotentialField.hpp>
#include <Internal/PotentialField/PotentialFieldActorImpl.hpp>
#include <Internal/AIContext.hpp>

#include <set>
namespace DoremiEngine
{
    namespace AI
    {
        class PotentialFieldImpl : public PotentialField
        {
        public:
            PotentialFieldImpl(AIContext& p_aiContext);
            virtual ~PotentialFieldImpl();
            void SetGrid(PotentialFieldGridPoint* p_grid) override;
            void SetHeight(const float& p_height) override { m_height = p_height; };
            void SetWidth(const float& p_width) override { m_width = p_width; };
            void SetCenter(const DirectX::XMFLOAT3& p_center) override { m_center = p_center; };
            void SetQuadSize(const DirectX::XMFLOAT2& p_quadSize) override { m_quadSize = p_quadSize; };
            void SetNumberOfQuads(const int& p_numberOfQuadsWidth, const int& p_numberOfQuadsHeight) override
            {
                m_numberOfQuadsHeight = p_numberOfQuadsHeight;
                m_numberOfQuadsWidth = p_numberOfQuadsWidth;
            };
            const DirectX::XMFLOAT3& GetCenter() const override { return m_center; };
            const DirectX::XMFLOAT2& GetQuadSize() const override { return m_quadSize; };
            const PotentialFieldGridPoint* GetGrid() const override { return m_grid; }
            void Update() override;
            void AddActor(PotentialFieldActor* p_newActor) override;
            void RemoveActor(PotentialFieldActor* p_newActor) override;
            DirectX::XMINT2 WhatGridPosAmIOn(const DirectX::XMFLOAT3& p_unitPosition);
            DirectX::XMFLOAT2 GetAttractionPosition(const DirectX::XMFLOAT3& p_unitPosition, bool& p_inField,
                                                    PotentialFieldActor* p_currentActor = nullptr, const bool& p_staticCheck = true) override;
            DirectX::XMFLOAT2 GetGridQuadPosition(const int& p_x, const int& p_z) override;

            int GetNumberOfQuadsWidth() override { return m_numberOfQuadsWidth; };
            int GetNumberOfQuadsHeight() override { return m_numberOfQuadsHeight; };

            // Not in interface
            float CalculateCharge(int p_quadX, int p_quadY, const PotentialFieldActor* p_currentActor);

        private:
            // Help functions

            bool AnyPositiveGoalInRange(const DirectX::XMFLOAT3& p_position);
            float GetChargeInfluenceFromActor(const DirectX::XMFLOAT2& p_position, const PotentialFieldActor& p_actor);
            float GetSpecialInfluenceBetweenActors(const DirectX::XMFLOAT2& p_position, const PotentialFieldActor& p_actorToCheck,
                                                   const PotentialFieldActor& p_yourActor, bool& o_phermoneActive);
            PotentialFieldGridPoint* m_grid; // [width][height]
            std::set<PotentialFieldActor*> m_staticActors; // set for fast check if actor already recides in list
            std::vector<PotentialFieldActor*> m_dynamicActors; // vector for fast access through the list
            float m_width;
            float m_height;
            DirectX::XMFLOAT2 m_quadSize;
            DirectX::XMFLOAT3 m_center;
            int m_numberOfQuadsWidth; // x
            int m_numberOfQuadsHeight; // z
            float m_phermoneEffect;
            AIContext& m_context;
            float m_jumpDistance;
        };
    }
}