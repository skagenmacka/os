#pragma once

enum vnode_type { VNODE_FILE, VNODE_DIR };

struct vnode;

struct fs_ops {
  int (*lookup)(struct vnode *dir, const char *name, struct vnode **out);
  int (*create)(struct vnode *dir, const char *name, enum vnode_type type,
                struct vnode **out);
  int (*read)(struct vnode *node, unsigned int offset, char *buf,
              unsigned int size);
  int (*write)(struct vnode *node, unsigned int offset, const char *buf,
               unsigned int size);
  int (*readdir)(struct vnode *dir, unsigned int index, char *name_out,
                 unsigned int name_out_size, enum vnode_type *type_out);
};

struct vnode {
  enum vnode_type type;
  unsigned int size;
  void *internal;
  struct fs_ops *ops;
};

int ramfs_init(void);
struct vnode *ramfs_root(void);
