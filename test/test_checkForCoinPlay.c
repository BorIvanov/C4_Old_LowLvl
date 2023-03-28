/*
 * test_checkForCoinPlay.c
 *
 *  Created on: 19 apr. 2018
 *      Author: jajansen
 */

/* INCLUDES: */
//#include "../vendor/ceedling/vendor/unity/src/unity.h"
#include "motorPIDs.h"
#include "encoder.h"
#include "stm32f30x_rcc.h"
#include "stm32f30x_tim.h"
#include "motorpwm.h"
#include "homing.h"
#include "motorControlInterrupt.h"
#include "servoControl.h"
#include "system_stm32f30x.h"
#include "stm32f30x_misc.h"
#include "checkForCoinPlay.h"
#include "vars.h"

/* MOCKED INCLUDES: */
#include "mock_stm32f30x_gpio.h"
#include "mock_i2c.h"

/* does not matter, but keep as original: */
#define I2C_ADD_CD			0x38<<1

unsigned char invertBuffer = 0x00;

void setUp(void)
{

}

void tearDown(void)
{

}

void FillStacks() {
	// fill all storage stacks
	for (int i = 0; i <= 2; i++) {
		mem_StorageStack[i] = 7;
	}
}

void EmptyStacks() {
	// empty all storage stacks
	for (int i = 0; i <= 2; i++) {
		mem_StorageStack[i] = 0;
	}
}

void sendReceiveData_CallbackReturnInverted(uint8_t address, uint8_t nBytesSend, uint8_t nBytesReceive, int NumCalls)
{
	buffer[0] = invertBuffer ^ 0xFF;
}

void test_queryLightGate_ShouldReturnErrorCode9(void)
{
	/* queryLightGate() -->  * Returns:
	 * -1: All gates are free
	 * 1-7: Stack where gate is blocked
	 * 9: More than one gate is blocked
	 */

	// 			| 00000011 |
	invertBuffer = 0x03;

	sendReceiveData_StubWithCallback(sendReceiveData_CallbackReturnInverted);

	TEST_ASSERT_EQUAL_INT32(9, queryLightGate());
}

void test_queryLightGate_ShouldReturnMinusOne(void)
{
	/* queryLightGate() -->  * Returns:
	 * -1: All gates are free
	 * 1-7: Stack where gate is blocked
	 * 9: More than one gate is blocked
	 */

	// 			| 00000000 |
	invertBuffer = 0x00;

	sendReceiveData_StubWithCallback(sendReceiveData_CallbackReturnInverted);

	TEST_ASSERT_EQUAL_INT32(-1, queryLightGate());
}

void test_queryLightGate_ShouldReturnOne(void)
{
	/* queryLightGate() -->  * Returns:
	 * -1: All gates are free
	 * 1-7: Stack where gate is blocked
	 * 9: More than one gate is blocked
	 */

	// 			| 00000010 |
	invertBuffer = 0x02;

	sendReceiveData_StubWithCallback(sendReceiveData_CallbackReturnInverted);

	TEST_ASSERT_EQUAL_INT32(1, queryLightGate());
}

void test_queryLightGate_ShouldReturnTwo(void)
{
	/* queryLightGate() -->  * Returns:
	 * -1: All gates are free
	 * 1-7: Stack where gate is blocked
	 * 9: More than one gate is blocked
	 */

	// 			| 00000100 |
	invertBuffer = 0x04;

	sendReceiveData_StubWithCallback(sendReceiveData_CallbackReturnInverted);

	TEST_ASSERT_EQUAL_INT32(2, queryLightGate());
}

void test_queryLightGate_ShouldReturnThree(void)
{
	/* queryLightGate() -->  * Returns:
	 * -1: All gates are free
	 * 1-7: Stack where gate is blocked
	 * 9: More than one gate is blocked
	 */

	// 			| 00001000 |
	invertBuffer = 0x08;

	sendReceiveData_StubWithCallback(sendReceiveData_CallbackReturnInverted);

	TEST_ASSERT_EQUAL_INT32(3, queryLightGate());
}

void test_queryLightGate_ShouldReturnFour(void)
{
	/* queryLightGate() -->  * Returns:
	 * -1: All gates are free
	 * 1-7: Stack where gate is blocked
	 * 9: More than one gate is blocked
	 */

	// 			| 00010000 |
	invertBuffer = 0x10;

	sendReceiveData_StubWithCallback(sendReceiveData_CallbackReturnInverted);

	TEST_ASSERT_EQUAL_INT32(4, queryLightGate());
}

void test_queryLightGate_ShouldReturnFive(void)
{
	/* queryLightGate() -->  * Returns:
	 * -1: All gates are free
	 * 1-7: Stack where gate is blocked
	 * 9: More than one gate is blocked
	 */

	// 			| 00100000 |
	invertBuffer = 0x20;

	sendReceiveData_StubWithCallback(sendReceiveData_CallbackReturnInverted);

	TEST_ASSERT_EQUAL_INT32(5, queryLightGate());
}

