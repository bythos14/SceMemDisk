#ifndef _PSP2KERN_VFS_VNOPS_H_
#define _PSP2KERN_VFS_VNOPS_H_

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

int ksceVopOpen(SceVfsVnode *vp, SceVfsPath *path, int flags, SceVfsFile *file);
int ksceVopCreate(SceVfsVnode *vp, SceVfsVnode **vpp, SceVfsPath *path, int flags, int mode);
int ksceVopClose(SceVfsVnode *vp, SceVfsFile *file);
int ksceVopLookup(SceVfsVnode *vp, SceVfsVnode **vpp, SceVfsPath *path, SceUInt32 flags);
int ksceVopRead(SceVfsVnode *vp, SceVfsFile *file, void *data, SceSize nbyte, SceSize *pResult);
int ksceVopWrite(SceVfsVnode *vp, SceVfsFile *file, const void *data, SceSize nbyte, SceSize *pResult);
SceOff ksceVopLseek(SceVfsVnode *vp, SceVfsFile *file, SceOff offset, int where);

int ksceVopIoctl(SceVfsVnode *vp, SceVfsFile *file, int cmd, const void *inData, SceSize inLen, void *outData, SceSize outLen);
int ksceVopRemove(SceVfsVnode *dvp, SceVfsVnode *vp, SceVfsPath *path, SceUInt32 flags); 
int ksceVopMkdir(SceVfsVnode *dvp,SceVfsVnode **vpp, SceVfsPath *path, int mode);
int ksceVopRmdir(SceVfsVnode *dvp, SceVfsVnode *vp, SceVfsPath *path);

int ksceVopDopen(SceVfsVnode *vp, SceVfsPath *path, SceVfsFile *file);
int ksceVopDclose(SceVfsVnode *vp, SceVfsFile *file);
int ksceVopDread(SceVfsVnode *vp, SceVfsFile *file, SceIoDirent *dir);

int ksceVopGetstat(SceVfsVnode *vp, SceVfsPath *path, SceIoStat *stat);
int ksceVopChstat(SceVfsVnode *vp, SceVfsPath *path, SceIoStat *stat, int bit);

/*
 * pNode  - maybe old parent node
 * pNode2 - rename old node
 * pNode3 - unknown
 */
int ksceVopRename(SceVfsVnode *odvp, SceVfsVnode *ovp, SceVfsPath *oldPath, SceVfsVnode *ndvp, SceVfsVnode **nvpp, SceVfsPath *newPath);
int ksceVopPread(SceVfsVnode *vp, SceVfsFile *file, void *data, SceSize nbyte, SceOff offset, SceSize *pResult);
int ksceVopPwrite(SceVfsVnode *vp, SceVfsFile *file, const void *data, SceSize nbyte, SceOff offset, SceSize *pResult);
int ksceVopInactive(SceVfsVnode *vp);
int ksceVopSync(SceVfsVnode *vp, SceVfsFile *file, int flags);
int ksceVopGetStateByFd(SceVfsVnode *vp, SceVfsFile *file, SceIoStat *stat);
int ksceVopChstatByFd(SceVfsVnode *vp, SceVfsFile *file, SceIoStat *stat, int bit);

int sceVopWhiteout(SceVfsVnode *dvp, SceVfsVnode *vp, SceVfsPath *oldPath, SceVfsPath *newPath);
int sceVopCleanup(SceVfsVnode *vp, SceVfsFile *file);
int sceVopZerofill(SceVfsVnode *dvp, SceUInt64 unk, SceUInt64 unk1, SceUInt64 unk2);

#ifdef __cplusplus
}
#endif

#endif