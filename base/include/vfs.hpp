#pragma once
#include "vfs_defs.hpp"
bool vfs_is_dir(char* path);
bool vfs_mount(uint16_t disk_id,char* path);
bool vfs_load_contents(struct node* node,char* path);
struct node* vfs_inspect(char* path);
struct dir_report vfs_iterate_dir(char* path);
struct file_buffer vfs_read(char* path,uint64_t offset,uint64_t size);
void vfs_write(char* path,struct file_buffer buf,uint64_t offset);