void test_queryLightGate_ShouldReturnSix(void)
{
	/* queryLightGate() -->  * Returns:
	 * -1: All gates are free
	 * 1-7: Stack where gate is blocked
	 * 9: More than one gate is blocked
	 */

	// 			| 01000000 |
	invertBuffer = 0x40;

	sendReceiveData_StubWithCallback(sendReceiveData_CallbackReturnInverted);

	TEST_ASSERT_EQUAL_INT32(6, queryLightGate());
}

void test_queryLightGate_ShouldReturnSeven(void)
{
	/* queryLightGate() -->  * Returns:
	 * -1: All gates are free
	 * 1-7: Stack where gate is blocked
	 * 9: More than one gate is blocked
	 */

	// 			| 10000000 |
	invertBuffer = 0x80;

	sendReceiveData_StubWithCallback(sendReceiveData_CallbackReturnInverted);

	TEST_ASSERT_EQUAL_INT32(7, queryLightGate());
}

/* All normal conditions are already tested in previous cases.
 *
 * Since there are only 8 channels on the I2C expander, of which
 * 7 are used for detection, it requires only 2^8=256 iterations
 * in order to test all possible combinations of channels.
 *
 * Only the ones that are a power of 2 should return a valid
 * output and should be considered.
 *
 */
void test_queryLightGate_AllPossibilities(void)
{
	/* queryLightGate() -->  * Returns:
	 * -1: All gates are free
	 * 1-7: Stack where gate is blocked
	 * 9: More than one gate is blocked
	 */

	// Mock the send/receive command
	sendReceiveData_StubWithCallback(sendReceiveData_CallbackReturnInverted);

	int counter = 0;

	for(int i = 0; i < 255; i++)
	{
		invertBuffer = i;
		if (i == 0)	// no gate is high
		{		// 0 should return if no bit is high
			TEST_ASSERT_EQUAL_INT32(-1, queryLightGate());
		}
		else if (!((i & (i - 1))==0) || i == 1) 	// i is not power of 2 or bit 1 is high
		{		// should return that more than one gate is blocked
			TEST_ASSERT_EQUAL_INT32(9, queryLightGate());
		}
		else 	// i == power of 2
		{	 	//only one bit is high, should return gatenr.
			counter++;
			TEST_ASSERT_EQUAL_INT32(counter, queryLightGate());
		}
	}
}

void test_whatStackToEmptyAllFullStacks_ShouldReturnStackIndex2(void)
{
	FillStacks();
	// expect stack 2
	TEST_ASSERT_EQUAL_INT32(2, whatStackToEmpty());
}

void test_whatStackToEmptyAllEmptyStacks_ShouldReturnStackIndexMinus1(void)
{
	EmptyStacks();
	// expect stack 2
	TEST_ASSERT_EQUAL_INT32(-1, whatStackToEmpty());
}

void test_whatStackToEmptyOneCoinStack2_ShouldReturnStackIndex2(void)
{
	EmptyStacks();
	// put one coin in stack 2
	mem_StorageStack[2] = 1;

	// expect stack 2
	TEST_ASSERT_EQUAL_INT32(2, whatStackToEmpty());
}

void test_whatStackToEmptyOneCoinStack1_ShouldReturnStackIndex1(void)
{
	EmptyStacks();

	// put one coin in stack 1
	mem_StorageStack[1] = 1;

	// expect stack 2
	TEST_ASSERT_EQUAL_INT32(1, whatStackToEmpty());
}

void test_whatStackToEmptyTwoCoinStack0_ShouldReturnStackIndex0(void)
{
	EmptyStacks();

	// put one coin in stack 0
	mem_StorageStack[0] = 2;

	// expect stack 2
	TEST_ASSERT_EQUAL_INT32(0, whatStackToEmpty());
}

void test_whatStackToEmptyOneCoinStack0_ShouldReturnStackIndex0(void)
{
	EmptyStacks();
	// put one coin in stack 0
	mem_StorageStack[0] = 1;

	// expect stack 2
	TEST_ASSERT_EQUAL_INT32(0, whatStackToEmpty());
}

void test_whatStackToEmptySevenCoinStack0_ShouldReturnStackIndex0(void)
{
	EmptyStacks();
	// put seven coins in stack 0
	mem_StorageStack[0] = 7;

	// expect stack 2
	TEST_ASSERT_EQUAL_INT32(0, whatStackToEmpty());
}

void test_whatStackToEmptySixCoinStack0_ShouldReturnStackIndex0(void)
{
	EmptyStacks();
	// put one coin in stack 0
	mem_StorageStack[0] = 6;

	// expect stack 2
	TEST_ASSERT_EQUAL_INT32(0, whatStackToEmpty());
}

void test_whatStackToFillEmptyStacks_ShouldReturnStackIndex0(void)
{
	EmptyStacks();
	TEST_ASSERT_EQUAL_INT32(0, whatStackToFill());
}

