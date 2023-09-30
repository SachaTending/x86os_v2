#include <libc.hpp>
#include <stdint.h>

struct vfs_node;

typedef int (*read_t)(vfs_node *node, int count, void *out);
typedef int (*tell_t)(vfs_node *node);

struct vfs_node
{
    int flags;
    const char *path;
};
