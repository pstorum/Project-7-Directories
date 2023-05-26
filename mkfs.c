#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mkfs.h"
#include "image.h"
#include "block.h"


void mkfs(void){
    //set all blocks to zero
    char buffer[BLOCK_SIZE] = {0};
    for(int i = 0; i < NUM_BLOCKS; i++){
        write(image_fd, buffer, sizeof(buffer));
    }
    
    //allocate first 7 free blocks
    for(int i = 0; i < NUM_ALLOC_BLOCKS; i++){
        alloc();
    }

    struct inode *free_inode = ialloc();
    int data_block = alloc();

    free_inode->flags = DIRECTORY_FLAG;
    free_inode->size = INIT_DIRECTORY_SIZE;
    free_inode->block_ptr[0] = data_block;

    unsigned char block[BLOCK_SIZE];
    bread(data_block, block);

    write_u16(block+(DIRECTORY_ENTRY_SIZE*0), free_inode->inode_num);
    strcpy((char*)block+FILE_NAME_OFFSET+(DIRECTORY_ENTRY_SIZE*0), ".");
    write_u16(block+(DIRECTORY_ENTRY_SIZE*1), free_inode->inode_num);
    strcpy((char*)block+FILE_NAME_OFFSET+(DIRECTORY_ENTRY_SIZE*1), "..");

    bwrite(data_block, block);
    iput(free_inode);
}


struct directory *directory_open(int inode_num){
    struct inode *directory_node = iget(inode_num);
    if(directory_node == NULL){
        return NULL;
    }

    struct directory *directory = (struct directory *)malloc(sizeof(struct directory));
    directory->inode = directory_node;
    directory->offset = 0;
    return directory;

}

int directory_get(struct directory *dir, struct directory_entry *ent){
    if(dir->offset >= dir->inode->size){
        return -1;
    }

    unsigned char block[BLOCK_SIZE];

    int data_block_index = dir->offset / BLOCK_SIZE;
    int data_block_num = dir->inode->block_ptr[data_block_index];
    bread(data_block_num, block);

    int offset_in_block = dir->offset % BLOCK_SIZE;

    ent->inode_num = read_u16(block + offset_in_block);
    strcpy(ent->name, (char*)block+offset_in_block+2);

    dir->offset += 32;

    return 0;
}

void directory_close(struct directory *d){
    iput(d->inode);
    free(d);
}