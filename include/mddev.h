#ifndef MDDEV_H
#define MDDEV_H

#include <psp2kern/kernel/threadmgr/fast_mutex.h>
#include <psp2kern/kernel/debug.h>

#define MDDEV_STAT_DISABLED  (1)
#define MDDEV_STAT_ENABLED   (2)

#define MDDEV_SECTOR_SHIFT (9)
#define MDDEV_SECTOR_SIZE  (1 << MDDEV_SECTOR_SHIFT)
#define MDDEV_SECTOR_MASK  (MDDEV_SECTOR_SIZE - 1)

#define MDDEV_STAT_IS_VALID(dev) ((dev->stat == MDDEV_STAT_DISABLED) || (dev->stat == MDDEV_STAT_ENABLED))

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

mddev_object *mddev_get_object(SceUInt index);

static inline int mddev_lock(mddev_object *dev)
{
    return ksceKernelLockFastMutex(&dev->fastMutex);
}

static inline int mddev_unlock(mddev_object *dev)
{
    return ksceKernelUnlockFastMutex(&dev->fastMutex);
}

int mddev_get_stat(mddev_object *dev);
int mddev_mkdev(SceUID block);
int mddev_rmdev(mddev_object *dev);

SceSSize mddev_read(mddev_object *dev, void *buf, SceUInt sector, SceSize nSectors);
SceSSize mddev_write(mddev_object *dev, const void *buf, SceUInt sector, SceSize nSectors);

#endif