#include "mddev.h"

#include <psp2kern/kernel/sysmem.h>

#include <string.h>

static mddev_object devs[2];

mddev_object *mddev_get_object(SceUInt index)
{
    return &devs[index];
}

int mddev_get_stat(mddev_object *dev)
{
    int ret;
    if ((ret = mddev_lock(dev)) < 0) {
        return ret;
    }

    int stat = dev->stat;

    mddev_unlock(dev);
    
    return stat;
}

static int mddev_init(mddev_object *dev)
{
    int ret;
    if ((ret = mddev_lock(dev)) < 0)
        return ret;

    dev->userBlock = 0;
    dev->kernelBlock = 0;
    dev->unk2 = 0;
    dev->blockSize = 0;
    dev->base = NULL;
    dev->stat = MDDEV_STAT_DISABLED;
    dev->unk = 0;

    mddev_unlock(dev);

    return 0;
}

static void mddev_destruct(mddev_object *dev)
{
    ksceKernelFinalizeFastMutex(&dev->fastMutex);
}

static int mddev_construct(mddev_object *dev)
{
    int ret;
    
    if ((ret = ksceKernelInitializeFastMutex(&dev->fastMutex, "SceMemDiskMdDev", 0, NULL)) < 0) {
        return ret;
    }

    if ((ret = mddev_init(dev)) < 0) {
        mddev_destruct(dev);
    }

    return 0;
}

static int mddev_enable(mddev_object *dev, SceUID block)
{
    int ret;
    if ((ret = mddev_lock(dev)) < 0) {
        return ret;
    }

    if (!MDDEV_STAT_IS_VALID(dev)) {
        ret = 0x80010013;
        goto exit;
    }

    if (dev->stat == MDDEV_STAT_ENABLED) {
        ret = 0x80010010;
        goto exit;
    }

    SceSize blockSize; 
    if ((ret = ksceKernelGetMemBlockAllocMapSize(block, &blockSize)) < 0) {
        goto exit;
    }

    if ((blockSize & MDDEV_SECTOR_MASK) != 0) {
        ret = 0x80010022;
        goto exit;
    }

    SceKernelAllocMemBlockKernelOpt opt = {0};
    opt.size = sizeof(opt);
    opt.attr = SCE_KERNEL_ALLOC_MEMBLOCK_ATTR_HAS_MIRROR_BLOCKID | 0x1000000;
    opt.mirror_blockid = block;

    SceUID kBlock = ksceKernelAllocMemBlock("SceMemDisk", SCE_KERNEL_MEMBLOCK_TYPE_KERNEL_ROOT_RW, blockSize, &opt);
    if (kBlock < 0) {
        ret = kBlock;
        goto exit;
    }

    void *base;
    if ((ret = ksceKernelGetMemBlockBase(kBlock, &base)) < 0) {
        ksceKernelFreeMemBlock(kBlock);
        goto exit;
    }

    dev->base = base;
    dev->kernelBlock = kBlock;
    dev->userBlock = block;
    dev->blockSize = blockSize;
    dev->stat = MDDEV_STAT_ENABLED;

exit:
    mddev_unlock(dev);
    return ret;
}

static int mddev_disable(mddev_object *dev)
{
    int ret;
    if ((ret = mddev_lock(dev)) < 0) {
        return ret;
    }

    if (!MDDEV_STAT_IS_VALID(dev)) {
        ret = 0x80010013;
        goto exit;
    }

    if (dev->kernelBlock == 0) {
        ret = 0;
        goto exit;
    }

    ksceKernelFreeMemBlock(dev->kernelBlock);

    dev->blockSize = 0;
    dev->kernelBlock = 0;
    dev->userBlock = 0;
    dev->base = NULL;
    dev->stat = MDDEV_STAT_DISABLED;
    dev->unk = 0;
    dev->unk2 = 0;

exit:
    mddev_unlock(dev);
    return ret;
}

int mddev_mkdev(SceUID block)
{
    int devIndex = 0;
    mddev_object *dev = mddev_get_object(0);
    int ret = mddev_get_stat(dev);
    if (ret < 0) {
        return ret;
    }

    if (ret == MDDEV_STAT_ENABLED) {
        devIndex = 1;
        dev = mddev_get_object(1);
        ret = mddev_get_stat(dev);
        if (ret < 0) {
            return ret;
        }

        if (ret == MDDEV_STAT_ENABLED) {
            return 0x80010018;
        }
    }

    if ((ret = mddev_enable(dev, block)) < 0) {
        return ret;
    }

    return devIndex;
}

int mddev_rmdev(mddev_object *dev) 
{
    return mddev_disable(dev);
}

SceSSize mddev_read(mddev_object *dev, void *buf, SceUInt sector, SceSize nSectors)
{
    SceSSize ret;
    if ((ret = mddev_lock(dev)) < 0) {
        return ret;
    }

    if (dev->stat != MDDEV_STAT_ENABLED) {
        ret = 0x80010013;
        goto exit;
    }

    SceSize nBytes = nSectors * MDDEV_SECTOR_SIZE;
    if (nBytes == 0) {
        ret = 0;
        goto exit;
    }

    SceUInt offset = sector * MDDEV_SECTOR_SIZE;

    if ((offset + nBytes) > dev->blockSize) {
        ret = 0x80010021;
        goto exit;
    }

    memcpy(buf, dev->base + offset, nBytes);
    ret = nBytes;

exit:
    mddev_unlock(dev);
    return ret;
}

SceSSize mddev_write(mddev_object *dev, const void *buf, SceUInt sector, SceSize nSectors)
{
    SceSSize ret;
    if ((ret = mddev_lock(dev)) < 0) {
        return ret;
    }

    if (dev->stat != MDDEV_STAT_ENABLED) {
        ret = 0x80010013;
        goto exit;
    }

    SceSize nBytes = nSectors * MDDEV_SECTOR_SIZE;
    if (nBytes == 0) {
        ret = 0;
        goto exit;
    }

    SceUInt offset = sector * MDDEV_SECTOR_SIZE;

    if ((offset + nBytes) > dev->blockSize) {
        ret = 0x80010021;
        goto exit;
    }

    memcpy(dev->base + offset, buf, nBytes);
    ret = nBytes;

exit:
    mddev_unlock(dev);
    return ret;
}

int mddev_initialize() 
{
    int ret = 0;

    memset(&devs, 0, sizeof(devs));

    for (int i = 0; i < 2; i++) {
        mddev_object *dev = mddev_get_object(i);
        if ((ret = mddev_construct(dev)) < 0) {
            if (i == 1) 
                mddev_destruct(mddev_get_object(0));

            return ret;
        }
    }

    return ret;
}