// Project specific
#include <Doremi/Core/Include/Streamers/NetworkStreamer.hpp>
#include <Windows.h>

// Third party
#include <DirectXMath.h>

// Standard library
#include <iostream>


namespace Doremi
{
    namespace Core
    {
        NetworkStreamer::NetworkStreamer()
        {
            m_IsReading = true;
            m_IsWriting = true;
        }

        NetworkStreamer::~NetworkStreamer() {}

        void NetworkStreamer::SetTargetBuffer(unsigned char*& p_buffer, uint32_t p_bufferSize)
        {
            // Save buffer and size
            m_buffer = p_buffer;
            m_bufferSize = p_bufferSize;

            // Reset variables
            m_writtenBits = 0;
            m_currentByte = 0;
            m_readBits = 0;

            // Set mode to none
            m_IsReading = false;
            m_IsWriting = false;
        }

        void NetworkStreamer::SetReadWritePosition(uint32_t p_bytePosition)
        {
            m_writtenBits = 0;

            m_currentByte = min(m_bufferSize, p_bytePosition);
            m_readBits = 0;
        }

        uint32_t NetworkStreamer::WriteBits(void* p_buffer, uint32_t p_numberOfBitsToWrite)
        {
            // If we already entered another mode
            if(m_IsReading)
            {
                return 0;
            }
            m_IsWriting = true;

            // Check if enough memory left on message
            if(!m_writtenBits)
            {
                // If we have 0 bits, we will initialize a new byte
                if(p_numberOfBitsToWrite > m_bufferSize * 8 - m_currentByte * 8)
                {
                    return 0;
                }
            }
            else if(p_numberOfBitsToWrite > m_bufferSize * 8 - (m_currentByte - 1) * 8 - m_writtenBits)
            {
                return 0;
            }


            // If we will write exactly one byte we can do a bit speedup
            if(p_numberOfBitsToWrite == sizeof(unsigned char) * 8 && m_writtenBits == 0)
            {
                m_buffer[m_currentByte] = *(unsigned char*)p_buffer;
                m_currentByte++;
                return p_numberOfBitsToWrite;
            }

            uint32_t BitsToWrite = 0;
            uint32_t LineToWrite = 0;
            uint32_t OutBitAmount = p_numberOfBitsToWrite;

            // While we still have bits to write
            while(p_numberOfBitsToWrite > 0)
            {
                if(m_writtenBits == 0)
                {
                    m_buffer[m_currentByte] = 0;
                    m_currentByte++;
                }

                // Check how many bits we can write in this byte
                BitsToWrite = 8 - m_writtenBits;

                // We will write "full bytes" until we need to write bits
                // If the bits left on the buffer byte (max 8) is less then
                // the amount of bits we have left to write, we se chose that instead
                if(BitsToWrite > p_numberOfBitsToWrite)
                {
                    BitsToWrite = p_numberOfBitsToWrite;
                }

                // Fetch the byte from the input buffer and use the bit operation
                // and with mask on how many bits we should write to remove data
                // that shouldn't be written
                LineToWrite = *(int*)p_buffer & ((1 << BitsToWrite) - 1);

                // Offset the byte we want to write with how many bits we've
                // already written (memorywise with bitshift)
                LineToWrite <<= m_writtenBits;

                // Add it to the buffer with the OR operator
                m_buffer[m_currentByte - 1] |= LineToWrite;

                // Remove how many bits we've written
                p_numberOfBitsToWrite -= BitsToWrite;

                // Move input buffer pointer forward
                *(int*)p_buffer >>= BitsToWrite;

                // Check how many bits we have left on the last byte we
                // wrote with modolus
                m_writtenBits = (m_writtenBits + BitsToWrite) % 8;
            }

            return OutBitAmount;
        }

