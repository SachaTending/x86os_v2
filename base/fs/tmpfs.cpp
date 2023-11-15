#include <vfs_drive_mgr.hpp>
#include <libc.hpp>
#include <logger.hpp>

static logger log("TMPFS");

typedef struct tmpfs_node_file {
    void *p;
    size_t size;
} tmpfs_node_file_t;

typedef struct tmpfs_node {
    const char *path;
    bool is_dir;
    tmpfs_node_file_t file_ex;
    tmpfs_node_t *next;
} tmpfs_node_t;



struct dir_report tmpfs_iterate_dir(char *path, fs_driver *d) {
    log.info("path: %s\n", path);
    dir_report rep;
    rep.num_entries = 1;
    rep.entries = (node *)malloc(sizeof(node));
    rep.entries->name = "SUS";
    rep.entries->flags = FLAGS_PRESENT;
    return rep;
}

struct file_buffer tmpfs_read_file(char* path,uint64_t offset,uint64_t size, fs_driver *drv) {
    file_buffer f = {
        .data = 0,
        .size = 0
    };
    return f;
}

bool tmpfs_write_file (char* path,uint64_t offset,struct file_buffer buf, fs_driver *drv) {
    return false;
}

struct node tmpfs_inspect_dir(char* path, fs_driver *drv) {
    return {};
}

fs_driver tmpfs_gen_drv() {
    fs_driver drv = {
        .present = true,
        .inspect = tmpfs_inspect_dir,
        .iterate_dir = tmpfs_iterate_dir,
        .read_file = tmpfs_read_file,
        .write_file = tmpfs_write_file,
        .priv_data = 0
    };
    drv.priv_data = malloc(sizeof(tmpfs_node_t));
    memset(drv.priv_data, 0, sizeof(tmpfs_node_t));
    tmpfs_node_t *tmp_node = (tmpfs_node_t *)drv.priv_data;
    tmp_node->path = "/";
    tmp_node->is_dir = true;
    tmp_node->next = 0;
    return drv;
}

void tmpfs_destroy_drv(fs_driver *drv) {
    tmpfs_node_t *tmp_node = (tmpfs_node_t *)drv->priv_data;
    tmpfs_node_t *n = tmp_node;
    while (tmp_node) {
        n = tmp_node->next;
        memset(tmp_node->file_ex.p, 0, tmp_node->file_ex.size);
        free(tmp_node->file_ex.p);
        memset(tmp_node, 0, sizeof(tmpfs_node_t));
        free((void *)tmp_node);
        tmp_node = n;
    }
}

fs_driver drv = {
    .present = true,
    .inspect = tmpfs_inspect_dir,
    .iterate_dir = tmpfs_iterate_dir,
    .read_file = tmpfs_read_file,
    .write_file = tmpfs_write_file,
    .priv_data = 0
};