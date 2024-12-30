#include <psp2kern/kernel/modulemgr.h>
#include <psp2kern/kernel/iofilemgr.h>
#include <psp2kern/kernel/sysmem/uid_puid.h>

#include <stdlib.h>

#include "vfs/vfs.h"
#include "mddev.h"
#include "memdisk.h"

int md_vfs_mount(SceVfsOpMountArgs *argp)
{
	SceVfsMount *mnt = argp->mnt;

	mnt->availableEntryNum = 2;
	mnt->defaultIoCacheSize = 0;
	return 0;
}

int md_vfs_umount(SceVfsOpUmountArgs *argp)
{
	return 0x80010010;
}

int md_vfs_set_root(SceVfsOpSetRootArgs *argp)
{
	SceVfsVnode *vp = argp->vp;
	vp->dd = NULL;
	vp->type = SCE_VNODE_TYPE_ROOTDIR_DEVFS;
	vp->state = SCE_VNODE_STATE_ACTIVE;
	vp->mnt = argp->mnt;
	vp->aclData[0] = vp->aclData[1] = 0;
	vp->size = 0;

	return 0;
}

int md_vfs_init(SceVfsOpInitArgs *argp)
{
	return 0;
}

int md_vfs_devctl(SceVfsOpDevctlArg *argp)
{
	const void *arg = argp->arg;
	SceSize argLen = argp->argLen;
	void *buf = argp->buf;
	SceSize bufLen = argp->bufLen;
	switch (argp->cmd) {
	case SCE_MEMDISK_DEVCTL_ENABLE:
		if ((arg == NULL) || (argLen != 4) || (buf == NULL) || (bufLen != 4)) {
			return 0x80010016;
		}

		SceUID block = ((SceMemDiskEnableArg *)arg)->block;
		if (block & 0x40000000) {
			block = kscePUIDtoGUID(0, block);
			if (block < 0) {
				return block;
			}
		}

		int devIndex = mddev_mkdev(block);
		if (devIndex < 0) {
			return devIndex;
		}

		((SceMemDiskEnableResult *)buf)->index = devIndex;
		break;
	case 0x1: // Used by Exfatfs to determine if a block device is read-only. Returning EUNSUP allows read-write mounting
		return 0x80010030;
	default:
		return 0x80010013;
	}

	return 0;
}

int md_vfs_open(SceVopOpenArgs *argp)
{
	SceVfsVnode *vp = argp->vp;
	SceVfsFile *file = argp->file;
	mddev_object *dev = vp->nodeData;
	int ret;

	if (dev == NULL) {
		return 0x80010002;
	}

	if ((ret = mddev_lock(dev)) < 0) {
		return ret;
	}

	if (MDDEV_STAT_IS_VALID(dev) && (dev->stat != MDDEV_STAT_ENABLED)) {
		ret = 0x80010002;
		goto exit;
	} else if (!MDDEV_STAT_IS_VALID(dev)) {
		ret = 0x80010013;
		goto exit;
	}

	file->fd = (SceUInt32)dev;
	vp->size = dev->blockSize;

exit:
	mddev_unlock(dev);
	return ret;
}

int md_vfs_close(SceVopCloseArgs *argp)
{
	return 0;
}

int md_vfs_lookup(SceVopLookupArgs *argp)
{
	SceVfsVnode *dvp = argp->dvp;

	int index = strtol(argp->path->name, NULL, 10);

	if (index != 0 && index != 1) {
		return 0x80010002;
	}

	mddev_object *dev = mddev_get_object(index);
	
	int stat = mddev_get_stat(dev);
	if (stat != MDDEV_STAT_ENABLED) {
		return 0x80010002;
	}

	SceVfsVnode *vp;
	if ((stat = vfsGetNewVnode(dvp->mnt, dvp->mnt->mntVfsInf->defaultVops, 0, &vp)) < 0) {
		return stat;
	}

	vfsLockVnode(vp);
	vp->mnt = dvp->mnt;
	vp->state = SCE_VNODE_STATE_ACTIVE;
	vp->type = SCE_VNODE_TYPE_DEV;
	vp->nodeData = dev;
	vp->aclData[0] = 0606;
	vp->aclData[1] = 0;
	vp->size = 0;

	*argp->vpp = vp;

	return 0;
}

SceSSize md_vfs_read(SceVopReadArgs *argp)
{
	SceVfsVnode *vp = argp->vp;
	SceVfsFile *file = argp->file;
	SceSize nbyte = argp->nbyte;
	void *buf = argp->buf;

	if (vp->nodeData == NULL) {
		return (int)0x80010013;
	}

	if (nbyte & MDDEV_SECTOR_MASK) {
		return (int)0x80010022;
	}

	SceOff offset = file->position;

	if ((offset + nbyte) > vp->size) {
		return 0x80010021;
	}

	SceSSize ret = mddev_read(vp->nodeData, buf, offset >> MDDEV_SECTOR_SHIFT, nbyte >> MDDEV_SECTOR_SHIFT);

	if (ret >= 0) {
		file->position += ret;
	}

	return ret;
}

