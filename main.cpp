#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/TestRegistry.h"
#include "CppUTestExt/MockSupport.h"
#include "CppUTestExt/MockSupportPlugin.h"
#include "stdio.h"

//#define SHOW_EXPECTED_BEHAVIOR

#ifdef SHOW_EXPECTED_BEHAVIOR
	#define eDeviceOther eDeviceC
#else
	#define eDeviceOther eDeviceB
#endif

/// General types
typedef unsigned char byte;

typedef enum {
	eMsgXPosition,
	eMsgYPosition,
	eMsgDiameter,
} tMessageId;

typedef struct {
	tMessageId nMsgId;
	unsigned int uiData;
} tMessage;

typedef enum {
	eDeviceA,
	eDeviceB,
	eDeviceC,
} tDeviceId;

bool PostMessage (tDeviceId nSender, tDeviceId nReceiver, tMessage *pMessage);

/// Debug console
void DebugOut(const char *szTemplate, ...)
{
	va_list ap;

	va_start (ap, szTemplate);
	vprintf (szTemplate, ap);
	va_end (ap);
}

/// Code to be tested
bool UpdateDiameter(unsigned int uiDia)
{
	tMessage sMessage;
	sMessage.nMsgId = eMsgDiameter;
	sMessage.uiData = uiDia;

	return PostMessage( eDeviceA, eDeviceB, &sMessage );
}

bool UpdateXPosition(unsigned int uiX)
{
	tMessage sMessage;
	sMessage.nMsgId = eMsgXPosition;
	sMessage.uiData = uiX;

	return PostMessage( eDeviceA, eDeviceOther, &sMessage );
}

bool CodeUnderTest(void) {
	bool bSuccess = 1;

	if( !UpdateDiameter(42) )
		bSuccess = 0;

	if( !UpdateXPosition(220) )
		bSuccess = 0;

	return bSuccess;
}


/// Mock
bool PostMessage (tDeviceId nSender, tDeviceId nReceiver, tMessage *pMessage)
{
	DebugOut("PostMessage - %p\n",pMessage);

	mock().actualCall("PostMessage")
		  .withParameter("nSender", nSender)
		  .withParameter("nReceiver", nReceiver)
		  .withParameterOfType("tMessage", "pMessage", pMessage);

	return (bool) mock().boolReturnValue();
}

/// Test
TEST_GROUP(OrderTest) {
	void setup()
	{
		mock().strictOrder();
	}
};

TEST(OrderTest, SimilarMockCallsInARow) {

	tMessage oDiameter = { .nMsgId = eMsgDiameter, .uiData = 42 };

	mock().expectOneCall("PostMessage")
		  .withParameter("nSender", eDeviceA)
		  .withParameter("nReceiver", eDeviceB)
		  .withParameterOfType("tMessage", "pMessage", &oDiameter)
		  .andReturnValue((bool) 1);

	tMessage oXPosition = { .nMsgId = eMsgXPosition, .uiData = 220 };

	mock().expectOneCall("PostMessage")
		  .withParameter("nSender", eDeviceA)
		  .withParameter("nReceiver", eDeviceOther)
		  .withParameterOfType("tMessage", "pMessage", &oXPosition)
		  .andReturnValue((bool) 1);

	CHECK_EQUAL(1, CodeUnderTest());
}

/// Custom comparator
class MessageComparator : public MockNamedValueComparator
{
	virtual bool isEqual(const void* oExpected, const void* oPosted)
	{
		bool bSuccess = 1;

		DebugOut("\tMessageComparator - oExpected=%p, oPosted=%p\n", oExpected, oPosted );

		/* on different pointer do custom check is desired */
		tMessage *psGeometryExpected = (tMessage*)oExpected;
		tMessage *psGeometryPosted = (tMessage*)oPosted;

		if( psGeometryExpected->nMsgId != psGeometryPosted->nMsgId )
			bSuccess = 0;
		if( psGeometryExpected->uiData != psGeometryPosted->uiData )
			bSuccess = 0;

		const char *szMsgId[] = { "eMsgXPosition", "eMsgYPosition", "eMsgDiameter" };
		const char *szSuccess[] = { "FAIL", "PASS" };

		DebugOut("\t\tID expected=%s, posted=%s -> %s\n", szMsgId[psGeometryExpected->nMsgId], szMsgId[psGeometryPosted->nMsgId], szSuccess[bSuccess] );

		return bSuccess;
	}
    virtual SimpleString valueToString(const void* object) {
        return StringFrom((void*)object);
    }
};

/// CppUTest main function

int main(int ac, char** av)
{
    MockSupportPlugin mockPlugin;
    MessageComparator comparator;
    mockPlugin.installComparator("tMessage", comparator);
    TestRegistry::getCurrentRegistry()->installPlugin(&mockPlugin);
    return RUN_ALL_TESTS(ac, av);
}


