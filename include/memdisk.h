#ifndef MEMDISK_H
#define MEMDISK_H

#include <psp2common/types.h>

#define SCE_MEMDISK_DEVCTL_ENABLE (0x10) // Originally 0x1 in firmware 0.990, but that devctl no. is now in use by Sdstor, so we use 0x10

typedef struct SceMemDiskEnableArg {
	SceUID block; //!< MemBlock to use as storage for the MemDisk
} SceMemDiskEnableArg;

typedef struct SceMemDiskEnableResult {
	SceInt index; //!< Index of the MemDisk file on the mountpoint
} SceMemDiskEnableResult;

#endif
