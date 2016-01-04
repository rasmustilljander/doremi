#include <ProximityHelperTesting.hpp>
#include <DirectXMath.h>

ProximityHelperTest::ProximityHelperTest() {}
ProximityHelperTest::~ProximityHelperTest() {}
void ProximityHelperTest::SetUp() {}
void ProximityHelperTest::TearDown() {}
TEST_F(ProximityHelperTest, proximityTest)
{
    using namespace DirectX;

    EXPECT_TRUE(ProximityHelperTest::IsInProximity(XMFLOAT3(50, 0, 0), XMFLOAT3(0, 0, 0), 50));
    EXPECT_FALSE(ProximityHelperTest::IsInProximity(XMFLOAT3(50, 0, 0), XMFLOAT3(0, 0, 0), 49));
    EXPECT_TRUE(ProximityHelperTest::IsInProximity(XMFLOAT3(50, 0, 0), XMFLOAT3(0, 0, 0), 51));
    EXPECT_TRUE(ProximityHelperTest::IsInProximity(XMFLOAT3(30, 5, 10), XMFLOAT3(5, 9, 7), 25.4951f)); // Range gathered from wolfram aplha
    EXPECT_FALSE(ProximityHelperTest::IsInProximity(XMFLOAT3(30, 5, 10), XMFLOAT3(5, 9, 7), 25.4950f)); // Range gathered from wolfram aplha
}