#pragma once

#include "fs.h"

struct file {
  struct vnode *node;
  unsigned int offset;
  int flags;
  int used;
};

int vfs_init(void);
int vfs_open(const char *path, int flags, struct file **out);
int vfs_read(struct file *file, char *buf, unsigned int size);
int vfs_write(struct file *file, const char *buf, unsigned int size);
int vfs_close(struct file *file);
int vfs_create(const char *path, enum vnode_type type);
int vfs_readdir(const char *path, unsigned int index, char *name_out,
                unsigned int name_out_size, enum vnode_type *type_out);
