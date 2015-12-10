#pragma once
class ID3D11Buffer;
namespace DoremiEngine
{
    namespace Graphic
    {
        struct MeshInfo
        {
            public:
            MeshInfo(){};
            ~MeshInfo(){};

            int m_vertexCount;
            ID3D11Buffer* m_bufferHandle;
        };
    }
}
