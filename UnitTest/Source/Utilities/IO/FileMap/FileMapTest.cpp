#include <UnitTest/Include/Utilities/IO/FileMap/FileMapTest.hpp>

TEST_F(FileMapTest, basicInit)
{
    void* pointer = m_fileMap->Initialize("test", 100);

    ASSERT_NE(nullptr, pointer);
}

TEST_F(FileMapTest, basicMultipleInit)
{
    FileMap a;
    void* first = a.Initialize("test", 1000000);

    FileMap b;
    void* second = b.Initialize("test", 1000000);

    ASSERT_NE(nullptr, first);
    ASSERT_NE(nullptr, second);
}

TEST_F(FileMapTest, basicMultipleWriteAndRead)
{
    FileMap a;
    void* first = a.Initialize("test", 1000000);

    FileMap b;
    void* second = b.Initialize("test", 1000000);

    ASSERT_NE(nullptr, first);
    ASSERT_NE(nullptr, second);
    ASSERT_NE(first, second);

    const std::string writeString = std::string("testing text");
    memcpy_s(first, 28, writeString.c_str(), 28);
    ASSERT_EQ("testing text", writeString);

    const char* secondAsChar = static_cast<char*>(second);
    const std::string readString = std::string(secondAsChar);
    ASSERT_EQ("testing text", readString);
}
