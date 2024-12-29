#ifndef _PSP2KERN_VFS_DEFS_H_
#define _PSP2KERN_VFS_DEFS_H_

#define SCE_VFS_TYPE_DEVFS     (0x00000010)

#define SCE_VNODE_TYPE_REG     (0x00000001) //!< Regular file
#define SCE_VNODE_TYPE_DIR     (0x00000002) //!< Regular directory
#define SCE_VNODE_TYPE_TTY     (0x00000010) //!< TTY file
#define SCE_VNODE_TYPE_ROOT    (0x00001000) //!< Root vnode
#define SCE_VNODE_TYPE_DEV     (0x00002000) //!< Device vnode
#define SCE_VNODE_TYPE_MOUNTED (0x00004000) //!< Vnode used as a backing for a mountpoint

#define SCE_VNODE_TYPE_ROOTDIR    (SCE_VNODE_TYPE_ROOT | SCE_VNODE_TYPE_DIR) //!< Root directory
#define SCE_VNODE_TYPE_DEVROOTDIR (0x10000 | 0x20) //!< Root directory on devfs mountpoints

#define SCE_VNODE_STATE_ACTIVE    (0x00000001)
#define SCE_VNODE_STATE_DELETED   (0x00000100)
#define SCE_VNODE_STATE_UNMOUNTED (0x00000200)
#define SCE_VNODE_STATE_WHITEOUT  (0x00000400)
#define SCE_VNODE_STATE_RECLAIMED (0x00002000)
#define SCE_VNODE_STATE_NEW       (0x00008000)

#define SCE_VFS_MOUNT_TYPE_PFS          (0x00000001)
#define SCE_VFS_MOUNT_TYPE_FSROOT       (0x00000002)
#define SCE_VFS_MOUNT_TYPE_DEVFS        (0x00000003)
#define SCE_VFS_MOUNT_TYPE_STACKFS      (0x00000005)
#define SCE_VFS_MOUNT_TYPE_HOSTFS       (0x00000006)
#define SCE_VFS_MOUNT_FLAG_TYPE_MASK    (0x000000FF)
#define SCE_VFS_MOUNT_FLAG_RDONLY       (0x00001000) //!< Read-only mount
#define SCE_VFS_MOUNT_FLAG_NOBUF        (0x00002000) //!< Disables VFS Buffer cache
/**
 * Files for this mountpoint are treated as numbered extensions of the assignName.
 *
 * Example: tty mountpoint devices (tty0:, tty1:) internally are handled /tty/tty0:
 */
#define SCE_VFS_MOUNT_FLAG_NUMBERED     (0x00004000)
#define SCE_VFS_MOUNT_FLAG_SHARED       (0x00008000) //!< Mount point is shared, prevents ncache entry of failed lookups
#define SCE_VFS_MOUNT_FLAG_INTERNAL     (0x00010000)
#define SCE_VFS_MOUNT_FLAG_EXTERNAL     (0x00020000)
#define SCE_VFS_MOUNT_FLAG_NO_RECLAIM   (0x00100000) //!< Prevents Vnodes from being reclaimed.

#endif