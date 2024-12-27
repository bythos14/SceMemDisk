#include <psp2kern/kernel/modulemgr.h>
#include <psp2kern/kernel/iofilemgr.h>

#include "vfs/vfs.h"
#include "mddev.h"

int md_vfs_mount(SceVfsOpMountArgs *argp)
{
	return 0; // Return success or handle as needed
}

// Callback function for vfs_umount
int md_vfs_umount(SceVfsOpUmountArgs *argp)
{
	return 0; // Return success or handle as needed
}

int md_vfs_set_root(SceVfsOpSetRootArgs *argp)
{
	return 0; // Return success or handle as needed
}

int md_vfs_init(SceVfsOpInitArgs *argp)
{
	// Empty function body
	return 0; // Return success or handle as needed
}

// Callback function for vfs_devctl
int md_vfs_devctl(SceVfsOpDevctlArg *arg)
{
	// Empty function body
	return 0; // Return success or handle as needed
}

int md_vfs_open(SceVopOpenArgs *argp)
{
	// Empty function body
	return 0; // Return success or handle as needed
}

int md_vfs_close(SceVopCloseArgs *argp)
{
	// Empty function body
	return 0; // Return success or handle as needed
}

int md_vfs_lookup(SceVopLookupArgs *argp)
{
	// Empty function body
	return 0; // Return success or handle as needed
}

SceSSize md_vfs_read(SceVopReadArgs *argp)
{
	// Empty function body
	return 0; // Return number of bytes read (0 for no operation)
}

SceSSize md_vfs_write(SceVopWriteArgs *argp)
{
	// Empty function body
	return 0; // Return number of bytes written (0 for no operation)
}

SceOff md_vfs_lseek(SceVopLseekArgs *argp)
{
	// Empty function body
	return 0; // Return the new offset (0 for no operation)
}

int md_vfs_remove(SceVopRemoveArgs *argp)
{
	// Empty function body
	return 0; // Return success or handle as needed
}

SceSSize md_vfs_pread(SceVopPreadArgs *argp)
{
	// Empty function body
	return 0; // Return number of bytes read (0 for no operation)
}

SceSSize md_vfs_pwrite(SceVopPwriteArgs *argp)
{
	// Empty function body
	return 0; // Return number of bytes written (0 for no operation)
}

int md_vfs_inactive(SceVopInactiveArgs *argp)
{
	// Empty function body
	return 0; // Return success or handle as needed
}

int md_vfs_sync(SceVopSyncArgs *argp)
{
	// Empty function body
	return 0; // Return success or handle as needed
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
	}};

int md_vfs_initialize()
{
	int ret = 0;
	if (ret = mddev_initialize() < 0) {
		return ret;
	}

	if (ret = ksceVfsAddVfs(&md_vfs_info) < 0) {
		return ret;
	}

	
	if ((ret = ksceIoMount(2, NULL, 0, 0, 0, 0)) < 0) {
		return ret;
	}
}

int _sceMemDiskModuleStart(SceSize args, void *argp)
{
	if (md_vfs_initialize() < 0) {
		return SCE_KERNEL_START_FAILED;
	}

	return SCE_KERNEL_START_SUCCESS;
}