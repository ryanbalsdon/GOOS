/*****************************************************************************
GOOS - Duck-typing for C
Written in 2012 by Ryan Balsdon ryanbalsdon@gmail.com
To the extent possible under law, the author(s) have dedicated 
all copyright and related and neighboring rights to this software 
to the public domain worldwide. This software is distributed 
without any warranty.
You should have received a copy of the CC0 Public Domain Dedication 
along with this software. If not, see 
<http://creativecommons.org/publicdomain/zero/1.0/>.
*****************************************************************************/


#include "goos.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

int methodTestSuccess = 0;
goos_data methodTest1(goos_object* self, void* args) {
	printf("methodTest1\n");
	methodTestSuccess = 1;
	
	return goos_returnObject_Nil;
}
goos_data methodTest2(goos_object* self, void* args) {
	printf("methodTest2\n");
	methodTestSuccess = 2;
	
	return goos_returnObject_Nil;
}
goos_data methodTestIO(goos_object* self, void* args) {
	printf("methodTestIO\n");
	goos_data output;
	int input = *(int*)args;
	
	output.d=input*2;
	
	return output;
}

void testDispatch(void) {
	printf("Testing goos_dispatcher add and call\n");
	goos_dispatcher* testDispatch =  goos_dispatcher_new();
	goos_dispatcher_addMethod(testDispatch, methodTest1, "methodTest1");
	methodTestSuccess = 0;
	goos_dispatcher_call(testDispatch, "methodTest1", nil, nil);
	if (methodTestSuccess == 1) printf("Success\n");
	else { printf("Failure\n"); exit(0); }
	
	printf("Testing goos_dispatcher_call on bad methodName\n");
	if (goos_dispatcher_call(testDispatch, "NoMethod", nil, nil).e == goos_errorCode_NO_HANDLE) printf("Success\n");
	else { printf("Failure\n"); exit(0); }
	
	printf("Testing goos_dispatcher with second method\n");
	goos_dispatcher_addMethod(testDispatch, methodTest2, "methodTest2");
	methodTestSuccess = 0;
	goos_dispatcher_call(testDispatch, "methodTest2", nil, nil);
	if (methodTestSuccess == 2) printf("Success\n");
	else { printf("Failure\n"); exit(0); }
	
	printf("Double-checking the first method again...\n");
	methodTestSuccess = 0;
	goos_dispatcher_call(testDispatch, "methodTest1", nil, nil);
	if (methodTestSuccess == 1) printf("Success\n");
	else { printf("Failure\n"); exit(0); }
	
	printf("Testing input/output\n");
	goos_dispatcher_addMethod(testDispatch, methodTestIO, "methodTestIO");
	int input = 21;
	if (goos_dispatcher_call(testDispatch, "methodTestIO", nil, (void*)&input).d == 42) printf("Success\n");
	else { printf("Failure\n"); exit(0); }
	
	printf("Testing method removal\n");
	goos_dispatcher_removeHandle(testDispatch, "methodTest2");
	if (goos_dispatcher_call(testDispatch, "methodTest2", nil, nil).e == goos_errorCode_NO_HANDLE) printf("Success\n");
	else { printf("Failure\n"); exit(0); }
	
	printf("Double-checking the first method again...\n");
	methodTestSuccess = 0;
	goos_dispatcher_call(testDispatch, "methodTest1", nil, nil);
	if (methodTestSuccess == 1) printf("Success\n");
	else { printf("Failure\n"); exit(0); }
	
	printf("...and the third one\n");
	goos_dispatcher_addMethod(testDispatch, methodTestIO, "methodTestIO");
	if (goos_dispatcher_call(testDispatch, "methodTestIO", nil, (void*)&input).d == 42) printf("Success\n");
	else { printf("Failure\n"); exit(0); }
	
	printf("Testing pointer variables\n");
	int life = 100;
	goos_data dLife;
	dLife.pd = &life;
	goos_dispatcher_addData(testDispatch, dLife, "life");
	if (*(goos_dispatcher_get(testDispatch, "life").pd) == 100) printf("Success\n");
	else { printf("Failure\n"); exit(0); }
	
	printf("Modify pointer\n");
	life = 80;
	if (*(goos_dispatcher_get(testDispatch, "life").pd) == 80) printf("Success\n");
	else { printf("Failure\n"); exit(0); }
	
	printf("Testing instance variables\n");
	int maxLife = 20;
	goos_data dmLife;
	dmLife.d = maxLife;
	goos_dispatcher_addData(testDispatch, dmLife, "maxLife");
	if (goos_dispatcher_get(testDispatch, "maxLife").d == 20) printf("Success\n");
	else { printf("Failure\n"); exit(0); }
	
	printf("Modify original but instance stays\n");
	maxLife = 0;
	dmLife.d = 0;
	if (goos_dispatcher_get(testDispatch, "maxLife").d == 20) printf("Success\n");
	else { printf("Failure\n"); exit(0); }
	
	printf("Modify instance variable\n");
	dmLife.d = 10;
	goos_dispatcher_set(testDispatch, "maxLife", dmLife);
	if (goos_dispatcher_get(testDispatch, "maxLife").d == 10) printf("Success\n");
	else { printf("Failure\n"); exit(0); }
	
	
	printf("Access method as data\n");
	if (goos_dispatcher_get(testDispatch, "methodTestIO").e == goos_errorCode_NOT_A_VARIABLE) printf("Success\n");
	else { printf("Failure\n"); exit(0); }
	
	printf("Access data as method\n");
	if (goos_dispatcher_call(testDispatch, "maxLife", nil, nil).e == goos_errorCode_NOT_A_METHOD) printf("Success\n");
	else { printf("Failure\n"); exit(0); }
}

