#ifndef _PSP2KERN_VFS_TYPES_H_
#define _PSP2KERN_VFS_TYPES_H_

/**
 * @file types.h
 * @author your name (you@domain.com)
 * @brief Structures specifically relevant to VFS implementations
 * @version 0.1
 * @date 2024-12-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <psp2kern/types.h>
#include <psp2common/kernel/iofilemgr.h>
#include <psp2kern/kernel/threadmgr/fast_mutex.h>

#include <stdbool.h>

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

#define SCE_VFS_TYPE_FS    (0x0000000)
#define SCE_VFS_TYPE_DEVFS (0x00000010)

/**
 * VFS Info defining a VFS Implementation
 */
typedef struct SceVfsInfo {
    SceVfsOpTable *vfsOps;
    char *vfsName;
    SceSize vfsNameLen;
    
    SceUInt32 refCount;
    SceUInt32 type;      //!< One of SCE_VFS_TYPE_*
    
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

#define SCE_VFS_FS_TYPE_FS      (0x01) //!< All exfat mounts
#define SCE_VFS_FS_TYPE_PFS     (0x03) //!< PFS mounts
#define SCE_VFS_FS_TYPE_HOSTFS  (0x04) //!< host0:
#define SCE_VFS_FS_TYPE_BLKDEV  (0x10) //!< sdstor0:, md0:
#define SCE_VFS_FS_TYPE_CHRDEV  (0x20) //!< tty0:

#define SCE_VFS_MOUNT_TYPE_PFS          (0x00000001) //!< PFS mounts
#define SCE_VFS_MOUNT_TYPE_FSROOT       (0x00000002) //!< Standard root file system
#define SCE_VFS_MOUNT_TYPE_DEVFS        (0x00000003) //!< Device File System
#define SCE_VFS_MOUNT_TYPE_STACKFS      (0x00000005) //!< Loop File System (File system mounted from a file)
#define SCE_VFS_MOUNT_TYPE_HOSTFS       (0x00000006) //!< Hostfs
#define SCE_VFS_MOUNT_FLAG_TYPE_MASK    (0x000000FF)
#define SCE_VFS_MOUNT_FLAG_RDONLY       (0x00001000) //!< Read-only mount
#define SCE_VFS_MOUNT_FLAG_NOBUF        (0x00002000) //!< Disables VFS Buffer cache
/**
 * Files for this mountpoint are treated as numbered extensions of the assignName.
 *
 * Example: tty mountpoint devices (tty0:, tty1:) internally are handled /tty/tty0:
 */
#define SCE_VFS_MOUNT_FLAG_NUMBERED     (0x00004000)
#define SCE_VFS_MOUNT_FLAG_REMOTE       (0x00008000) //!< Prevents ncache entry of failed lookups
#define SCE_VFS_MOUNT_FLAG_INTERNAL     (0x00010000)
#define SCE_VFS_MOUNT_FLAG_EXTERNAL     (0x00020000)
#define SCE_VFS_MOUNT_FLAG_WRITE_CACHE  (0x00040000) //!< Allow write-caching. Default behavior is to flush buffer cache after write.
#define SCE_VFS_MOUNT_FLAG_NO_RECLAIM   (0x00100000) //!< Prevents Vnodes from being reclaimed. Only set internally for devfs 

typedef struct SceVfsMount {
    SceKernelFastMutex fastMutex;

    SceVfsVnode *mntVnode;
    
    SceUID allocator;
    
    SceUInt32 state;
    SceUInt8 fsType;    //!< One of SCE_VFS_FS_TYPE_*
    SceUInt16 opt;
    SceUInt32 mntFlags; //!< ORed together SCE_VFS_MOUNT_FLAG_* flags
    
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
    struct SceVfsNcache *next; //1< Next ncache entry in directory
    struct SceVfsNcache *ncdd; //!< Parent ncache entry
    struct SceVfsNcache *listNext; //!< Next ncache entry in global ncache list
    struct SceVfsNcache *nnc; //!< Child ncache entry (Used for directory entries)
    
    SceVfsVnode *vp;

    SceUInt type;

    SceSize nameLen;
    char *name;
} SceVfsNcache;

#define SCE_VNODE_TYPE_REG     (0x00000001) //!< Regular file
#define SCE_VNODE_TYPE_DIR     (0x00000002) //!< Regular directory
#define SCE_VNODE_TYPE_CHR     (0x00000010) //!< Character file
#define SCE_VNODE_TYPE_ROOT    (0x00001000) //!< Root vnode
#define SCE_VNODE_TYPE_DEV     (0x00002000) //!< Device vnode
#define SCE_VNODE_TYPE_MOUNTED (0x00004000) //!< Vnode used as a backing for a mountpoint

#define SCE_VNODE_TYPE_CHRDEV        (SCE_VNODE_TYPE_DEV | SCE_VNODE_TYPE_CHR)
#define SCE_VNODE_TYPE_ROOTDIR       (SCE_VNODE_TYPE_ROOT | SCE_VNODE_TYPE_DIR) //!< Root directory
#define SCE_VNODE_TYPE_ROOTDIR_DEVFS (0x10000 | 0x20) //!< Root directory on devfs mountpoints

#define SCE_VNODE_STATE_ACTIVE    (0x00000001)
#define SCE_VNODE_STATE_DELETED   (0x00000100)
#define SCE_VNODE_STATE_UNMOUNTED (0x00000200)
#define SCE_VNODE_STATE_WHITEOUT  (0x00000400)
#define SCE_VNODE_STATE_RECLAIMED (0x00002000)
#define SCE_VNODE_STATE_NEW       (0x00008000)

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

        SceUInt32 state; //!< One of SCE_VNODE_STATE_*
        SceUInt32 type;  //!< ORed together SCE_VNODE_TYPE_* flags
        
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
	SceVfsInfo *vfsInf;
} SceVfsOpInitArgs;

typedef struct SceVfsOpFiniArgs {
    SceVfsInfo *vfsInf;
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

typedef struct SceVopFchstatArgs {
	SceVfsVnode *pNode;
	SceVfsFile *file;
	SceIoStat *stat;
	SceUInt32 bit;
} SceVopFchstatArgs;

typedef struct SceVopFgetstatArgs {
	SceVfsVnode *vp;
	SceVfsFile *file;
	SceIoStat *stat;
} SceVopFgetstatArgs;

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

    int (*vop_fgetstat)(SceVopFgetstatArgs *argp);
    int (*vop_fchstat)(SceVopFchstatArgs *argp);

    int (*vop_whiteout)(SceVopWhiteoutArgs *argp);
    int (*vop_cleanup)(SceVopCleanupArgs *argp);
    int (*vop_verofill)(SceVopZerofillArgs *argp);
} SceVopTable;

#ifdef __cplusplus
}
#endif

#endif