SceSSize md_vfs_write(SceVopWriteArgs *argp)
{
	SceVfsVnode *vp = argp->vp;
	SceVfsFile *file = argp->file;
	SceSize nbyte = argp->nbyte;
	const void *buf = argp->buf;

	if (vp->nodeData == NULL) {
		return (int)0x80010013;
	}

	if (nbyte & MDDEV_SECTOR_MASK) {
		return (int)0x80010022;
	}

	SceOff offset = file->position;

	if ((offset + nbyte) > vp->size) {
		return 0x80010021;
	}

	SceSSize ret = mddev_write(vp->nodeData, buf, offset >> MDDEV_SECTOR_SHIFT, nbyte >> MDDEV_SECTOR_SHIFT);

	if (ret >= 0) {
		file->position += ret;
	}

	return ret;
}

SceOff md_vfs_lseek(SceVopLseekArgs *argp)
{
	SceVfsVnode *vp = argp->vp;
	SceVfsFile *file = argp->file;
	SceOff offset = argp->offset;
	SceUInt whence = argp->where;

	if (vp->nodeData == NULL) {
		return (int)0x80010013;
	}

	if (offset & MDDEV_SECTOR_MASK) {
		return (int)0x80010022;
	}

	switch (whence) {
	case SCE_SEEK_SET:
		break;
	case SCE_SEEK_CUR:
		offset += file->position;
		break;
	case SCE_SEEK_END:
		offset += vp->size;
		break;
	default:
		return (int)0x80010016;
	}

	if (offset > vp->size) {
		return (int)0x80010021;
	}

	file->position = offset;

	return offset; // Return the new offset (0 for no operation)
}

int md_vfs_remove(SceVopRemoveArgs *argp)
{
	SceVfsVnode *vp = argp->vp;
	mddev_object *dev = vp->nodeData;

	vp->nodeData = NULL;

	return mddev_rmdev(dev);
}

SceSSize md_vfs_pread(SceVopPreadArgs *argp)
{
	SceVfsVnode *vp = argp->vp;
	SceSize nbyte = argp->nbyte;
	void *buf = argp->buf;
	SceOff offset = argp->offset;

	if (vp->nodeData == NULL) {
		return (int)0x80010013;
	}

	if ((offset & MDDEV_SECTOR_MASK) || (nbyte & MDDEV_SECTOR_MASK)) {
		return (int)0x80010022;
	}

	if ((offset + nbyte) > vp->size) {
		return 0x80010021;
	}

	return mddev_read(vp->nodeData, buf, offset >> MDDEV_SECTOR_SHIFT, nbyte >> MDDEV_SECTOR_SHIFT);
}

SceSSize md_vfs_pwrite(SceVopPwriteArgs *argp)
{
	SceVfsVnode *vp = argp->vp;
	SceSize nbyte = argp->nbyte;
	const void *buf = argp->buf;
	SceOff offset = argp->offset;

	if (vp->nodeData == NULL) {
		return 0x80010013;
	}

	if ((offset & MDDEV_SECTOR_MASK) || (nbyte & MDDEV_SECTOR_MASK)) {
		return 0x80010022;
	}

	if ((offset + nbyte) > vp->size) {
		return 0x80010021;
	}

	return mddev_write(vp->nodeData, buf, offset >> MDDEV_SECTOR_SHIFT, nbyte >> MDDEV_SECTOR_SHIFT);
}

int md_vfs_inactive(SceVopInactiveArgs *argp)
{
	return 0;
}

int md_vfs_sync(SceVopSyncArgs *argp)
{
	return 0;
}

static SceVfsInfo md_vfs_info = {
	.vfsOps = &(SceVfsOpTable){
		.vfs_mount = md_vfs_mount,
		.vfs_umount = md_vfs_umount,
		.vfs_set_root = md_vfs_set_root,
		.vfs_init = md_vfs_init,
		.vfs_devctl = md_vfs_devctl},
	.vfsName = "md_dev_fs",
	.vfsNameLen = __builtin_strlen("md_dev_fs") + 1,
	.defaultVops = &(SceVopTable){
		.vop_open = md_vfs_open,
		.vop_close = md_vfs_close,
		.vop_lookup = md_vfs_lookup,
		.vop_read = md_vfs_read,
		.vop_write = md_vfs_write,
		.vop_lseek = md_vfs_lseek,
		.vop_remove = md_vfs_remove,
		.vop_pread = md_vfs_pread,
		.vop_pwrite = md_vfs_pwrite,
		.vop_inactive = md_vfs_inactive,
		.vop_sync = md_vfs_sync
	},
	
	.type = SCE_VFS_TYPE_DEVFS
	};

int md_vfs_initialize()
{
	int ret = 0;
	if ((ret = mddev_initialize()) < 0) {
		return ret;
	}

	if ((ret = ksceVfsAddVfs(&md_vfs_info)) < 0) {
		return ret;
	}

	if ((ret = ksceIoMount(2, NULL, 0, 0, 0, 0)) < 0) {
		return ret;
	}

	return ret;
}

int _sceMemDiskModuleStart(SceSize args, void *argp)
{
	if (md_vfs_initialize() < 0) {
		return SCE_KERNEL_START_FAILED;
	}

	return SCE_KERNEL_START_SUCCESS;
}