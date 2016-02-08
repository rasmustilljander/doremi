#include <UnitTest/Include/BitStreamerTest.hpp>


TEST_F(BitStreamerTest, writeOutsideMemoryFullBytes)
{
    uint32_t size = sizeof(m_netMessage->Data) / 4;

    for(size_t i = 0; i < size + 1; i++)
    {
        if(i == size)
        {
            int breakp = 3;
        }
        m_streamer->WriteUnsignedInt32(4294967295);
    }
}

TEST_F(BitStreamerTest, writeOutsideMemoryBits)
{
    uint32_t size = sizeof(m_netMessage->Data) * 8;

    for(size_t i = 0; i < size + 1; i++)
    {
        if(i == size)
        {
            int breakp = 3;
        }
        m_streamer->WriteBool(true);
    }
}

TEST_F(BitStreamerTest, readOutsideMemoryBitsThenByte)
{
    uint32_t size = sizeof(m_netMessage->Data) * 8;

    for(size_t i = 0; i < size - 1; i++)
    {
        m_streamer->ReadBool();
    }
    m_streamer->ReadUnsignedInt32();
}

TEST_F(BitStreamerTest, readOutsideMemoryFullBytes)
{
    uint32_t size = sizeof(m_netMessage->Data) / 4;

    for(size_t i = 0; i < size + 1; i++)
    {
        if(i == size)
        {
            int breakp = 3;
        }
        m_streamer->ReadUnsignedInt32();
    }
}

TEST_F(BitStreamerTest, readOutsideMemoryBits)
{
    uint32_t size = sizeof(m_netMessage->Data) * 8;

    for(size_t i = 0; i < size + 1; i++)
    {
        if(i == size)
        {
            int breakp = 3;
        }
        m_streamer->ReadBool();
    }
}

TEST_F(BitStreamerTest, writeOutsideMemoryBitsThenByte)
{
    uint32_t size = sizeof(m_netMessage->Data) * 8;

    for(size_t i = 0; i < size - 1; i++)
    {
        m_streamer->ReadBool();
    }
    m_streamer->ReadUnsignedInt32();
}