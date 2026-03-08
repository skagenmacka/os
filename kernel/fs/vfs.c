#include "vfs.h"

#define VFS_MAX_OPEN_FILES 16

static struct vnode *vfs_root_node;
static struct file vfs_open_files[VFS_MAX_OPEN_FILES];

static int path_next_segment(const char **path, char *segment,
                             unsigned int segment_size) {
  unsigned int len = 0;
  const char *p = *path;

  while (*p == '/') {
    p++;
  }

  if (*p == '\0') {
    *path = p;
    return 0;
  }

  while (*p != '\0' && *p != '/') {
    if (len + 1 >= segment_size) {
      return -1;
    }

    segment[len++] = *p++;
  }

  segment[len] = '\0';
  *path = p;
  return 1;
}

static int vfs_lookup_path(const char *path, struct vnode **out) {
  struct vnode *current;
  char segment[32];
  int result;

  if (!path || path[0] != '/' || !out) {
    return -1;
  }

  current = vfs_root_node;
  if (path[1] == '\0') {
    *out = current;
    return 0;
  }

  while (1) {
    result = path_next_segment(&path, segment, sizeof(segment));
    if (result < 0) {
      return -1;
    }

    if (result == 0) {
      *out = current;
      return 0;
    }

    if (!current->ops || !current->ops->lookup) {
      return -1;
    }

    if (current->ops->lookup(current, segment, &current) < 0) {
      return -1;
    }
  }
}

static int vfs_lookup_parent(const char *path, struct vnode **parent_out,
                             char *name_out, unsigned int name_out_size) {
  struct vnode *current;
  const char *segment_start;
  const char *p;
  char segment[32];
  int result;

  if (!path || path[0] != '/' || !parent_out || !name_out) {
    return -1;
  }

  current = vfs_root_node;
  p = path;

  while (1) {
    while (*p == '/') {
      p++;
    }

    if (*p == '\0') {
      return -1;
    }

    segment_start = p;
    while (*p != '\0' && *p != '/') {
      p++;
    }

    if (*p == '\0') {
      unsigned int len = (unsigned int)(p - segment_start);
      unsigned int i;

      if (len == 0 || len + 1 > name_out_size) {
        return -1;
      }

      for (i = 0; i < len; i++) {
        name_out[i] = segment_start[i];
      }

      name_out[len] = '\0';
      *parent_out = current;
      return 0;
    }

    result = path_next_segment(&path, segment, sizeof(segment));
    if (result <= 0) {
      return -1;
    }

    if (!current->ops || !current->ops->lookup) {
      return -1;
    }

    if (current->ops->lookup(current, segment, &current) < 0) {
      return -1;
    }

    path = p;
  }
}

static struct file *vfs_alloc_file(void) {
  unsigned int i;

  for (i = 0; i < VFS_MAX_OPEN_FILES; i++) {
    if (!vfs_open_files[i].used) {
      vfs_open_files[i].used = 1;
      vfs_open_files[i].node = 0;
      vfs_open_files[i].offset = 0;
      vfs_open_files[i].flags = 0;
      return &vfs_open_files[i];
    }
  }

  return 0;
}

int vfs_init(void) {
  if (ramfs_init() < 0) {
    return -1;
  }

  vfs_root_node = ramfs_root();
  return vfs_root_node ? 0 : -1;
}

int vfs_open(const char *path, int flags, struct file **out) {
  struct vnode *node;
  struct file *file;

  if (!out) {
    return -1;
  }

  if (vfs_lookup_path(path, &node) < 0) {
    return -1;
  }

  if (node->type != VNODE_FILE) {
    return -1;
  }

  file = vfs_alloc_file();
  if (!file) {
    return -1;
  }

  file->node = node;
  file->offset = 0;
  file->flags = flags;
  *out = file;
  return 0;
}

int vfs_read(struct file *file, char *buf, unsigned int size) {
  int n;

  if (!file || !file->used || !file->node || !file->node->ops ||
      !file->node->ops->read) {
    return -1;
  }

  n = file->node->ops->read(file->node, file->offset, buf, size);
  if (n > 0) {
    file->offset += (unsigned int)n;
  }

  return n;
}

int vfs_write(struct file *file, const char *buf, unsigned int size) {
  int n;

  if (!file || !file->used || !file->node || !file->node->ops ||
      !file->node->ops->write) {
    return -1;
  }

  n = file->node->ops->write(file->node, file->offset, buf, size);
  if (n > 0) {
    file->offset += (unsigned int)n;
  }

  return n;
}

int vfs_close(struct file *file) {
  if (!file || !file->used) {
    return -1;
  }

  file->used = 0;
  file->node = 0;
  file->offset = 0;
  file->flags = 0;
  return 0;
}

int vfs_create(const char *path, enum vnode_type type) {
  struct vnode *parent;
  struct vnode *node;
  char name[32];

  if (vfs_lookup_parent(path, &parent, name, sizeof(name)) < 0) {
    return -1;
  }

  if (!parent->ops || !parent->ops->create) {
    return -1;
  }

  if (parent->ops->create(parent, name, type, &node) < 0) {
    return -1;
  }

  return 0;
}

int vfs_readdir(const char *path, unsigned int index, char *name_out,
                unsigned int name_out_size, enum vnode_type *type_out) {
  struct vnode *dir;

  if (!name_out || !type_out) {
    return -1;
  }

  if (vfs_lookup_path(path, &dir) < 0) {
    return -1;
  }

  if (dir->type != VNODE_DIR || !dir->ops || !dir->ops->readdir) {
    return -1;
  }

  return dir->ops->readdir(dir, index, name_out, name_out_size, type_out);
}
