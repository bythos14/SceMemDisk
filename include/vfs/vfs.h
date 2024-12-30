#ifndef _PSP2KERN_VFS_VFS_H_
#define _PSP2KERN_VFS_VFS_H_

#include <psp2kern/types.h>

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SceVfsMountParam {
    char *rootPath;
    char *blockdevName;

    SceUInt8 fsType;
    SceUInt16 opt;
    SceUInt32 mntFlags;
    
    char *vfsName;
    
    void *pData;
    
    struct SceVfsMountData *misc;
    
    struct SceVopTable *vops;
} SceVfsMountParam;

#define SCE_VFS_UMOUNT_FLAG_FORCE (0x1)

typedef struct SceVfsUmountParam {
    char *assignName;
    int flag;
} SceVfsUmountParam;

int ksceVfsAddVfs(SceVfsInfo *pVfsInfo);
int ksceVfsDeleteVfs(const char *name, SceVfsInfo **ppVfsInfo);

int vfsMount(SceVfsMountParam *param);
int vfsUmount(SceVfsUmountParam *param);

int vfsMountForPFS(SceVfsMountParam *param);
int vfsUmountForPFS(SceVfsUmountParam *param, int unk, int unk2);

int vfsLockMnt(SceVfsMount *mnt);
int vfsUnlockMnt(SceVfsMount *mnt);
int vfsIsLockedMnt(SceVfsMount *mnt);

SceUID vfsAllocateFile(SceVfsVnode *vp, SceVfsFile **ppFile, const char *name);
SceUID vfsAllocateFileDummy(SceUInt32 flags, SceBool isDir, const char *name);
int vfsFreeFile(SceVfsVnode *vp, SceUID fd);

int vfsGetNewVnode(SceVfsMount *mnt, SceVopTable *vops, int unk, SceVfsVnode **vpp);
int vfsFreeVnode(SceVfsVnode *vp);

int vfsLockVnode(SceVfsVnode *vp);
int vfsUnlockVnode(SceVfsVnode *vp);
SceBool vfsIsOwnerVnode(SceVfsVnode *vp);

int vfsBuffCacheCtl(SceVfsVnode *vp, SceSize data_cache_size, SceInt way, SceInt line_size, SceSize io_size);
int vfsBuffCacheFree(SceVfsVnode *vp, SceVfsFile *file, int flag, int unk);


#ifdef __cplusplus
}
#endif

#endif /* _PSP2KERN_VFS_VNODE_H_ */