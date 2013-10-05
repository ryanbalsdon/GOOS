#ifndef SAND_MutablePointerArray_h
#define SAND_MutablePointerArray_h
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

FYI: This source was released originally as GPLv3 for the SAND project. If 
you happen to have an older version, you're welcome to use whichever license
is least restrictive.
*****************************************************************************/


/*
 MutablePointerArray
 Mutable Array of Pointers
 
 This class is meant to be a simple C replacement of NSMutableArray. It is far from as-capable but does the job.
 Right now, this is an array that re-sizes itself as-needed. It was intended to be an array-list hybrid but this 
 works pretty well.
 */


typedef struct {
    void ** Array;           //The actual array
    int ArrayAllocationSize;//How large the internal array is. This is always as-large or larger than ArraySize.
    int ArraySize;          //How many objects the user has added to the array.
}MutablePointerArray;

//Init sets up default values. Free de-allocates the array and frees itself. The objects pointed to by the array entries
//are not automatically freed!
void InitMutablePointerArray(MutablePointerArray* self);
void FreeMutablePointerArray(MutablePointerArray* self);

//Calls free() on the pointer and removes it from the array
void MPA_FreeIndex(MutablePointerArray* self, int index);

//Calls free() on every pointer and removes them all from the array
void MPA_FreeAll(MutablePointerArray* self);
void MPA_RemoveAll(MutablePointerArray* self);


//Adds a new object to the list
int MPA_AddPointer(MutablePointerArray* self, void* pointer);

//Removes an object by pointer. This first searches the array for a matching pointer.
int MPA_RemovePointer(MutablePointerArray* self, void* pointer);
int MPA_FreePointer(MutablePointerArray* self, void* pointer);

int MPA_IndexOfPointer(MutablePointerArray* self, void* pointer);

//Removes an object at a specific index
int MPA_RemoveIndex(MutablePointerArray* self, int index);

//Gets the pointer at this index. If inlined, it should be just as fast as a single de-reference.
void* MPA_PointerAtIndex(MutablePointerArray* self, int index);
void* MPA_LastPointer(MutablePointerArray* self);


//Returns the length of the array
int MPA_GetSize(MutablePointerArray* self);
int MPA_Length(MutablePointerArray* self);

void MPA_Pop(MutablePointerArray* self);
int MPA_Swap(MutablePointerArray* self, int indexA, int indexB);



#endif
