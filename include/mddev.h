#ifndef MDDEV_H
#define MDDEV_H

#include <psp2kern/kernel/threadmgr/fast_mutex.h>

#define MDDEV_STAT_UNMAPPED (1)
#define MDDEV_STAT_MAPPED   (2)

typedef struct mddev_object {
    SceKernelFastMutex fastMutex;
    SceUID userBlock;
    SceUID kernelBlock;
    SceSize blockSize;
    void *base;
    SceUInt unk;
    SceUInt16 unk2;
    SceUInt8 stat;
} mddev_object;

int mddev_initialize();

#endif