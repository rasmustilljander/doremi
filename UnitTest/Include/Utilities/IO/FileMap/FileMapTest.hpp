#pragma once
#include <gtest/gtest.h>
#include <Utility/Utilities/Include/IO/FileMap/FileMap.hpp>

using namespace Doremi::Utilities::IO;

class FileMapTest : public testing::Test
{
public:
    FileMapTest() {}
    virtual ~FileMapTest() {}

    FileMap* m_fileMap;

    void SetUp() override { m_fileMap = new FileMap(); }

    void TearDown() override { delete m_fileMap; }
};