        uint32_t NetworkStreamer::ReadBits(void* p_buffer, uint32_t p_numberOfBitsToRead)
        {
            // If we already entered another mode
            if(m_IsWriting)
            {
                return 0;
            }

            m_IsReading = true;


            // Check if enough memory left on message
            if(!m_readBits)
            {
                // If we have 0 bits, we will initialize a new byte
                if(p_numberOfBitsToRead > m_bufferSize * 8 - m_currentByte * 8)
                {
                    return 0;
                }
            }
            else if(p_numberOfBitsToRead > m_bufferSize * 8 - (m_currentByte - 1) * 8 - m_readBits)
            {
                return 0;
            }

            //// Check if enough memory left on message
            // if(p_numberOfBitsToRead > m_bufferSize * 8 - m_currentByte * 8 - m_readBits)
            //{
            //    return 0;
            //}

            // If we will read exactly one byte we can do a bit speedup
            if(p_numberOfBitsToRead == sizeof(unsigned char) * 8 && m_readBits == 0)
            {
                *(unsigned char*)p_buffer = m_buffer[m_currentByte];
                m_currentByte++;
                return p_numberOfBitsToRead;
            }

            uint32_t BitsToRead = 0;
            uint32_t LineToRead = 0;
            uint32_t OutBitAmount = p_numberOfBitsToRead;
            uint32_t OffsetBits = 0;

            // While we have bits left to read
            while(p_numberOfBitsToRead > 0)
            {
                // If we start new we increase byte
                if(m_readBits == 0)
                {
                    m_currentByte++;
                }

                // Check how many bits there are left to read on this byte
                BitsToRead = 8 - m_readBits;

                // Check if the amount of bits left on the current byte
                // are more then we need to read
                if(BitsToRead > p_numberOfBitsToRead)
                {
                    BitsToRead = p_numberOfBitsToRead;
                }

                // Fetch the data from the buffer
                LineToRead = m_buffer[m_currentByte - 1];

                // Bitstep the data so only the data we want to read is in the line
                // If data is 00100, and were only interested in 001, we step two steps
                // left and get 001--
                LineToRead >>= m_readBits;

                // Bitstep a mask that will exclude data we're not intersted in
                // aka the amount of bits we have left to read
                LineToRead &= ((1 << BitsToRead) - 1);

                // We add the line with an offset(how many bytes of the int we've already read)
                *(int*)p_buffer |= LineToRead << OffsetBits;

                // Increment the offset
                OffsetBits += BitsToRead;

                // Remove from bits left to read
                p_numberOfBitsToRead -= BitsToRead;

                // See if any bits left on byte
                m_readBits = (m_readBits + BitsToRead) % 8;
            }

            return OutBitAmount;
        }

        bool NetworkStreamer::WriteInt8(int8_t p_Value)
        {
            int32_t Value = p_Value;

            uint32_t AmountWritten = WriteBits(&p_Value, sizeof(int8_t) * 8);

            return AmountWritten == sizeof(int8_t) * 8;
        }

        int8_t NetworkStreamer::ReadInt8()
        {
            int32_t Value = 0;

            int32_t AmountRead = ReadBits(&Value, sizeof(int8_t) * 8);

            return Value;
        }

        bool NetworkStreamer::WriteInt16(int16_t p_Value)
        {
            int32_t Value = p_Value;

            uint32_t AmountWritten = WriteBits(&p_Value, sizeof(int16_t) * 8);

            return AmountWritten == sizeof(int16_t) * 8;
        }

        int16_t NetworkStreamer::ReadInt16()
        {
            int32_t Value = 0;

            ReadBits(&Value, sizeof(int16_t) * 8);

            return Value;
        }

        bool NetworkStreamer::WriteInt32(int32_t p_Value)
        {
            uint32_t AmountWritten = WriteBits(&p_Value, sizeof(int32_t) * 8);

            return AmountWritten == sizeof(int32_t) * 8;
        }

        int32_t NetworkStreamer::ReadInt32()
        {
            int32_t Out = 0;

            ReadBits(&Out, sizeof(int32_t) * 8);

            return Out;
        }

        bool NetworkStreamer::WriteInt64(int64_t p_Value)
        {
            uint32_t AmountWritten = WriteBits(&p_Value, sizeof(int64_t) * 4);

            AmountWritten += WriteBits(&((int32_t*)&p_Value)[1], sizeof(int64_t) * 4);

            return AmountWritten == sizeof(int64_t) * 8;
        }

        int64_t NetworkStreamer::ReadInt64()
        {
            int64_t Out = 0;

            ReadBits(&Out, sizeof(int64_t) * 4);

            ReadBits(&((int32_t*)&Out)[1], sizeof(int64_t) * 4);

            return Out;
        }

        bool NetworkStreamer::WriteUnsignedInt8(uint8_t p_Value)
        {
            uint32_t Value = p_Value;

            uint32_t AmountWritten = WriteBits(&p_Value, sizeof(uint8_t) * 8);

            return AmountWritten == sizeof(uint8_t) * 8;
        }

