#include "fs.h"
#include "../lib/string.h"

#define RAMFS_NAME_MAX 32
#define RAMFS_CHILDREN_MAX 16
#define RAMFS_FILE_MAX 1024
#define RAMFS_MAX_NODES 128

struct ramfs_node {
  struct vnode vnode;
  char name[RAMFS_NAME_MAX];
  struct ramfs_node *parent;
  struct ramfs_node *children[RAMFS_CHILDREN_MAX];
  unsigned int child_count;
  char data[RAMFS_FILE_MAX];
};

static int ramfs_lookup(struct vnode *dir, const char *name, struct vnode **out);
static int ramfs_create(struct vnode *dir, const char *name, enum vnode_type type,
                        struct vnode **out);
static int ramfs_read(struct vnode *node, unsigned int offset, char *buf,
                      unsigned int size);
static int ramfs_write(struct vnode *node, unsigned int offset, const char *buf,
                       unsigned int size);
static int ramfs_readdir(struct vnode *dir, unsigned int index, char *name_out,
                         unsigned int name_out_size,
                         enum vnode_type *type_out);

static struct fs_ops ramfs_ops = {
    .lookup = ramfs_lookup,
    .create = ramfs_create,
    .read = ramfs_read,
    .write = ramfs_write,
    .readdir = ramfs_readdir,
};

static struct ramfs_node ramfs_nodes[RAMFS_MAX_NODES];
static int ramfs_nodes_used[RAMFS_MAX_NODES];
static struct vnode *ramfs_root_node;

static struct ramfs_node *ramfs_alloc_node(void) {
  unsigned int i;

  for (i = 0; i < RAMFS_MAX_NODES; i++) {
    if (!ramfs_nodes_used[i]) {
      ramfs_nodes_used[i] = 1;
      memset(&ramfs_nodes[i], 0, sizeof(ramfs_nodes[i]));
      ramfs_nodes[i].vnode.ops = &ramfs_ops;
      ramfs_nodes[i].vnode.internal = &ramfs_nodes[i];
      return &ramfs_nodes[i];
    }
  }

  return 0;
}

static struct ramfs_node *ramfs_from_vnode(struct vnode *node) {
  return (struct ramfs_node *)node->internal;
}

static struct ramfs_node *ramfs_find_child(struct ramfs_node *dir,
                                           const char *name) {
  unsigned int i;

  if (!dir || dir->vnode.type != VNODE_DIR) {
    return 0;
  }

  for (i = 0; i < dir->child_count; i++) {
    if (strcmp(dir->children[i]->name, name) == 0) {
      return dir->children[i];
    }
  }

  return 0;
}

int ramfs_init(void) {
  struct ramfs_node *root;

  root = ramfs_alloc_node();
  if (!root) {
    return -1;
  }

  root->vnode.type = VNODE_DIR;
  root->vnode.size = 0;
  root->name[0] = '\0';
  root->parent = 0;
  ramfs_root_node = &root->vnode;
  return 0;
}

struct vnode *ramfs_root(void) { return ramfs_root_node; }

static int ramfs_lookup(struct vnode *dir, const char *name, struct vnode **out) {
  struct ramfs_node *dir_node;
  struct ramfs_node *child;

  if (!dir || !name || !out) {
    return -1;
  }

  dir_node = ramfs_from_vnode(dir);
  child = ramfs_find_child(dir_node, name);
  if (!child) {
    return -1;
  }

  *out = &child->vnode;
  return 0;
}

static int ramfs_create(struct vnode *dir, const char *name, enum vnode_type type,
                        struct vnode **out) {
  struct ramfs_node *dir_node;
  struct ramfs_node *node;

  if (!dir || !name || !out) {
    return -1;
  }

  dir_node = ramfs_from_vnode(dir);
  if (dir_node->vnode.type != VNODE_DIR) {
    return -1;
  }

  if (dir_node->child_count >= RAMFS_CHILDREN_MAX) {
    return -1;
  }

  if (ramfs_find_child(dir_node, name)) {
    return -1;
  }

  node = ramfs_alloc_node();
  if (!node) {
    return -1;
  }

  node->vnode.type = type;
  node->vnode.size = 0;
  strcpy(node->name, name);
  node->parent = dir_node;

  dir_node->children[dir_node->child_count++] = node;
  *out = &node->vnode;
  return 0;
}

static int ramfs_read(struct vnode *node, unsigned int offset, char *buf,
                      unsigned int size) {
  struct ramfs_node *ram_node;
  unsigned int i;

  if (!node || !buf || node->type != VNODE_FILE) {
    return -1;
  }

  ram_node = ramfs_from_vnode(node);

  if (offset >= ram_node->vnode.size) {
    return 0;
  }

  if (offset + size > ram_node->vnode.size) {
    size = ram_node->vnode.size - offset;
  }

  for (i = 0; i < size; i++) {
    buf[i] = ram_node->data[offset + i];
  }

  return (int)size;
}

static int ramfs_write(struct vnode *node, unsigned int offset, const char *buf,
                       unsigned int size) {
  struct ramfs_node *ram_node;
  unsigned int i;

  if (!node || !buf || node->type != VNODE_FILE) {
    return -1;
  }

  ram_node = ramfs_from_vnode(node);

  if (offset >= RAMFS_FILE_MAX) {
    return 0;
  }

  if (offset + size > RAMFS_FILE_MAX) {
    size = RAMFS_FILE_MAX - offset;
  }

  for (i = 0; i < size; i++) {
    ram_node->data[offset + i] = buf[i];
  }

  if (offset + size > ram_node->vnode.size) {
    ram_node->vnode.size = offset + size;
  }

  return (int)size;
}

static int ramfs_readdir(struct vnode *dir, unsigned int index, char *name_out,
                         unsigned int name_out_size,
                         enum vnode_type *type_out) {
  struct ramfs_node *dir_node;
  struct ramfs_node *child;
  unsigned int i;

  if (!dir || !name_out || !type_out || dir->type != VNODE_DIR) {
    return -1;
  }

  dir_node = ramfs_from_vnode(dir);
  if (index >= dir_node->child_count) {
    return -1;
  }

  child = dir_node->children[index];
  for (i = 0; child->name[i] != '\0'; i++) {
    if (i + 1 >= name_out_size) {
      return -1;
    }

    name_out[i] = child->name[i];
  }

  name_out[i] = '\0';
  *type_out = child->vnode.type;
  return 0;
}
