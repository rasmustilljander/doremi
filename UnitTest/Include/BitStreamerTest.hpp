#pragma once
#include <gtest/gtest.h>
#include <Doremi/Core/Include/Streamers/NetworkStreamer.hpp>
#include <Doremi/Core/Include/Manager/Network/NetMessage.hpp>

class BitStreamerTest : public testing::Test
{
public:
    BitStreamerTest() {}

    ~BitStreamerTest() {}

    Doremi::Core::NetworkStreamer* m_streamer;
    Doremi::Core::NetMessage* m_netMessage;
    uint32_t m_bytesWritten;
    uint32_t m_bytesRead;
    bool m_finished;

    void SetUp()
    {
        m_streamer = new Doremi::Core::NetworkStreamer();
        m_netMessage = new Doremi::Core::NetMessage();
        m_bytesWritten = 0;
        m_bytesRead = 0;
        m_finished = false;
        unsigned char* point = m_netMessage->Data;
        m_streamer->SetTargetBuffer(point, sizeof(m_netMessage->Data));
    }
    void TearDown()
    {
        delete m_streamer;
        delete m_netMessage;
    }
};