        uint8_t NetworkStreamer::ReadUnsignedInt8()
        {
            uint32_t Value = 0;

            int32_t AmountRead = ReadBits(&Value, sizeof(uint8_t) * 8);

            return Value;
        }

        bool NetworkStreamer::WriteUnsignedInt16(uint16_t p_Value)
        {
            uint32_t Value = p_Value;

            uint32_t AmountWritten = WriteBits(&p_Value, sizeof(uint16_t) * 8);

            return AmountWritten == sizeof(uint16_t) * 8;
        }

        uint16_t NetworkStreamer::ReadUnsignedInt16()
        {
            uint32_t Out = 0;

            ReadBits(&Out, sizeof(uint16_t) * 8);

            return Out;
        }

        bool NetworkStreamer::WriteUnsignedInt32(uint32_t p_Value)
        {
            uint32_t AmountWritten = WriteBits(&p_Value, sizeof(int32_t) * 8);

            return AmountWritten == sizeof(int32_t) * 8;
        }

        uint32_t NetworkStreamer::ReadUnsignedInt32()
        {
            uint32_t Out = 0;

            ReadBits(&Out, sizeof(uint32_t) * 8);

            return Out;
        }

        bool NetworkStreamer::WriteUnsignedInt64(uint64_t p_Value)
        {
            uint32_t AmountWritten = WriteBits(&p_Value, sizeof(uint64_t) * 4);

            AmountWritten += WriteBits(&((int32_t*)&p_Value)[1], sizeof(uint64_t) * 4);

            return AmountWritten == sizeof(uint64_t) * 8;
        }

        uint64_t NetworkStreamer::ReadUnsignedInt64()
        {
            uint64_t Out = 0;

            ReadBits(&Out, sizeof(uint64_t) * 4);

            ReadBits(&((uint32_t*)&Out)[1], sizeof(uint64_t) * 4);

            return Out;
        }

        bool NetworkStreamer::WriteFloat(float p_Value)
        {
            // TODOCM because we know its equal size on our compiler... if not we will get error in memory
            // Could case it to an int, but if float is larger then int32 we still have problem
            uint32_t AmountWritten = WriteBits(&p_Value, sizeof(float) * 8);

            return AmountWritten == sizeof(float) * 8;
        }

        float NetworkStreamer::ReadFloat()
        {
            int32_t Out = 0;

            ReadBits(&Out, sizeof(int32_t) * 8);

            return *(float*)&Out;
        }

        bool NetworkStreamer::WriteDouble(double p_Value)
        {
            uint32_t AmountWritten = WriteBits(&p_Value, sizeof(double) * 4);

            AmountWritten += WriteBits(&((int32_t*)&p_Value)[1], sizeof(double) * 4);

            return AmountWritten == sizeof(double) * 8;
        }

        double NetworkStreamer::ReadDouble()
        {
            uint64_t Out = 0;

            ReadBits(&Out, sizeof(double) * 4);

            ReadBits(&((uint32_t*)&Out)[1], sizeof(double) * 4);

            return *(double*)&Out;
        }

        bool NetworkStreamer::WriteBool(bool p_Value)
        {
            uint32_t BoolInt = (uint32_t)p_Value;

            uint32_t AmountWritten = WriteBits(&BoolInt, 1);

            return AmountWritten == 1;
        }

        bool NetworkStreamer::ReadBool()
        {
            uint32_t Value = 0;

            uint32_t AmountRead = ReadBits(&Value, 1);

            return (bool)Value;
        }

        bool NetworkStreamer::WriteFloat2(DirectX::XMFLOAT2 p_Value)
        {
            WriteFloat(p_Value.x);
            WriteFloat(p_Value.y);
            return true;
        }

        DirectX::XMFLOAT2 NetworkStreamer::ReadFloat2()
        {
            DirectX::XMFLOAT2 Out;
            Out.x = ReadFloat();
            Out.y = ReadFloat();

            return Out;
        }

        bool NetworkStreamer::WriteFloat3(DirectX::XMFLOAT3 p_Value)
        {
            WriteFloat(p_Value.x);
            WriteFloat(p_Value.y);
            WriteFloat(p_Value.z);

            return true;
        }

