#pragma once
#include <stdint.h>
#include <string>

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
        class Streamer
        {
        public:
            /**
                Write 8-bit integer
            */
            virtual bool WriteInt8(int8_t p_Value) = 0;

            /**
                Read 8-bit integer
            */
            virtual int8_t ReadInt8() = 0;

            /**
                Write 16-bit integer
            */
            virtual bool WriteInt16(int16_t p_Value) = 0;

            /**
                Read 16-bit integer
            */
            virtual int16_t ReadInt16() = 0;

            /**
                Write 32-bit integer
            */
            virtual bool WriteInt32(int32_t p_Value) = 0;

            /**
                Read 32-bit integer
            */
            virtual int32_t ReadInt32() = 0;

            /**
                Write 64-bit integer
            */
            virtual bool WriteInt64(int64_t p_Value) = 0;

            /**
                Read 64-bit integer
            */
            virtual int64_t ReadInt64() = 0;

            /**
                Write unsigned 8-bit integer
            */
            virtual bool WriteUnsignedInt8(uint8_t p_Value) = 0;

            /**
                Read unsigned 8-bit integer
            */
            virtual uint8_t ReadUnsignedInt8() = 0;

            /**
                Write unsigned 16-bit integer
            */
            virtual bool WriteUnsignedInt16(uint16_t p_Value) = 0;

            /**
                Read unsigned 16-bit integer
            */
            virtual uint16_t ReadUnsignedInt16() = 0;

            /**
                Write unsigned 32-bit integer
            */
            virtual bool WriteUnsignedInt32(uint32_t p_Value) = 0;

            /**
                Read unsigned 32-bit integer
            */
            virtual uint32_t ReadUnsignedInt32() = 0;

            /**
                Write unsigned 64-bit integer
            */
            virtual bool WriteUnsignedInt64(uint64_t p_Value) = 0;

            /**
             Read unsigned 64-bit integer
            */
            virtual uint64_t ReadUnsignedInt64() = 0;

            /**
                Write float
            */
            virtual bool WriteFloat(float p_Value) = 0;

            /**
                Read float
            */
            virtual float ReadFloat() = 0;

            /**
                Write high precision float
            */
            virtual bool WriteDouble(double p_Value) = 0;

            /**
                Read high precision float
            */
            virtual double ReadDouble() = 0;

            /**
                Write bool (one bit)
            */
            virtual bool WriteBool(bool p_Value) = 0;

            /**
                Read bool (one bit)
            */
            virtual bool ReadBool() = 0;

            /**
                Write DirectX float2
            */
            virtual bool WriteFloat2(DirectX::XMFLOAT2 p_Value) = 0;

            /**
                Read DirectX float2
            */
            virtual DirectX::XMFLOAT2 ReadFloat2() = 0;

            /**
             Write DirectX float3
            */
            virtual bool WriteFloat3(DirectX::XMFLOAT3 p_Value) = 0;

            /**
                Read DirectX float3
            */
            virtual DirectX::XMFLOAT3 ReadFloat3() = 0;

            /**
                Write DirectX float4
            */
            virtual bool WriteFloat4(DirectX::XMFLOAT4 p_Value) = 0;

            /**
                Read DirectX float4
            */
            virtual DirectX::XMFLOAT4 ReadFloat4() = 0;

            /**
                Write DirectX quaternion, packed into 3 values and 2 bit
                TODOCM fix rotation quaternion packing writing
            */
            virtual bool WriteRotationQuaternion(DirectX::XMFLOAT4 p_Value) = 0;

            /**
                Read DirectX quaternion, packed into 3 values and 2 bit
                TODOCM fix rotation quaternion packing reading
            */
            virtual DirectX::XMFLOAT4 ReadRotationQuaternion() = 0;

            /**
                Write string
            */
            virtual bool WriteString(std::string p_Value) = 0;

            /**
                Read string
            */
            virtual std::string ReadString() = 0;

            /**
                Write buffer byte for byte
            */
            virtual bool WriteBuffer(void* p_buffer, uint32_t p_bufferSize) = 0;

            /**
                Read buffer byte for byte
            */
            virtual void ReadBuffer(void* p_buffer, uint32_t p_bufferSize) = 0;

            /**
             Write buffer with memcpy, should be fastest
            */
            virtual void WriteMemcpy(void* p_buffer, uint32_t p_bufferSize) = 0;

            /**
                Read buffer with memcpy, should be fastest
            */
            virtual void ReadMemcpy(void* p_buffer, uint32_t p_bufferSize) = 0;
        };
    }
}