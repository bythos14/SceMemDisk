#ifndef _PSP2KERN_VFS_TYPES_H_
#define _PSP2KERN_VFS_TYPES_H_

#include <psp2kern/types.h>
#include <psp2common/kernel/iofilemgr.h>
#include <psp2kern/kernel/threadmgr/fast_mutex.h>

#include <stdbool.h>

#include "defs.h"

#ifdef __cplusplus
extern "C" {
#endif

//Forward declarations
typedef struct SceVfsVnode SceVfsVnode;
typedef struct SceVfsMount SceVfsMount;
typedef struct SceVfsFile SceVfsFile;
typedef struct SceVfsOpTable SceVfsOpTable;
typedef struct SceVopTable SceVopTable;

typedef struct SceVfsPath {
    char *name;
    SceSize nameLength;
    char *path;
} SceVfsPath;

/**
 * VFS Info defining a VFS Implementation
 */
typedef struct SceVfsInfo {
    SceVfsOpTable *vfsOps;
    char *vfsName;
    SceSize vfsNameLen;
    
    SceUInt32 refCount;
    SceUInt32 type;
    
    SceVopTable *defaultVops;
    void *vfsData;
    
    struct SceVfsInfo *next;
} SceVfsInfo;

typedef struct SceVfsMountData {
    char *assignName;
    char *fsName;
    char *blockdevName;
    char *blockdevNameNoPart;
    SceUInt32 mntId;
} SceVfsMountData;

typedef struct SceVfsMount {
    SceKernelFastMutex fastMutex;

    SceVfsVnode *mntVnode;
    
    SceUID allocator;
    
    SceUInt32 state;
    SceUInt8 fsType;
    SceUInt16 opt;
    SceUInt32 mntFlags;
    
    SceVfsVnode *vnodeList;
    SceUInt32 vnodeNum;
    
    SceVfsInfo *mntVfsInf;
    SceUInt32 mntRefCount;
    
    SceUInt32 openedEntryNum;
    SceUInt32 availableEntryNum;
    
    SceUID pid;
    
    SceVfsMount *mntedOnList;
    SceVfsMount *mntedOnListPrev;
    SceVfsMount *mntListNext;
    
    SceVfsMountData *mntData;
    char path[64];

    SceUInt32 defaultIoCacheSize;
    
    void *pData;  //!< VFS Implementation defined mount point data.
    
    struct FdLock *pFdLock;
    struct Fumount *pFumount;

    SceUInt32 opaque[4];  // Backing storage for pFdLock
    SceUInt32 opaque2[4]; // Backing storage for pFumount
    
    SceUInt8 padding[16];
} SceVfsMount;

typedef struct SceVfsNcache {
    struct SceVfsNcache *next; /* Next ncache entry in directory */
    struct SceVfsNcache *ncdd; /* Parent ncache entry */
    struct SceVfsNcache *listNext; /* Next ncache entry in global ncache list */
    struct SceVfsNcache *nnc; /* Child ncache entry (Used for directory entries) */
    
    SceVfsVnode *vp; /* Vnode ptr */

    SceUInt type;

    SceSize nameLen;
    char *name;
} SceVfsNcache;

typedef struct SceVfsVnode {
    struct {
        SceUInt32 waiter;
        SceUID ownerId;
        SceUInt32 recursiveCount;
        SceUID waitId;
        SceUInt32 waitPattern;
    };

    SceUInt8 padding[44];

    struct {  
        struct SceVopTable *ops;
        
        SceUInt32 nodeInf; 
        void *nodeData;
        
        SceVfsMount *mnt;
        
        SceVfsVnode *dd; // Parent
        SceVfsVnode *next;
        
        SceUInt32 refCount;
        
        struct BuffCache *bc;

        SceUInt32 fid[2];
        struct Flock *flock;

        SceUID allocator;
        
        struct SceVfsNcache *ncache;
        
        SceUInt32 state;
        SceUInt32 type;
        
        struct SceVopTable *vop_tbl; 
        
        SceUInt64 size;
        SceUInt32 aclData[2];

        struct SceVfsFile *fdList;
        SceUInt32 fdNum;
        
        struct SceVfsVnode *linkTo;
        SceUInt32 linkedNum;
        
        SceUInt8 unused[48];
        
        SceUInt32 mntOpt;
        SceUInt32 unk_0xd4;
    };

    SceUInt8 padding2[40];
} SceVfsVnode;

typedef struct SceVfsFile {
    SceUInt32 isDir;
    SceUInt32 openMode;

    SceOff position;

    SceUInt32 state;
    SceUID pid;
    
    SceVfsVnode *vnode;
    struct SceVfsFile *next;
    
    SceUInt32 fd;
    SceUInt16 flockBusyCount;
    
    bool locked;
    bool hasFlockEnt;
    
    struct SceVfsFdLock *fdLock;
    
    SceUInt32 idata;
    
    void *debugPath; 
    
    SceUInt32 ioSchedData[3];
} SceVfsFile;

/**
 * VFS Op Callback Arguments
 */

typedef struct SceVfsOpMountArgs {
	struct SceVfsMount *mnt;
	SceVfsPath *devFilePath;
} SceVfsOpMountArgs;

typedef struct SceVfsOpUmountArgs {
	struct SceVfsMount *mnt;
	int flags;
} SceVfsOpUmountArgs;

typedef struct SceVfsOpSetRootArgs {
	struct SceVfsMount *mnt;
	int unk; // Typically 0
	struct SceVfsVnode *vp;
} SceVfsOpSetRootArgs;

typedef struct SceVfsOpGetRootArgs {
	struct SceVfsMount *mnt;
	int unk; // Typically 0
	struct SceVfsVnode **vpp;
} SceVfsOpGetRootArgs;

typedef struct SceVfsOpSyncArgs {
	struct SceVfsMount *mnt;
	int flags;
} SceVfsOpSyncArgs;

typedef struct SceVfsOpInitArgs {
	struct SceVfsAddParam *pParam;
} SceVfsOpInitArgs;

typedef struct SceVfsOpFiniArgs {
	struct SceVfsAddParam *pParam;
} SceVfsOpFiniArgs;

typedef struct SceVfsOpDevctlArg {
	struct SceVfsMount *mnt;
	const char *dev;
	unsigned int cmd;
    const void *arg;
    SceSize argLen;
    void *buf;
    SceSize bufLen;
} SceVfsOpDevctlArg;

typedef struct SceVfsOpDecodePathElemArgs { 
	struct SceVfsMount *mnt;
	const char *path;      
	const char **path2;    // path + something
	const char **path3;    // path3 + *decodeLen
	char *buf;             
	SceSize bufLen;      
	SceSize *decodeLen;  // out
} SceVfsOpDecodePathElemArgs;

typedef struct SceVfsOpTable
{
    int (*vfs_mount)(SceVfsOpMountArgs *argp);
    int (*vfs_umount)(SceVfsOpUmountArgs *argp);
    int (*vfs_set_root)(SceVfsOpSetRootArgs *argp);
    int (*vfs_get_root)(SceVfsOpGetRootArgs *argp);

    void *reserved[2]; // Unused

    int (*vfs_sync)(SceVfsOpSyncArgs *argp);

    void *reserved2; // Unused

    int (*vfs_init)(SceVfsOpInitArgs *argp);
    int (*vfs_fini)(SceVfsOpFiniArgs *argp);

    void *reserved3; // Unused

    int (*vfs_devctl)(SceVfsOpDevctlArg *arg);
    int (*vfs_decode_path_elem)(SceVfsOpDecodePathElemArgs *argp);
} SceVfsOpTable;

/**
 * VFS VOP Callback Arguments
 */
typedef struct SceVopOpenArgs {
	SceVfsVnode *vp;
	SceVfsPath *path;
	int flags;
	SceVfsFile *file;
} SceVopOpenArgs;

typedef struct SceVopCreateArgs {
	SceVfsVnode *dvp;
	SceVfsVnode **vpp;
	SceVfsPath *path;
	int flags;
	int mode;
} SceVopCreateArgs;

typedef struct SceVopCloseArgs {
	SceVfsVnode *vp;
	SceVfsFile *file;
} SceVopCloseArgs;

typedef struct SceVopLookupArgs {
	SceVfsVnode  *dvp;
	SceVfsVnode **vpp;
	SceVfsPath *path;
	SceUInt32 flags;
} SceVopLookupArgs;

typedef struct SceVopReadArgs {
	SceVfsVnode *vp;
	SceVfsFile *file;
	void *buf;
	SceSize nbyte;
} SceVopReadArgs;

typedef struct SceVopWriteArgs {
	SceVfsVnode *vp;
	SceVfsFile *file;
	const void *buf;
	SceSize nbyte;
} SceVopWriteArgs;

typedef struct SceVopLseekArgs {
	SceVfsVnode *vp;
	SceVfsFile *file;
	SceOff offset;
	int where;
} SceVopLseekArgs;

typedef struct SceVopIoctlArgs {
	SceVfsVnode *vp;
	SceVfsFile *file;
    int cmd;
    const void *inData;
    SceSize inLen;
    void *outData;
    SceSize outLen;
} SceVopIoctlArgs;

typedef struct SceVopRemoveArgs {
	SceVfsVnode *dvp;
	SceVfsVnode *vp;
	SceVfsPath *path;
	SceUInt32 flags;
} SceVopRemoveArgs;

typedef struct SceVopMkdirArgs {
	SceVfsVnode  *dvp;
	SceVfsVnode **vpp;
	SceVfsPath *path;
	int mode;
} SceVopMkdirArgs;

typedef struct SceVopRmdirArgs {
	SceVfsVnode *dvp;
	SceVfsVnode *vp;
	SceVfsPath *path;
} SceVopRmdirArgs;

typedef struct SceVopDopenAgrs {
	SceVfsVnode *vp;
	SceVfsPath *path;
	SceVfsFile *file;
} SceVopDopenAgrs;

typedef struct SceVopDcloseArgs {
	SceVfsVnode *vp;
	SceVfsFile *file;
} SceVopDcloseArgs;

typedef struct SceVopDreadArgs {
	SceVfsVnode *vp;
	SceVfsFile *file;
	SceIoDirent *dir;
} SceVopDreadArgs;

typedef struct SceVopGetstatArgs {
	SceVfsVnode *vp;
	SceVfsPath *path;
	SceIoStat *stat;
} SceVopGetstatArgs;

typedef struct SceVopChstatArgs {
	SceVfsVnode *vp;
	SceVfsPath *path;
	SceIoStat *stat;
	int bit;
} SceVopChstatArgs;

typedef struct SceVopChstatByFdArgs {
	SceVfsVnode *pNode;
	SceVfsFile *file;
	SceIoStat *stat;
	SceUInt32 bit;
} SceVopChstatByFdArgs;

typedef struct SceVopGetstatByFdArgs {
	SceVfsVnode *vp;
	SceVfsFile *file;
	SceIoStat *stat;
} SceVopGetstatByFdArgs;

typedef struct SceVopInactiveArgs {
	SceVfsVnode *vp;
} SceVopInactiveArgs;

typedef struct SceVopLinkArgs {
	SceVfsVnode *fvp;
	SceVfsVnode *tvp;
} SceVopLinkArgs;

typedef struct SceVopUnlinkArgs {
	SceVfsVnode *fvp;
	SceVfsVnode *tvp;
} SceVopUnlinkArgs;

typedef struct SceVopSyncArgs {
	SceVfsVnode *vp;
	SceVfsFile *file;
	int flags;
} SceVopSyncArgs;

typedef struct SceVopRenameArgs {
	SceVfsVnode *odvp;
	SceVfsVnode *ovp;
	SceVfsPath *oldPath;
	SceVfsVnode *ndvp;
	SceVfsVnode **nvpp;
	SceVfsPath *newPath;
} SceVopRenameArgs;

typedef struct SceVopPreadArgs {
	SceVfsVnode *vp;
	SceVfsFile *file;
    void *buf;
    SceSize nbyte;
    SceOff offset;
} SceVopPreadArgs;

typedef struct SceVopPwriteArgs {
	SceVfsVnode *vp;
	SceVfsFile *file;
    const void *buf;
    SceSize nbyte;
    SceOff offset;
} SceVopPwriteArgs;

typedef struct SceVopWhiteoutArgs {
	SceVfsVnode *dvp;
	SceVfsVnode *vp;
	SceVfsPath *oldPath;
	SceVfsPath *newPath;
} SceVopWhiteoutArgs;

typedef struct SceVopCleanupArgs {
	SceVfsVnode *vp;
	SceVfsFile *file;
} SceVopCleanupArgs;

typedef struct SceVopZerofillArgs {
	SceVfsVnode *vp;
    SceUInt64 unk;
    SceUInt64 unk1;
    SceUInt64 unk2;
} SceVopZerofillArgs;

typedef struct SceVopTable
{
    int (*vop_open)(SceVopOpenArgs *argp);
    int (*vop_create)(SceVopCreateArgs *argp);
    int (*vop_close)(SceVopCloseArgs *argp);
    int (*vop_lookup)(SceVopLookupArgs *argp);

    SceSSize (*vop_read)(SceVopReadArgs *argp);
    SceSSize (*vop_write)(SceVopWriteArgs *argp);
    SceOff (*vop_lseek)(SceVopLseekArgs *argp);

    int (*vop_ioctl)(SceVopIoctlArgs *argp);

    int (*vop_remove)(SceVopRemoveArgs *argp);

    int (*vop_mkdir)(SceVopMkdirArgs *argp);
    int (*vop_rmdir)(SceVopRmdirArgs *argp);

    int (*vop_dopen)(SceVopDopenAgrs *argp);
    int (*vop_dclose)(SceVopDcloseArgs *argp);
    int (*vop_dread)(SceVopDreadArgs *argp);

    int (*vop_getstat)(SceVopGetstatArgs *argp);
    int (*vop_chstat)(SceVopChstatArgs *argp);

    int (*vop_rename)(SceVopRenameArgs *argp);

    const void *func44; // Unused

    SceSSize (*vop_pread)(SceVopPreadArgs *argp);
    SceSSize (*vop_pwrite)(SceVopPwriteArgs *argp);

    int (*vop_inactive)(SceVopInactiveArgs *argp);
    int (*vop_link)(SceVopLinkArgs *argp);
    int (*vop_unlink)(SceVopUnlinkArgs *argp);

    int (*vop_sync)(SceVopSyncArgs *argp);

    int (*vop_fgetstat)(SceVopGetstatByFdArgs *argp);
    int (*vop_fchstat)(SceVopChstatByFdArgs *argp);

    int (*vop_whiteout)(SceVopWhiteoutArgs *argp);
    int (*vop_cleanup)(SceVopCleanupArgs *argp);
    int (*vop_verofill)(SceVopZerofillArgs *argp);
} SceVopTable;

#ifdef __cplusplus
}
#endif

#endif