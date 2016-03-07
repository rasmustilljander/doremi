#include <UnitTest/Include/NetworkEventTest.hpp>
#include <Doremi/Core/Include/EventHandler/Events/ExampleEvent.hpp>


TEST_F(NetworkEventTest, basicNoWrite)
{
    m_netEventSender->WriteEvents(*m_streamer, sizeof(m_netMessage->Data), m_bytesWritten, m_finished);

    ASSERT_EQ(true, m_finished);
    ASSERT_EQ(2, m_bytesWritten);
}

TEST_F(NetworkEventTest, basicOneWrite)
{
    m_netEventSender->QueueEventToFrame(new Doremi::Core::ExampleEvent());

    m_netEventSender->WriteEvents(*m_streamer, sizeof(m_netMessage->Data), m_bytesWritten, m_finished);

    ASSERT_EQ(true, m_finished);
    ASSERT_EQ(5, m_bytesWritten);
}

TEST_F(NetworkEventTest, writeNoMemoryHeaderStart)
{
    for(size_t i = 0; i < 6; i++)
    {
        m_netEventSender->QueueEventToFrame(new Doremi::Core::ExampleEvent());
    }

    m_netEventSender->WriteEvents(*m_streamer, 1, m_bytesWritten, m_finished);

    ASSERT_EQ(false, m_finished);
    ASSERT_EQ(0, m_bytesWritten);
}

TEST_F(NetworkEventTest, writeNoMemoryHeaderLater)
{
    for(size_t i = 0; i < 6; i++)
    {
        m_netEventSender->QueueEventToFrame(new Doremi::Core::ExampleEvent());
    }


    m_netEventSender->WriteEvents(*m_streamer, 2, m_bytesWritten, m_finished);

    ASSERT_EQ(false, m_finished);
    ASSERT_EQ(2, m_bytesWritten);
}

TEST_F(NetworkEventTest, writeNoMemoryForInfo)
{
    for(size_t i = 0; i < 32; i++)
    {
        m_netEventSender->QueueEventToFrame(new Doremi::Core::ExampleEvent());
    }


    m_netEventSender->WriteEvents(*m_streamer, 6, m_bytesWritten, m_finished);

    ASSERT_EQ(false, m_finished);
    ASSERT_EQ(6, m_bytesWritten);
}

// Can't have this cause of size of the message
// TEST_F(NetworkEventTest, writeNoMemoryInfoLarge)
//{
//    for(size_t i = 0; i < 257; i++)
//    {
//        m_netEventSender->QueueEventToFrame(new Doremi::Core::ExampleEvent());
//    }
//
//    // no check if we write outside memory on netmessage
//    m_netEventSender->WriteEvents(*m_streamer, 1287, m_bytesWritten, m_finished);
//
//    ASSERT_EQ(true, m_finished);
//    ASSERT_EQ(1065, m_bytesWritten);
//}

TEST_F(NetworkEventTest, updateBufferFromSequenceSame)
{
    m_netEventSender->QueueEventToFrame(new Doremi::Core::ExampleEvent());

    m_netEventSender->AddFrameQueuedObjectsToBuffer();

    m_netEventSender->UpdateBufferWithRecievedClientSequenceAcc(255);

    ASSERT_EQ(0, m_netEventSender->GetNextSequenceUsed());
}

TEST_F(NetworkEventTest, updateBufferFromSequenceInvalid)
{
    m_netEventSender->QueueEventToFrame(new Doremi::Core::ExampleEvent());

    m_netEventSender->AddFrameQueuedObjectsToBuffer();

    m_netEventSender->UpdateBufferWithRecievedClientSequenceAcc(188);

    ASSERT_EQ(0, m_netEventSender->GetNextSequenceUsed());
}

TEST_F(NetworkEventTest, updateBufferFromSequenceBasic)
{
    m_netEventSender->QueueEventToFrame(new Doremi::Core::ExampleEvent());

    m_netEventSender->AddFrameQueuedObjectsToBuffer();

    m_netEventSender->UpdateBufferWithRecievedClientSequenceAcc(1);

    ASSERT_EQ(1, m_netEventSender->GetNextSequenceUsed());
}


TEST_F(NetworkEventTest, updateBufferFromSequenceHalf)
{
    for(size_t i = 0; i < 120; i++)
    {
        m_netEventSender->QueueEventToFrame(new Doremi::Core::ExampleEvent());

        m_netEventSender->AddFrameQueuedObjectsToBuffer();
    }


    m_netEventSender->UpdateBufferWithRecievedClientSequenceAcc(120);

    ASSERT_EQ(120, m_netEventSender->GetNextSequenceUsed());
}

