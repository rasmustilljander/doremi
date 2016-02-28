#pragma once
#include <gtest/gtest.h>
#include <Doremi/Core/Include/NetworkEventSender.hpp>
#include <Doremi/Core/Include/NetworkEventReceiver.hpp>
#include <Doremi/Core/Include/Streamers/NetworkStreamer.hpp>
#include <Doremi/Core/Include/Network/NetMessages.hpp>

class NetworkEventTest : public testing::Test
{
public:
    NetworkEventTest() {}

    ~NetworkEventTest() {}

    Doremi::Core::NetworkEventSender* m_netEventSender;
    Doremi::Core::NetworkEventReceiver* m_networkEventReceiver;
    Doremi::Core::NetworkStreamer* m_streamer;
    Doremi::Core::NetworkStreamer* m_streamerRead;
    Doremi::Core::NetMessageBuffer* m_netMessage;
    uint32_t m_bytesWritten;
    uint32_t m_bytesRead;
    bool m_finished;

    void SetUp()
    {
        m_netEventSender = new Doremi::Core::NetworkEventSender();
        m_networkEventReceiver = new Doremi::Core::NetworkEventReceiver();
        m_streamer = new Doremi::Core::NetworkStreamer();
        m_streamerRead = new Doremi::Core::NetworkStreamer();
        m_netMessage = new Doremi::Core::NetMessageBuffer();
        m_bytesWritten = 0;
        m_bytesRead = 0;
        m_finished = false;
        unsigned char* point = m_netMessage->Data;
        m_streamer->SetTargetBuffer(point, sizeof(m_netMessage->Data));
        m_streamerRead->SetTargetBuffer(point, sizeof(m_netMessage->Data));
    }
    void TearDown()
    {
        delete m_netEventSender;
        delete m_networkEventReceiver;
        delete m_streamer;
        delete m_netMessage;
    }
};