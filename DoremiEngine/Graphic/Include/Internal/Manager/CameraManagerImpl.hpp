#pragma once
#include <Interface/Manager/CameraManager.hpp>
#include <string>
struct ID3D11Buffer;
namespace DoremiEngine
{
    namespace Graphic
    {
        struct GraphicModuleContext;
        class CameraManagerImpl : public CameraManager
        {
            public:
            // TODOKO add documents
            CameraManagerImpl(const GraphicModuleContext& p_graphicContext);
            virtual ~CameraManagerImpl();
            void Initialize();
            // Creates a new camera at origo with direction in z=1 with the specified projectionmatrix
            Camera* BuildNewCamera(DirectX::XMFLOAT4X4& p_projectionMatrix) override;
            void PushCameraToDevice(const Camera& p_camera) override;

            private:
            const GraphicModuleContext& m_graphicContext;
            ID3D11Buffer* m_cameraBuffer;
        };
    }
}