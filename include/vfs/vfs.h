#ifndef _PSP2KERN_VFS_VFS_H_
#define _PSP2KERN_VFS_VFS_H_

#include <psp2kern/types.h>

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

int ksceVfsAddVfs(SceVfsInfo *pVfsInfo);
int ksceVfsDeleteVfs(const char *name, SceVfsInfo **ppVfsInfo);

SceUID vfsAllocateFile(SceVfsVnode *vp, SceVfsFile **ppFile, const char *name);
int vfsFreeFile(SceVfsVnode *vp, SceUID fd);

int vfsGetNewVnode(SceVfsMount *mnt, SceVopTable *vops, SceUInt unk, SceVfsVnode **vpp);
int vfsFreeVnode(SceVfsVnode *vp);

int vfsLockVnode(SceVfsVnode *vp);
int vfsUnlockVnode(SceVfsVnode *vp);
SceBool vfsIsOwnerVnode(SceVfsVnode *vp);


#ifdef __cplusplus
}
#endif

#endif /* _PSP2KERN_VFS_VNODE_H_ */