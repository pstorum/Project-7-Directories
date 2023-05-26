#ifndef MKFS_H
#define MKFS_H

#include "image.h"
#include "block.h"
#include "inode.h"

#define NUM_BLOCKS 1024
#define NUM_ALLOC_BLOCKS 7
#define DIRECTORY_FLAG 2
#define INIT_DIRECTORY_SIZE 64
#define DIRECTORY_ENTRY_SIZE 32
#define FILE_NAME_OFFSET 2

struct directory {
    struct inode *inode;
    unsigned int offset;
};

struct directory_entry {
    unsigned int inode_num;
    char name[16];
};

void mkfs(void);
struct directory *directory_open(int inode_num);
int directory_get(struct directory *dir, struct directory_entry *ent);
void directory_close(struct directory *d);

#endif