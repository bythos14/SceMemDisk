#ifndef MEMDISK_H
#define MEMDISK_H

#include <psp2common/types.h>

#define SCE_MEMDISK_DEVCTL_ENABLE (0x1)

typedef struct SceMemDiskEnableArg {
	SceUID block; //!< MemBlock to use as storage for the MemDisk
} SceMemDiskEnableArg;

typedef struct SceMemDiskEnableResult {
	SceInt index; //!< Index of the MemDisk file on the mountpoint
} SceMemDiskEnableResult;

#endif
