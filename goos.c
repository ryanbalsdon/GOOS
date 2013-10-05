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


#include "goos.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"

int goos_version(void) {
	return 1;
}

goos_data goos_returnObject_Nil;

void goos_init(void) {
}

goos_dispatcher* goos_dispatcher_new(void) {
	goos_dispatcher* newDispatcher = (goos_dispatcher*)malloc(sizeof(goos_dispatcher));
	InitMutablePointerArray(&newDispatcher->array);
	
	return newDispatcher;
}

void goos_dispatcher_delete(goos_dispatcher* self) {
	FreeMutablePointerArray(&self->array);
	free(self);
}

/* hash is djb2. see http://www.cse.yorku.ca/~oz/hash.html */
unsigned long hash(const char* str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

#define goos_dispatcher_objectInfo_MAX_HANDLE 64
typedef struct {
	goos_method method;
	goos_data data;
	char handle[goos_dispatcher_objectInfo_MAX_HANDLE];
	unsigned long hash;
} goos_dispatcher_objectInfo;
void goos_dispatcher_objectInfo_init(goos_dispatcher_objectInfo* self) {
	memset(self, 0, sizeof(goos_dispatcher_objectInfo));
	self->data.e = goos_errorCode_NOT_VALID_DATA;
}
void goos_dispatcher_addMethod(goos_dispatcher* self, goos_method method, const char* handle) {
	goos_dispatcher_objectInfo* methodInfo = (goos_dispatcher_objectInfo*)malloc(sizeof(goos_dispatcher_objectInfo));
	goos_dispatcher_objectInfo_init(methodInfo);
	methodInfo->method=method;
	methodInfo->hash = hash(handle);
	strncpy(methodInfo->handle, handle, goos_dispatcher_objectInfo_MAX_HANDLE);
	MPA_AddPointer(&self->array, methodInfo);
}
void goos_dispatcher_addData(goos_dispatcher* self, goos_data data, const char* handle) {
	goos_dispatcher_objectInfo* objectInfo = (goos_dispatcher_objectInfo*)malloc(sizeof(goos_dispatcher_objectInfo));
	goos_dispatcher_objectInfo_init(objectInfo);
	objectInfo->data=data;
	strncpy(objectInfo->handle, handle, goos_dispatcher_objectInfo_MAX_HANDLE);
	MPA_AddPointer(&self->array, objectInfo);
}

int goos_dispatcher_removeHandle(goos_dispatcher* self, const char* methodHandle) {
	int removeCount = 0;
	int methodIndex;
	while ((methodIndex = goos_dispatcher_find(self, methodHandle)) >= 0) {
		MPA_RemoveIndex(&self->array, methodIndex);
	}
	return removeCount;
}

goos_data goos_dispatcher_call(goos_dispatcher* self, const char* handle, goos_object* callee, void* arg) {
	int methodIndex = goos_dispatcher_find(self, handle);
	if (methodIndex < 0 || methodIndex >= MPA_Length(&self->array)) {
		printf("ERROR: Handle not found in dispatcher: %s.\n", handle);
		goos_data error;
		error.e=goos_errorCode_NO_HANDLE;
		return error;
	}
	
	goos_dispatcher_objectInfo* methodInfo = (goos_dispatcher_objectInfo*)MPA_PointerAtIndex(&self->array, methodIndex);
	if (methodInfo->method == nil) {
		printf("ERROR: Accessing data handle as a method: %s\n", handle);
		goos_data error;
		error.e=goos_errorCode_NOT_A_METHOD;
		return error;
	}
	return methodInfo->method(callee, arg);	
}

goos_data goos_dispatcher_get(goos_dispatcher* self, const char* handle) {
	int dataIndex = goos_dispatcher_find(self, handle);
	if (dataIndex < 0 || dataIndex >= MPA_Length(&self->array)) {
		printf("ERROR: Handle not found in dispatcher: %s.\n", handle);
		goos_data error;
		error.e=goos_errorCode_NO_HANDLE;
		return error;
	}
	
	goos_dispatcher_objectInfo* objectInfo = (goos_dispatcher_objectInfo*)MPA_PointerAtIndex(&self->array, dataIndex);
	if (objectInfo->data.e == goos_errorCode_NOT_VALID_DATA) {
		printf("ERROR: Accessing method handle as data\n");
		goos_data error;
		error.e=goos_errorCode_NOT_A_VARIABLE;
		return error;
	}
	
	return objectInfo->data;	
}

goos_errorCode goos_dispatcher_set(goos_dispatcher* self, const char* handle, goos_data data) {
	int dataIndex = goos_dispatcher_find(self, handle);
	if (dataIndex < 0 || dataIndex >= MPA_Length(&self->array)) {
		printf("ERROR: Handle not found in dispatcher: %s.\n", handle);
		return goos_errorCode_NO_HANDLE;
	}
	
	goos_dispatcher_objectInfo* objectInfo = (goos_dispatcher_objectInfo*)MPA_PointerAtIndex(&self->array, dataIndex);
	if (objectInfo->data.e == goos_errorCode_NOT_VALID_DATA) {
		printf("ERROR: Accessing method handle as data: %s\n", handle);
		return goos_errorCode_NOT_A_VARIABLE;
	}
	
	objectInfo->data = data;
	return goos_errorCode_HAPPY;
}

int goos_dispatcher_find(goos_dispatcher* self, const char* handle) {
	unsigned long handleHash = hash(handle);
	for (int index=0; index<MPA_Length(&self->array); index++) {
		goos_dispatcher_objectInfo* info = (goos_dispatcher_objectInfo*)MPA_PointerAtIndex(&self->array, index);
		if (info->hash == handleHash && strncmp(info->handle, handle, goos_dispatcher_objectInfo_MAX_HANDLE) == 0) {
			return index;
		}
	}
	for (int index=0; index<MPA_Length(&self->array); index++) {
		goos_dispatcher_objectInfo* info = (goos_dispatcher_objectInfo*)MPA_PointerAtIndex(&self->array, index);
		if (strncmp(info->handle, handle, goos_dispatcher_objectInfo_MAX_HANDLE) == 0) {
			return index;
		}
	}
	return -1;
}

goos_object* goos_object_new(void) {
	goos_object* newClass = (goos_object*)malloc(sizeof(goos_object));
	newClass->dispatch = goos_dispatcher_new();
	
	return newClass;
}

void goos_object_delete(goos_object* class) {
	goos_dispatcher_delete(class->dispatch);
	free(class);
}

void goos_object_addMethod(goos_object* self, goos_method method, const char* methodHandle) {
	goos_dispatcher_addMethod(self->dispatch, method, methodHandle);
}
void goos_object_addData(goos_object* self, goos_data data, const char* methodHandle) {
	goos_dispatcher_addData(self->dispatch, data, methodHandle);
}
int goos_object_removeHandle(goos_object* self, const char* handle) {
	return goos_dispatcher_removeHandle(self->dispatch, handle);
}
goos_data goos_object_call(goos_object* self, const char* handle, void* arg) {
	goos_object* target = goos_object_respondsTo(self, handle);
	if (target == nil) {		
		printf("ERROR: Handle not found: %s\n", handle);
		goos_data error;
		error.e=goos_errorCode_NO_HANDLE;
		return error;
	}
	return goos_dispatcher_call(target->dispatch, handle, self, arg);
}
goos_data goos_object_callSuper(goos_object* self, const char* handle, goos_object* callee, void* arg) {
	if (self->baseClass == nil) {
		printf("ERROR: Trying to call super on a class without a base: %s\n", handle);
		goos_data error;
		error.e=goos_errorCode_WTF;
		return error;
	}
	return goos_object_call(self->baseClass, handle, arg);
}
goos_object* goos_object_respondsTo(goos_object* self, const char* handle) {
	if (goos_dispatcher_find(self->dispatch, handle) >=0) return self;
	if (self->baseClass != nil) return goos_object_respondsTo(self->baseClass, handle);
	return nil;
}
goos_data goos_object_get(goos_object* self, const char* handle) {
	goos_object* target = goos_object_respondsTo(self, handle);
	if (target == nil) {		
		printf("ERROR: Handle not found: %s\n", handle);
		goos_data error;
		error.e=goos_errorCode_NO_HANDLE;
		return error;
	}
	return goos_dispatcher_get(target->dispatch, handle);
}
goos_errorCode goos_object_set(goos_object* self, const char* handle, goos_data data) {
	goos_object* target = goos_object_respondsTo(self, handle);
	if (target == nil) {		
		printf("ERROR: Handle not found: %s\n", handle);
		return goos_errorCode_NO_HANDLE;
	}
	return goos_dispatcher_set(target->dispatch, handle, data);
}

void goos_object_inherits(goos_object* self, goos_object* baseClass) {
	if (self->baseClass != nil) {
		printf("ERROR: Trying to set a base class when one already exists!\n You should implement multiple inheritance?\n");
	}
	self->baseClass = baseClass;
}