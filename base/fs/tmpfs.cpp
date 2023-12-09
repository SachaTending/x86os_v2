#include "libc.hpp"
#include <vfs_drive_mgr.hpp>

struct tmpfs_node
{
    struct tmpfs_node *next;
    struct tmpfs_node *prev;
    bool is_dir;
    char *path;
    struct ext_file
    {
        void *data;
        uint32_t len;
    } file;
};

tmpfs_node *tmpfs_find_node(tmpfs_node *root, char *path) {
    tmpfs_node *node = root;
    while (node)
    {
        if (!strcmp(node->path, path)) {
            return node;
        }
        node = node->next;
    }
    return 0;
}

struct file_buffer tmpfs_read(char* path,uint64_t offset,uint64_t size, fs_driver *drv) {
    tmpfs_node *n = (tmpfs_node *)drv->priv_data;
    tmpfs_node *n2 = tmpfs_find_node(n, path);
    if (n2) {
        if (offset > n2->file.len) return;
        if (!n2->is_dir) {
            file_buffer buf;
            buf.data = malloc(size);
            memcpy(buf.data, (const void *)(((uint32_t)n2->file.data)+offset), size);
            return buf;
        }
    }
}

void tmpfs_insert(tmpfs_node *root, tmpfs_node *n) {
    tmpfs_node *node = root;
    while (node)
    {
        if (!node->next) {
            node->next = n;
            n->prev = node;
            return;
        }
        node = node->next;
    }
}

bool tmpfs_write(char* path,uint64_t off,struct file_buffer buf, fs_driver *drv) {
    tmpfs_node *root = (tmpfs_node *)drv->priv_data;
    tmpfs_node *node = tmpfs_find_node(root, path);
    size_t len = buf.size;
    if (!node) {
        node = (tmpfs_node *)malloc(sizeof(tmpfs_node));
        memset(node, 0, sizeof(tmpfs_node));
        node->path = strdup(path);
        tmpfs_insert(root, node);
    }
    if (!node->file.len) {
        node->file.data = malloc(1);
    }
    if (node->file.len < (off+len)) {
        void *dn = malloc(off+len);
        memcpy(dn, node->file.data, off+len);
        free(node->file.data);
        node->file.data = dn;
    }
    memcpy((void *)(((uint32_t)node->file.data)+off), buf.data, len);
}

void tmpfs_mkdir(char *path, struct vfs_mount *mnt) {
    tmpfs_node *root = (tmpfs_node *)mnt->prv_data;
    tmpfs_node *node = tmpfs_find_node(root, path);
    if (!node) {
        node = (tmpfs_node *)malloc(sizeof(tmpfs_node));
        memset(node, 0, sizeof(tmpfs_node));
        node->path = strdup((const char *)path);
        node->is_dir = true;
        tmpfs_insert(root, node);
    }
}

void tmpfs_inspect() {
    tmpfs_node *node = (tmpfs_node *)c_drive.prv_data;
    while (node) {
        if (node->path[0] == 0) goto end;
        printf("Path: %s Is dir: %d\n", node->path, node->is_dir);
        end:
        node = node->next;
    }
}

void tmpfs_init(char l) {
    fs_driver *fs = (fs_driver *)malloc(sizeof(fs_driver));
    fs->priv_data = malloc(sizeof(tmpfs_node));
    memset(fs->priv_data, 0, sizeof(tmpfs_node));
    tmpfs_node *n = (tmpfs_node *)fs->priv_data;
    n->is_dir = true;
    n->path = (char *)"";
    fs->read_file = tmpfs_read;
    fs->write_file = tmpfs_write;
    //fs->mkdir = tmpfs_mkdir;
}