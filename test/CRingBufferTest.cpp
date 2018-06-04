#include <gtest/gtest.h>

#include <CRingBuffer.h>

TEST(RingBufferTest, PushPop)
{
	CRingBuffer cRingBuffer;
	cRingBuffer.init(3, 30);
	unsigned char pu8TestData[] = { 0, 1, 2, 3, 4 };
	cRingBuffer.push(pu8TestData, sizeof(pu8TestData));
	cRingBuffer.commit();

	unsigned char pu8PopData[sizeof(pu8TestData)];
	unsigned long sizePop = 0;
	cRingBuffer.pop(pu8PopData, sizeof(pu8PopData), &sizePop);

	EXPECT_EQ(sizeof(pu8TestData), sizePop);
	EXPECT_TRUE(0 == memcmp(pu8TestData, pu8PopData, sizePop));
}