        DirectX::XMFLOAT3 NetworkStreamer::ReadFloat3()
        {
            DirectX::XMFLOAT3 Out;
            Out.x = ReadFloat();
            Out.y = ReadFloat();
            Out.z = ReadFloat();

            return Out;
        }

        bool NetworkStreamer::WriteFloat4(DirectX::XMFLOAT4 p_Value)
        {
            WriteFloat(p_Value.x);
            WriteFloat(p_Value.y);
            WriteFloat(p_Value.z);
            WriteFloat(p_Value.w);

            return true;
        }

        DirectX::XMFLOAT4 NetworkStreamer::ReadFloat4()
        {
            DirectX::XMFLOAT4 Out;
            Out.x = ReadFloat();
            Out.y = ReadFloat();
            Out.z = ReadFloat();
            Out.w = ReadFloat();

            return Out;
        }

        bool NetworkStreamer::WriteRotationQuaternion(DirectX::XMFLOAT4 p_Value) { return WriteFloat4(p_Value); }

        DirectX::XMFLOAT4 NetworkStreamer::ReadRotationQuaternion() { return ReadFloat4(); }

        bool NetworkStreamer::WriteString(std::string p_Value)
        {
            uint32_t AmountWritten = 0;

            uint32_t StringLength = p_Value.size();

            // Write the length of the string
            WriteUnsignedInt32(StringLength);

            const char* CharArray = p_Value.c_str();

            // Write all chars
            for(size_t i = 0; i < StringLength; i++)
            {
                uint32_t Value = (uint32_t)CharArray[i];

                AmountWritten += WriteBits(&Value, sizeof(unsigned char) * 8);
            }

            return AmountWritten == StringLength * sizeof(unsigned char) * 8;
        }

        std::string NetworkStreamer::ReadString()
        {
            std::string OutString;

            uint32_t StringLength = 0;
            uint32_t AmountRead = ReadBits(&StringLength, sizeof(int32_t) * 8);

            char* StringArray = new char[StringLength + 1];


            for(size_t i = 0; i < StringLength; i++)
            {
                uint32_t Value = 0;
                AmountRead = ReadBits(&Value, sizeof(unsigned char) * 8);
                StringArray[i] = (char)Value;
            }
            StringArray[StringLength] = 0;
            OutString = std::string(StringArray);

            delete StringArray;

            return OutString;
        }

        bool NetworkStreamer::WriteBuffer(void* p_buffer, uint32_t p_bufferSize)
        {
            uint32_t AmountWritten = 0;
            for(size_t i = 0; i < p_bufferSize; i++)
            {
                uint32_t Value = ((char*)p_buffer)[i];
                AmountWritten += WriteBits(&Value, sizeof(char) * 8);
            }
            return AmountWritten == p_bufferSize * 8;
        }

        void NetworkStreamer::ReadBuffer(void* p_buffer, uint32_t p_bufferSize)
        {
            for(size_t i = 0; i < p_bufferSize; i++)
            {
                uint32_t Value = 0;
                ReadBits(&Value, sizeof(char) * 8);
                ((char*)p_buffer)[i] = Value;
            }
        }

        void NetworkStreamer::WriteMemcpy(void* p_buffer, uint32_t p_bufferSize)
        {
            // Check if we're in wrong mode
            if(m_IsReading)
            {
                return;
            }
            m_IsWriting = true;

            // Check if there's memory left in buffer
            if(p_bufferSize > m_bufferSize - (m_currentByte + 1))
            {
                return;
            }

            // Reset step forward to full byte, (pointers cannot adress bits)
            if(m_writtenBits)
            {
                m_writtenBits = 0;
                m_currentByte++;
            }

            memcpy(&m_buffer[m_currentByte - 1], p_buffer, p_bufferSize);

            m_currentByte += p_bufferSize;
        }

        void NetworkStreamer::ReadMemcpy(void* p_buffer, uint32_t p_bufferSize)
        {
            // Check if we're in wrong mode
            if(m_IsWriting)
            {
                return;
            }
            m_IsReading = true;

            // Check if there's memory left in buffer
            if(p_bufferSize > m_bufferSize - (m_currentByte + 1))
            {
                return;
            }

            // Reset step forward to full byte, (pointers cannot adress bits)
            if(m_readBits)
            {
                m_readBits = 0;
                m_currentByte++;
            }

            memcpy(p_buffer, &m_buffer[m_currentByte - 1], p_bufferSize);

            m_currentByte += p_bufferSize;
        }
    }
}