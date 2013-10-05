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


#include "MutablePointerArray.h"
#include <stdlib.h>
#include <stdio.h>


void InitMutablePointerArray(MutablePointerArray* self) {
    //default to 128 entries
    self->Array = (void**)malloc(128*sizeof(void*));
    self->ArrayAllocationSize = 128;
    self->ArraySize = 0;
    
}
void FreeMutablePointerArray(MutablePointerArray* self) {
    //Should this also free the managed pointers?
    //No--they may need custom free routines
    free(self->Array);
    self->ArrayAllocationSize = 0;
    self->ArraySize = 0;
}

inline int MPA_GetSize(MutablePointerArray* self) {
    return self->ArraySize;
}

inline int MPA_Length(MutablePointerArray* self) {
    return self->ArraySize;
}


static void MPA_IncreaseAllocation(MutablePointerArray* self) {
    //double allocation
    void ** newArray = (void**)malloc(2*self->ArrayAllocationSize*sizeof(void*));
    for (int i=0; i<self->ArrayAllocationSize; i++) {
        newArray[i] = self->Array[i];
    }
    free(self->Array);
    self->Array = newArray;
    self->ArrayAllocationSize *= 2;
}

int MPA_AddPointer(MutablePointerArray* self, void* pointer) {
    if (self->ArrayAllocationSize <= self->ArraySize)
        MPA_IncreaseAllocation(self);
    
    self->Array[self->ArraySize] = pointer;
    self->ArraySize++;
    
    return self->ArraySize-1;
}

inline void* MPA_PointerAtIndex(MutablePointerArray* self, int index) {
    return self->Array[index];
}

void* MPA_LastPointer(MutablePointerArray* self) {
    return MPA_PointerAtIndex(self, MPA_GetSize(self)-1);
}

int MPA_RemoveIndex(MutablePointerArray* self, int index) {
    //This does not free the memory!
    for (int i=index; i<self->ArraySize-1; i++) {
        self->Array[i] = self->Array[i+1];
    }
    self->ArraySize--;
    self->Array[self->ArraySize] = 0;
    
    return 1;
}

inline void MPA_FreeIndex(MutablePointerArray* self, int index) {
    void* pointer = MPA_PointerAtIndex(self, index);
    MPA_RemoveIndex(self, index);
    free(pointer);
}

void MPA_FreeAll(MutablePointerArray* self) {
    for (int i=MPA_GetSize(self)-1; i>=0; i--) {
        MPA_FreeIndex(self, i);
    }
}
inline void MPA_RemoveAll(MutablePointerArray* self) {
    while (MPA_GetSize(self) > 0) {
        MPA_Pop(self);
    }
}

int MPA_IndexOfPointer(MutablePointerArray* self, void* pointer) {
    for (int i=0; i<self->ArraySize; i++) {
        if (self->Array[i] == pointer)
            return i;
    }
    return -1;
}

int MPA_RemovePointer(MutablePointerArray* self, void* pointer) {
    //This does not free the memory!
    int i;
    for (i=0; i<self->ArraySize; i++) {
        if (self->Array[i] == pointer)
            break;
    }
    if (i >= self->ArraySize) return 0; //No pointer found
    
    MPA_RemoveIndex(self, i);
    
    //Recurse incase there are more copies
    MPA_RemovePointer(self, pointer);
    
    return 1;
}

int MPA_FreePointer(MutablePointerArray* self, void* pointer) {
    int index = MPA_IndexOfPointer(self, pointer);
    if (index < 0) {
        printf("ERROR: Pointer not found in Array\n");
        return 0;
    }
    MPA_FreeIndex(self, index);
    return 1;
}

void MPA_Pop(MutablePointerArray* self) {
    MPA_RemoveIndex(self, MPA_GetSize(self)-1);
}

int MPA_Swap(MutablePointerArray* self, int indexA, int indexB) {
    if (indexA > self->ArraySize || indexB > self->ArraySize)
        return 0;
    void* temp = self->Array[indexA];
    self->Array[indexA] = self->Array[indexB];
    self->Array[indexB] = temp;
    return 1;
}



