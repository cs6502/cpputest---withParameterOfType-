# cpputest---withParameterOfType-
Strange behavior with custom parameter check using withParameterOfType

I have put an example together that should
run out of the box for everyone who compiles it in his CppUTest Suite as main module
without any other stuff needed. There is console output for explanation. The output
from my run is also attached below.

The example is showing the unexpected as well as the expected behavior. 
The function CodeUnderTest() is doing 2 calls in a row to function PostMessage(). 
Therefore the TEST() pushes 2 times the expectations for the details of those
calls to the mock() plugin stack. The plugin is set to "strict order".
The parameters pushed to PostMessage() differ from first to second call just 
on the third parameter where the custom check should be done with. 
But already on the first call to PostMessage() the 
MessageComparator() is called twice from the MockPlugin: once with the expected
value from the first call and the second time with the expected value from
the second call. The potential bug is that the MessageComparator() is called twice
after the first call of PostMessage(). From my point of view it should be called
just once.
Because, If there is a difference in one of the other parameters of PostMessage() it is 
only called once as I would expect. This expected behavior will be shown by uncommenting 
the define SHOW_EXPECTED_BEHAVIOR. Even both scenarios run with OK this might become
an issue for more complex custom comparators.

Suspected console output:
PostMessage - 0069faa8
	MessageComparator - oExpected=0069fb30, oPosted=0069faa8
		ID expected=eMsgDiameter, posted=eMsgDiameter -> PASS
	MessageComparator - oExpected=0069fb28, oPosted=0069faa8
		ID expected=eMsgXPosition, posted=eMsgDiameter -> FAIL
PostMessage - 0069faa8
	MessageComparator - oExpected=0069fb28, oPosted=0069faa8
		ID expected=eMsgXPosition, posted=eMsgXPosition -> PASS
OK (1 tests, 1 ran, 5 checks, 0 ignored, 0 filtered out, 1 ms)

Expected console output: (with define SHOW_EXPECTED_BEHAVIOR)
PostMessage - 0069faa8
	MessageComparator - oExpected=0069fb30, oPosted=0069faa8
		ID expected=eMsgDiameter, posted=eMsgDiameter -> PASS
PostMessage - 0069faa8
	MessageComparator - oExpected=0069fb28, oPosted=0069faa8
		ID expected=eMsgXPosition, posted=eMsgXPosition -> PASS
OK (1 tests, 1 ran, 5 checks, 0 ignored, 0 filtered out, 0 ms)
