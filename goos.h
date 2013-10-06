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

/* goos_version
 *  Returns an int version number. 
 */
int goos_version(void);

/* goos_init
 *  Not currently used for anything and doesn't have to be run. Was originally
 * meant to be a place to setup a set of standard classes but the project never 
 * made it that far...
 */
void goos_init(void);

/* A goos_object is more of an instance/object than a real class. Because
 *  the implementation details of a class and an object would be identical,
 *  there's no need to separate them out. An instance of a class can be
 *  created by sub-classing or inhereting from the class. This approach lets
 *  us modify class definitions at run-time and have all existing instances 
 *  use the modifcations.
*/
struct goos_object_s;
typedef struct goos_object_s goos_object;
struct goos_dispatcher_s;
typedef struct goos_dispatcher_s goos_dispatcher;

/* These error codes are returned in goos_data.e */
typedef enum {
	goos_errorCode_HAPPY,
	goos_errorCode_NO_HANDLE,
	goos_errorCode_NOT_A_METHOD,
	goos_errorCode_NOT_A_VARIABLE,
	goos_errorCode_NOT_VALID_DATA,
	goos_errorCode_WTF
} goos_errorCode;

/* A quick and dirty way to handle return codes without pointers (and 
 *  allocating mem) in a system without any sort of garbage collection.
 *  All metods are expected to return one of these but can return the
 *  global goos_returnObject_Nil if the method doesn't have to return
 *  anything.
*/
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

/* All methods must conform to this prototype! The first argument is the
 *  calling class. The second object is a void* that can be cast to
 *  whatever you need. The return is a goos_data struct.
*/
typedef goos_data (*goos_method)(goos_object*, void*);

/* The actuall object/class struct is very simple. A single baseClass
 *  means that multiple inheritance is not supported. All of the instance
 *  methods and variables are saved in dispatch.
*/
struct goos_dispatcher_s {
	MutablePointerArray array; /* This is a C array with some convenience functions */
};
struct goos_object_s {
	goos_object* baseClass;
	goos_dispatcher* dispatch;
};

/* goos_object_new
 *  Allocates a new object and returns it
 */
goos_object* goos_object_new(void);

/* goos_object_delete
 *  De-allocates the object
 */
void goos_object_delete(goos_object* class);

/* goos_object_addMethod
 *  Defines/adds an instance method if this is a class instance or a class
 * method if this is a class.
 */
void goos_object_addMethod(goos_object* class, goos_method method, const char* methodHandle);

/* goos_object_addData
 *  Defines/adds an instance variable if this is a class instance or a class
 * variable if this is a class.
 */
void goos_object_addData(goos_object* class, goos_data data, const char* methodHandle);

/* goos_object_removeHandle
 *  Undefines/removes a handle (either method or a variable) from the object. This
 * will not affect the baseClasses if the handle is defined for them too.
 */
int goos_object_removeHandle(goos_object* class, const char* handle);

/* goos_object_call
 *  Calls a method based on handle. If the method doesn't exist in the
 * object, it will try calling it in the baseClass. If the method doesn't exist
 * in the tree, an error is returned.
 */
goos_data goos_object_call(goos_object* class, const char* handle, void* arg);

/* goos_object_callSuper
 *  Similar to goos_object_call but it will start looking for the method in the
 * baseClass
 */
goos_data goos_object_callSuper(goos_object* class, const char* handle, goos_object* callee, void* arg);

/* goos_object_respondsTo
 *  If it returns Nil, the handle doesn't exist. If it does exist, this function
 * return a pointer to the goos_object it belongs to.
 */
goos_object* goos_object_respondsTo(goos_object* class, const char* handle);

/* goos_object_get
 *  Returns the value of a variable, based on handle. If the variable doesn't exist in the
 * object, it will try getting it from the baseClass. If the variable doesn't exist
 * in the tree, an error is returned.
 */
goos_data goos_object_get(goos_object* class, const char* handle);

/* goos_object_set
 *  Sets the value of a variable, based on handle. If the variable doesn't exist in the
 * object, it will try setting it from the baseClass. If the variable doesn't exist
 * in the tree, an error is returned.
 */
goos_errorCode goos_object_set(goos_object* class, const char* handle, goos_data data);

/* goos_object_inherits
 *  Sets the object's baseClass. If this is a class instance, pass it the object's
 * class. If this is a class, pass it the class you would like to inherit from.
 */
void goos_object_inherits(goos_object* class, goos_object* baseClass);



#endif