int greetSimple_wasRun = 0;
goos_data classHello_greetSimple(goos_object* self, void* args) {
	greetSimple_wasRun = 1;
	goos_data output;
	output.s = "Hello!";
	return output;
}

int greetAwesome_wasRun = 0;
goos_data classHello_greetAwesome(goos_object* self, void* args) {
	for (int i=0; i<goos_object_get(self, "awesomeness").d; i++) {
		printf("Hello %s!\n", goos_object_get(self, "name").s);
	}
	
	greetAwesome_wasRun = goos_object_get(self, "awesomeness").d;	
	goos_data output;
	return output;
}

int greetLevel = 0;
goos_data classSecondHello_greetHi(goos_object* self, void* args) {
	greetLevel = goos_object_get(self, "height").d;
	return goos_returnObject_Nil;
}

void testClass(void) {
	goos_object* classHello = goos_object_new();
	
	printf("Testing class functions\n");
	goos_object_addMethod(classHello, classHello_greetSimple, "greetSimple");
	greetSimple_wasRun = 0;
	printf("%s\n", goos_object_call(classHello, "greetSimple", nil).s);
	if (greetSimple_wasRun == 1) printf("Success\n");
	else { printf("Failure\n"); exit(0); }
	
	printf("Testing class vars\n");
	goos_data sAwesome; sAwesome.s = "Awesome";
	goos_object_addData(classHello, sAwesome, "name");
	char* sName = goos_object_get(classHello, "name").s;
	printf("You are %s\n", sName);
	if (strcmp("Awesome", sName) == 0) printf("Success\n");
	else { printf("Failure\n"); exit(0); }
	
	printf("Testing class vars in a method\n");
	goos_data dMultiplier; dMultiplier.d=5;
	goos_object_addData(classHello, dMultiplier, "awesomeness");
	goos_object_addMethod(classHello, classHello_greetAwesome, "greetMuch");
	goos_object_call(classHello, "greetMuch", nil);
	if (greetAwesome_wasRun == 5) printf("Success\n");
	else { printf("Failure\n"); exit(0); }
	
	printf("Testing subclassing\n");
	goos_object* classSecondHello = goos_object_new();
	goos_object_inherits(classSecondHello, classHello);
	greetSimple_wasRun = 0;
	printf("%s\n", goos_object_call(classSecondHello, "greetSimple", nil).s);
	if (greetSimple_wasRun == 1) printf("Success\n");
	else { printf("Failure\n"); exit(0); }
	
	printf("Testing super's methods use self's data\n");
	goos_object_addMethod(classHello, classSecondHello_greetHi, "greetHi");
	goos_data dHeight; dHeight.d=12000;
	goos_object_addData(classSecondHello, dHeight, "height");
	dHeight.d=12;
	goos_object_addData(classHello, dHeight, "height");
	goos_object_call(classSecondHello, "greetHi", nil);
	if (greetLevel == 12000) printf("Success\n");
	else { printf("Failure\n"); exit(0); }
	goos_object_call(classHello, "greetHi", nil);
	if (greetLevel == 12) printf("Success\n");
	else { printf("Failure\n"); exit(0); }
}

int main(void) {
	printf("Testing goos Version %d\n", goos_version());
	
	testDispatch();
	testClass();
	
	printf("All tests passed\n");
}