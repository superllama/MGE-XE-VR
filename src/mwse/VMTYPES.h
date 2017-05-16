//VmTypes.h
#pragma once

#include <wtypes.h>

typedef BYTE* VPVOID;       // so I can do arithmetic
typedef DWORD VMSIZE;       // to hold max address
typedef WORD OPCODE;        // any consistent size
typedef long VMREGTYPE;     // as large as the largest type
typedef short VMINTERRUPT;  // how many interrupts do you need?
typedef float VMFLOAT;      // make sure this is the same size as STDSTACKTYPE, 32 bits
typedef char VMBYTE;        // any consistent size, pref 8 bits
typedef short VMSHORT;      // any consistent size
typedef long VMLONG;        // any consistent size > VMSHORT
typedef DWORD VMFLAGSTYPE;  // how many flags do you need?

// 'ui32' is the pre-Visual C++ 2005 suffix, equivalent to 'll'.
#define VMSIZE_MAX (1ui64<<32)
#define VMBYTE_MAX (1<<8)
#define VMSHORT_MAX (1<<16)
#define VMLONG_MAX (1ui64<<32)

struct MWRecord
{
    const void **vtbl;
    char tag[4];
    unsigned int flags;
    void *sourceMod;
};

struct MWReference
{
    const void **vtbl;
    char tag[4];
    unsigned int flags;
    void *sourceMod;
    void *visual;
    void *cellList;
    MWReference *lastCloneRefr;
    MWReference *nextListRefr;
    MWReference *prevListRefr;
    void *unknown;
    MWRecord *baseEntity;
    float orientation[3];
    float position[3];
    void *extraData;
    unsigned int sourceID, targetID;
};