TEST_F(NetworkEventTest, updateBufferFromSequenceMoreThenExist)
{
    for(size_t i = 0; i < 100; i++)
    {
        m_netEventSender->QueueEventToFrame(new Doremi::Core::ExampleEvent());

        m_netEventSender->AddFrameQueuedObjectsToBuffer();
    }


    m_netEventSender->UpdateBufferWithRecievedClientSequenceAcc(120);

    ASSERT_EQ(100, m_netEventSender->GetNextSequenceUsed());
}

TEST_F(NetworkEventTest, basicNoRead)
{
    m_networkEventReceiver->ReadEvents(*m_streamerRead, sizeof(m_netMessage->Data), m_bytesRead);

    std::list<Doremi::Core::Event*> eventlist = m_networkEventReceiver->GetEventsReceivedFromServer();

    ASSERT_EQ(0, eventlist.size());
    ASSERT_EQ(2, m_bytesRead);
}

TEST_F(NetworkEventTest, basicOneRead)
{
    m_netEventSender->QueueEventToFrame(new Doremi::Core::ExampleEvent());

    m_netEventSender->WriteEvents(*m_streamer, sizeof(m_netMessage->Data), m_bytesWritten, m_finished);

    m_networkEventReceiver->ReadEvents(*m_streamerRead, sizeof(m_netMessage->Data), m_bytesRead);

    std::list<Doremi::Core::Event*> eventlist = m_networkEventReceiver->GetEventsReceivedFromServer();

    ASSERT_EQ(1, eventlist.size());
    ASSERT_EQ(5, m_bytesRead);
}

TEST_F(NetworkEventTest, readNoMemoryHeaderStart)
{
    m_netEventSender->QueueEventToFrame(new Doremi::Core::ExampleEvent());

    m_netEventSender->WriteEvents(*m_streamer, sizeof(m_netMessage->Data), m_bytesWritten, m_finished);

    m_networkEventReceiver->ReadEvents(*m_streamerRead, 1, m_bytesRead);

    std::list<Doremi::Core::Event*> eventlist = m_networkEventReceiver->GetEventsReceivedFromServer();

    ASSERT_EQ(0, eventlist.size());
    ASSERT_EQ(0, m_bytesRead);
}

TEST_F(NetworkEventTest, readNoMemoryHeaderLater)
{
    m_netEventSender->QueueEventToFrame(new Doremi::Core::ExampleEvent());

    m_netEventSender->WriteEvents(*m_streamer, sizeof(m_netMessage->Data), m_bytesWritten, m_finished);

    m_networkEventReceiver->ReadEvents(*m_streamerRead, 2, m_bytesRead);

    std::list<Doremi::Core::Event*> eventlist = m_networkEventReceiver->GetEventsReceivedFromServer();

    ASSERT_EQ(0, eventlist.size());
    ASSERT_EQ(2, m_bytesRead);
}

// We ignore this one, cause we always asume the server messages are valid
TEST_F(NetworkEventTest, readNoMemoryForInfo)
{
    for(size_t i = 0; i < 3; i++)
    {
        m_netEventSender->QueueEventToFrame(new Doremi::Core::ExampleEvent());
    }

    m_netEventSender->WriteEvents(*m_streamer, sizeof(m_netMessage->Data), m_bytesWritten, m_finished);

    m_networkEventReceiver->ReadEvents(*m_streamerRead, 3, m_bytesRead);

    std::list<Doremi::Core::Event*> eventlist = m_networkEventReceiver->GetEventsReceivedFromServer();

    ASSERT_EQ(3, eventlist.size());
    ASSERT_EQ(1 + 1 + (1 * 3) + 2, m_bytesRead);
}

// Same as above
TEST_F(NetworkEventTest, readNoMemoryInfoLarge)
{
    for(size_t i = 0; i < 257; i++)
    {
        m_netEventSender->QueueEventToFrame(new Doremi::Core::ExampleEvent());
    }

    m_netEventSender->WriteEvents(*m_streamer, sizeof(m_netMessage->Data), m_bytesWritten, m_finished);

    m_networkEventReceiver->ReadEvents(*m_streamerRead, sizeof(m_netMessage->Data), m_bytesRead);

    std::list<Doremi::Core::Event*> eventlist = m_networkEventReceiver->GetEventsReceivedFromServer();

    // Not really sure if it's 248, hard to count
    ASSERT_EQ(257, eventlist.size());

    // Not sure with this either, but should be close to max size
    ASSERT_EQ(294, m_bytesRead);
}