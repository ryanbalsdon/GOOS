#ifndef goos_h
#define goos_h

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

/*
 GOOS
 Duck-typing lib for C
 
 This lib was originally put together to port some Python code to C. Taking
approach is a very, very bad idea! It's much easier (and maintainable) to
just use standard C techniques instead of trying to shoehorn in interpreted
launguage approaches. I haven't yet found a use for this code but if you do,
please let me know!
 */


#include "MutablePointerArray.h"

#ifndef nil
#define nil (void*)0
#endif

int goos_version(void);
void goos_init(void);

struct goos_object_s;
typedef struct goos_object_s goos_object;

typedef enum {
	goos_errorCode_HAPPY,
	goos_errorCode_NO_HANDLE,
	goos_errorCode_NOT_A_METHOD,
	goos_errorCode_NOT_A_VARIABLE,
	goos_errorCode_NOT_VALID_DATA,
	goos_errorCode_WTF
} goos_errorCode;
typedef struct {
	union {
		goos_object* o;
		int d;
		float f;
		void* p;
		char c;
		char* s;
		int* pd;
		float* pf;
	};
	goos_errorCode e;
} goos_data;
extern goos_data goos_returnObject_Nil;

typedef goos_data (*goos_method)(goos_object*, void*);

typedef struct goos_dispatcher_s {
	MutablePointerArray array;
} goos_dispatcher;

goos_dispatcher* goos_dispatcher_new(void);
void goos_dispatcher_delete(goos_dispatcher* dispatcher);
void goos_dispatcher_addMethod(goos_dispatcher* dispatcher, goos_method method, const char* methodHandle);
void goos_dispatcher_addData(goos_dispatcher* dispatcher, goos_data data, const char* methodHandle);
int goos_dispatcher_removeHandle(goos_dispatcher* dispatcher, const char* handle);
goos_data goos_dispatcher_call(goos_dispatcher* dispatcher, const char* handle, goos_object* callee, void* arg);
int goos_dispatcher_find(goos_dispatcher* dispatcher, const char* handle);
goos_data goos_dispatcher_get(goos_dispatcher* dispatcher, const char* handle);
goos_errorCode goos_dispatcher_set(goos_dispatcher* dispatcher, const char* handle, goos_data data);


struct goos_object_s {
	goos_object* baseClass;
	goos_dispatcher* dispatch;
};
goos_object* goos_object_new(void);
void goos_object_delete(goos_object* class);
void goos_object_addMethod(goos_object* class, goos_method method, const char* methodHandle);
void goos_object_addData(goos_object* class, goos_data data, const char* methodHandle);
int goos_object_removeHandle(goos_object* class, const char* handle);
goos_data goos_object_call(goos_object* class, const char* handle, void* arg);
goos_data goos_object_callSuper(goos_object* class, const char* handle, goos_object* callee, void* arg);
goos_object* goos_object_respondsTo(goos_object* class, const char* handle);
goos_data goos_object_get(goos_object* class, const char* handle);
goos_errorCode goos_object_set(goos_object* class, const char* handle, goos_data data);
void goos_object_inherits(goos_object* class, goos_object* baseClass);


#endif