void test_whatStackToFillFullBoard_ShouldReturnStackIndexMinus1(void)
{
	FillStacks();
	TEST_ASSERT_EQUAL_INT32(-1, whatStackToFill());
}

void test_whatStackToFillOneCoinStack0_ShouldReturnStackIndex0(void)
{
	EmptyStacks();
	// put one coin in stack 0
	mem_StorageStack[0] = 1;
	TEST_ASSERT_EQUAL_INT32(0, whatStackToFill());
}

void test_whatStackToFillSixCoinStack0_ShouldReturnStackIndex0(void)
{
	EmptyStacks();
	// put six coins in stack 0
	mem_StorageStack[0] = 6;
	TEST_ASSERT_EQUAL_INT32(0, whatStackToFill());
}

void test_whatStackToFillSevenCoinStack0_ShouldReturnStackIndex1(void)
{
	EmptyStacks();
	// put seven coins in stack 0
	mem_StorageStack[0] = 7;
	TEST_ASSERT_EQUAL_INT32(1, whatStackToFill());
}

void test_whatStackToFillAllFullMinus1_ShouldReturnStackIndex2(void)
{
	FillStacks();
	// put six coins in stack 2
	mem_StorageStack[2] = 6;
	TEST_ASSERT_EQUAL_INT32(2, whatStackToFill());
}

void test_coinInsertDetection_ShouldReturnErrorCode9(void)
{
	// bigger than 7  --> more than one coin detected
	//			| 11111111 |
	invertBuffer = 0xFF;
	sendReceiveData_StubWithCallback(sendReceiveData_CallbackReturnInverted);

	TEST_ASSERT_EQUAL_INT32(9, coinInsertDetection());
}

void test_coinInsertDetection_ShouldReturnMinusOne(void)
{
	// no coins detected
	//			| 00000000 |
	invertBuffer = 0x00;
	sendReceiveData_StubWithCallback(sendReceiveData_CallbackReturnInverted);

	TEST_ASSERT_EQUAL_INT32(0, coinInsertDetection());
}

void sendReceiveData_CallbackReturnInvertedCallDependant(uint8_t address, uint8_t nBytesSend, uint8_t nBytesReceive, int NumCalls)
{
	switch (NumCalls)
	{
	case 0: // first time function is called
		buffer[0] = 0x00 ^ 0xFF;
		break;

	case 1: // second
		buffer[0] = 0x20 ^ 0xFF;
	    break;

	case 2: // third
		buffer[0] = 0x00 ^ 0xFF;
		break;

	default:
	    TEST_FAIL_MESSAGE("sendReceiveData was called too many times!");
	    break;
	}
}

void test_coinInsertDetection_ShouldReturnFiveAfterCoinInsert(void)
{
	GPIO_SetBits_Ignore();

	// 0x00 --> neutral state
	sendReceiveData_StubWithCallback(sendReceiveData_CallbackReturnInvertedCallDependant);
	TEST_ASSERT_EQUAL_INT32(0, coinInsertDetection());

	// 0x20	--> coin play in slot 5
	sendReceiveData_StubWithCallback(sendReceiveData_CallbackReturnInvertedCallDependant);
	TEST_ASSERT_EQUAL_INT32(0, coinInsertDetection());

	// 0x00 --> neutral state
	sendReceiveData_StubWithCallback(sendReceiveData_CallbackReturnInvertedCallDependant);
	TEST_ASSERT_EQUAL_INT32(5, coinInsertDetection());
}

void test_addToColumn_OneColOne(void)
{
	// clear board
	for(int i = 0; i <= 6; i++)
	{
		mem_Board[i] = 0;
	}

	TEST_ASSERT_EQUAL_INT32(1, addToColumn(0));

	TEST_ASSERT_EQUAL_INT32(1, mem_Board[0]);
}

void test_addToColumn_OneColSeven(void)
{
	int test[7];
	// clear board and test array
	for(int i = 0; i <= 6; i++)
	{
		mem_Board[i] = 0;
		test[i] = 0;
	}

	TEST_ASSERT_EQUAL_INT32(1, addToColumn(6));

	test[6] = 1;
	TEST_ASSERT_EQUAL_INT32_ARRAY(test, mem_Board, 7);
}

void test_addToColumnTen_ShouldReturnError(void)
{
	// clear board
	for(int i = 0; i <= 6; i++)
	{
		mem_Board[i] = 0;
	}

	TEST_ASSERT_EQUAL_INT32(-1, addToColumn(10));
}

void test_addToColumnMinusOne_ShouldReturnError(void)
{
	// clear board
	for(int i = 0; i <= 6; i++)
	{
		mem_Board[i] = 0;
	}

	TEST_ASSERT_EQUAL_INT32(-1, addToColumn(-1));
}

void test_addToFullColumnFive_ShouldReturnError(void)
{
	// clear board
	for(int i = 0; i <= 6; i++)
	{
		mem_Board[i] = 0;
	}
	mem_Board[5] = 6;

	TEST_ASSERT_EQUAL_INT32(-1, addToColumn(5));
}
