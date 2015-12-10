#pragma once

namespace DirectX // TODO review if we should forward declare or not
{
    struct XMFLOAT4X4;
    struct XMFLOAT3;
}
namespace DoremiEngine
{
    namespace Graphic
    {
        enum class CameraType
        {
            FREELOOK,
            THIRDPERSON
        };
        class CameraSystem
        {
            public:
            CameraSystem();
            virtual ~CameraSystem();

            void SetViewMatrix(const DirectX::XMFLOAT3& p_position, const DirectX::XMFLOAT3& p_direction,
                               const DirectX::XMFLOAT3& p_up); // TODO refactor to use correct math lib
            void SetFieldOfView(float p_angle);

            private:
            void UpdateProjectionMatrix();

            size_t m_screenWidth;
            size_t m_screenHeight;
            float m_angle;
            size_t m_viewDistance;

            DirectX::XMFLOAT4X4* m_projectionMatrix;
            DirectX::XMFLOAT4X4* m_viewMatrix;
        };
    }
}
