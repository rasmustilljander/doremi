#pragma once
#include <string>
#include <cstdint>


namespace DirectX
{
    struct XMFLOAT2;
    struct XMFLOAT3;
    struct XMFLOAT4;
}

namespace Doremi
{
    namespace Core
    {
        class BitStreamer
        {
        public:
            /**
                TODOCM doc
            */
            BitStreamer();
            /**
                TODOCM doc
            */
            ~BitStreamer();

            /**
                Load a buffer into the streamer, makes it aviable to use
            */
            void SetTargetBuffer(unsigned char*& p_buffer, uint32_t p_bufferSize);

            /**
                TODOCM doc
            */
            void SetReadWritePosition(uint32_t p_bytePosition);


            /**
                Write 8-bit integer
            */
            bool WriteInt8(int8_t p_Value);

            /**
                 Read 8-bit integer
            */
            int8_t ReadInt8();

            /**
                 Write 16-bit integer
            */
            bool WriteInt16(int16_t p_Value);

            /**
                 Read 16-bit integer
            */
            int16_t ReadInt16();

            /**
                 Write 32-bit integer
            */
            bool WriteInt32(int32_t p_Value);

            /**
                 Read 32-bit integer
            */
            int32_t ReadInt32();

            /**
                 Write 64-bit integer
            */
            bool WriteInt64(int64_t p_Value);

            /**
                 Read 64-bit integer
            */
            int64_t ReadInt64();

            /**
                 Write unsigned 8-bit integer
            */
            bool WriteUnsignedInt8(uint8_t p_Value);

            /**
                 Read unsigned 8-bit integer
            */
            uint8_t ReadUnsignedInt8();

            /**
                 Write unsigned 16-bit integer
            */
            bool WriteUnsignedInt16(uint16_t p_Value);

            /**
                 Read unsigned 16-bit integer
            */
            uint16_t ReadUnsignedInt16();

            /**
                 Write unsigned 32-bit integer
            */
            bool WriteUnsignedInt32(uint32_t p_Value);

            /**
                 Read unsigned 32-bit integer
            */
            uint32_t ReadUnsignedInt32();

            /**
                 Write unsigned 64-bit integer
            */
            bool WriteUnsignedInt64(uint64_t p_Value);

            /**
                 Read unsigned 64-bit integer
            */
            uint64_t ReadUnsignedInt64();

            /**
                 Write float
            */
            bool WriteFloat(float p_Value);

            /**
                Read float
            */
            float ReadFloat();

            /**
                Write high precision float
            */
            bool WriteDouble(double p_Value);

            /**
                Read high precision float
            */
            double ReadDouble();

            /**
                Write bool (one bit)
            */
            bool WriteBool(bool p_Value);

            /**
                Read bool (one bit)
            */
            bool ReadBool();

            /**
                Write DirectX float2
            */
            bool WriteFloat2(DirectX::XMFLOAT2 p_Value);

            /**
                Read DirectX float2
            */
            DirectX::XMFLOAT2 ReadFloat2();

            /**
                Write DirectX float3
            */
            bool WriteFloat3(DirectX::XMFLOAT3 p_Value);

            /**
                Read DirectX float3
            */
            DirectX::XMFLOAT3 ReadFloat3();

            /**
                Write DirectX float4
            */
            bool WriteFloat4(DirectX::XMFLOAT4 p_Value);

            /**
                Read DirectX float4
            */
            DirectX::XMFLOAT4 ReadFloat4();

            /**
                Write DirectX quaternion, packed into 3 values and 2 bit
                TODOCM fix rotation quaternion packing writing
            */
            bool WriteRotationQuaternion(DirectX::XMFLOAT4 p_Value);

            /**
                Read DirectX quaternion, packed into 3 values and 2 bit
                TODOCM fix rotation quaternion packing reading
            */
            DirectX::XMFLOAT4 ReadRotationQuaternion();

            /**
                Write string
            */
            bool WriteString(std::string p_Value);

            /**
                Read string
            */
            std::string ReadString();

            /**
                Write buffer byte for byte
            */
            bool WriteBuffer(void* p_buffer, uint32_t p_bufferSize);

            /**
                Read buffer byte for byte
            */
            void ReadBuffer(void* p_buffer, uint32_t p_bufferSize);

            /**
                Write buffer with memcpy, should be fastest
            */
            void WriteMemcpy(void* p_buffer, uint32_t p_bufferSize);

            /**
                Read buffer with memcpy, should be fastest
            */
            void ReadMemcpy(void* p_buffer, uint32_t p_bufferSize);
           
        private:
            /**
                Write bits to stored buffer
            */
            uint32_t WriteBits(void* p_buffer, uint32_t p_numberOfBitsToWrite);
            /**
                Read bits from stored buffer
            */
            uint32_t ReadBits(void* p_buffer, uint32_t p_numberOfBitsToRead);

            /** 
                Saved buffer
            */
            unsigned char * m_buffer;

            /**
                Size of buffer in bytes
            */
            uint32_t m_bufferSize;

            /**
                bool for writing mode
            */
            bool m_IsWriting;

            /**
                bool for reading mode
            */
            bool m_IsReading;

            /**
                Number of bits written on current byte
            */
            uint32_t m_writtenBits;

            /**
                Number of bits read on current byte
            */
            uint32_t m_readBits;

            /**
                Next byte to read/write
            */
            uint32_t m_currentByte;
        };
    }
}