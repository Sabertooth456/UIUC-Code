/**
 * finding_filesystems
 * CS 341 - Spring 2024
 */
#include "minixfs.h"
#include "minixfs_utils.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>

// bool
#include <stdbool.h>

// Yoinked from StackExchange XDDDDDDDDDD
 #define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

 #define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

/**
 * Virtual paths:
 *  Add your new virtual endpoint to minixfs_virtual_path_names
 */
char *minixfs_virtual_path_names[] = {"info", /* add your paths here*/};

/**
 * Forward declaring block_info_string so that we can attach unused on it
 * This prevents a compiler warning if you haven't used it yet.
 *
 * This function generates the info string that the virtual endpoint info should
 * emit when read
 */
static char *block_info_string(ssize_t num_used_blocks) __attribute__((unused));
static char *block_info_string(ssize_t num_used_blocks) {
    char *block_string = NULL;
    ssize_t curr_free_blocks = DATA_NUMBER - num_used_blocks;
    asprintf(&block_string,
             "Free blocks: %zd\n"
             "Used blocks: %zd\n",
             curr_free_blocks, num_used_blocks);
    return block_string;
}

// Don't modify this line unless you know what you're doing
int minixfs_virtual_path_count =
    sizeof(minixfs_virtual_path_names) / sizeof(minixfs_virtual_path_names[0]);

int minixfs_chmod(file_system *fs, char *path, int new_permissions) {
    // Thar she blows!
    inode *fs_inode = get_inode(fs, path);
    if (fs_inode == NULL) {
        errno = ENOENT;
        return -1;
    }

    uint16_t *fs_mode = &(fs_inode->mode);
    *fs_mode = *fs_mode >> RWX_BITS_NUMBER << RWX_BITS_NUMBER;   // This should clear the last RWX_BITS_NUMBER bits of the inode's mode
    *fs_mode = *fs_mode | new_permissions;

    clock_gettime(CLOCK_REALTIME, &(fs_inode->ctim));
    return 0;
}

int minixfs_chown(file_system *fs, char *path, uid_t owner, gid_t group) {
    // Land ahoy!
    inode *fs_inode = get_inode(fs, path);
    if (fs_inode == NULL) {
        errno = ENOENT;
        return -1;
    }

    if (owner != (uid_t) - 1) {
        fs_inode->uid = owner;
    }
    if (group != (gid_t) - 1) {
        fs_inode->gid = group;
    }
    clock_gettime(CLOCK_REALTIME, &(fs_inode->ctim));
    return 0;
}

inode *minixfs_create_inode_for_path(file_system *fs, const char *path) {
    // Land ahoy!
    // Check that the file has not been created
    if (get_inode(fs, path) != NULL) {
        return NULL;
    }

    // Get parent directory, and check if the filename is valid
    const char *f_name;
    inode *p_inode = parent_directory(fs, path, &f_name);
    if (p_inode == NULL || !valid_filename(f_name)) {
        return NULL;
    }

    // Get the index of the first unused inode in the filesystem
    inode_number new_inode_index = first_unused_inode(fs);
    if (new_inode_index == -1) {
        return NULL;
    }

    // Initialize the new inode
    inode *new_inode = fs->inode_root + new_inode_index;
    init_inode(p_inode, new_inode);

    // Add the new inode to the parent's direct directory
    bool inode_assigned = false;
    for (size_t i = 0; i < NUM_DIRECT_BLOCKS; i++) {
        if (p_inode->direct[i] == UNASSIGNED_NODE) {
            p_inode->direct[i] = new_inode_index;
            inode_assigned = true;
        }
    }

    // If the parent's direct directory is full, add it to the parent's indirect directory
    if (!inode_assigned) {
        if (p_inode->indirect == UNASSIGNED_NODE) {
            p_inode->indirect = first_unused_inode(fs);
            if (p_inode->indirect == -1) {
                return NULL;
            }
            set_data_used(fs, p_inode->indirect, true);
            data_block_number *p_block = (data_block_number *) fs->data_root[p_inode->indirect].data;
            // Initialize the data_block to unassigned data_block_numbers
            for (size_t i = 0; i < sizeof(data_block) / sizeof(data_block_number); i++) {
                p_block[i] = UNASSIGNED_NODE;
            }
        }
        
        data_block_number *p_block = (data_block_number *) fs->data_root[p_inode->indirect].data;
        for (size_t i = 0; i < sizeof(data_block) / sizeof(data_block_number); i++) {
            if (p_block[i] == UNASSIGNED_NODE) {
                p_block[i] = new_inode_index;
                inode_assigned = true;
                break;
            }
        }
    }
    // If the inode can't be assigned, destroy it and return -1
    if (!inode_assigned) {
        free_inode(fs, new_inode);
        return NULL;
    }
    return new_inode;
}

ssize_t minixfs_virtual_read(file_system *fs, const char *path, void *buf,
                             size_t count, off_t *off) {
    if (!strcmp(path, "info")) {
        // TODO implement the "info" virtual file here
        superblock* fs_meta = fs->meta;
        size_t used_blocks = 0;
        char* data_map = GET_DATA_MAP(fs_meta);
        for (size_t i = 0; i < fs_meta->dblock_count; i++) {
            if (data_map[i] == 1) {
                used_blocks++;
            }
        }
        char *info = block_info_string(used_blocks);
        if ((size_t) *off >= strlen(info)) {
            return 0;
        }

        size_t to_read = min(count, strlen(info) - (size_t) *off);
        memcpy(buf, info + *off, to_read);
        return to_read;
    }

    errno = ENOENT;
    return -1;
}

ssize_t minixfs_write(file_system *fs, const char *path, const void *buf,
                      size_t count, off_t *off) {
    // X marks the spot

    // Make sure the file exists
    inode *file_inode = get_inode(fs, path);
    if (file_inode == NULL) {
        file_inode = minixfs_create_inode_for_path(fs, path);
        if (file_inode == NULL) {
            return -1;
        }
    }

    // Allocate the minimum amount of required blocks to store the written data
    size_t required_blocks = (count + *off + (sizeof(data_block) - 1)) / sizeof(data_block);
    if (minixfs_min_blockcount(fs, path, required_blocks) == -1) {
        errno = ENOSPC;
        return -1;
    }

    // Find the starting data_block and offset that will be used for writing
    size_t cur_block_index = *off / sizeof(data_block);
    size_t cur_block_offset = *off % sizeof(data_block);
    size_t bytes_written = 0;
    while(bytes_written < count) {
        // Get the current block of data
        data_block cur_block;
        if (cur_block_index < NUM_DIRECT_BLOCKS) {
            cur_block = fs->data_root[file_inode->direct[cur_block_index]];
        } else {
            data_block indirect_block = fs->data_root[file_inode->indirect];
            size_t indirect_index = cur_block_index - NUM_DIRECT_BLOCKS;
            cur_block = fs->data_root[((data_block_number *) (indirect_block.data))[indirect_index]];
        }

        // Calculate the amount of bytes that should be written to the current block
        size_t to_write;
        if (cur_block_offset > 0) {
            // This is only true on the first write, when the block hasn't been completely filled
            to_write = min(sizeof(data_block) - cur_block_offset, count);
        } else if (cur_block_offset > 0 && bytes_written != 0) {
            perror("minixfs_write: This should not happen\n");
            return -1;
        } else {
            to_write = min(count - bytes_written, sizeof(data_block));
        }

        // Copy over the data
        memcpy(cur_block.data + cur_block_offset, buf + bytes_written, to_write);

        // Update the bytes that have been written
        bytes_written += to_write;

        // Update the offset for the next write
        cur_block_offset += to_write;
        if (cur_block_offset == sizeof(data_block)) {
            cur_block_index++;
            cur_block_offset = 0;
        } else if (cur_block_offset > sizeof(data_block)) {
            perror("minixfs_write: Block overflow\n");
            return -1;
        }
    }

    // Set the new size of the inode
    file_inode->size = *off + count;

    clock_gettime(CLOCK_REALTIME, &(file_inode->atim));
    clock_gettime(CLOCK_REALTIME, &(file_inode->mtim));
    return bytes_written;
}

ssize_t minixfs_read(file_system *fs, const char *path, void *buf, size_t count,
                     off_t *off) {
    const char *virtual_path = is_virtual_path(path);
    if (virtual_path)
        return minixfs_virtual_read(fs, virtual_path, buf, count, off);
    // 'ere be treasure!

    inode *file_inode = get_inode(fs, path);
    if (file_inode == NULL) {
        errno = ENOENT;
        return -1;
    }

    // Check if the offset is at an index that exceeds the size of the file
    if (file_inode->size >= (uint64_t) *off) {
        return 0;
    }

    // Calculate the maximum number of readable bytes
    size_t max_count = min(count, file_inode->size - *off);

    size_t cur_block_index = *off / sizeof(data_block);
    size_t cur_block_offset = *off % sizeof(data_block);
    size_t bytes_read = 0;
    while(bytes_read < max_count) {
        // Get the current block
        data_block cur_block;
        if (cur_block_index < NUM_DIRECT_BLOCKS) {
            cur_block = fs->data_root[file_inode->direct[cur_block_index]];
        } else {
            data_block indirect_block = fs->data_root[file_inode->indirect];
            size_t indirect_index = cur_block_index - NUM_DIRECT_BLOCKS;
            cur_block = fs->data_root[((data_block_number *) (indirect_block.data))[indirect_index]];
        }
        
        // Calculate the amount of bytes that should be read to the buffer
        size_t to_read;
        if (cur_block_offset > 0) {
            // This is only true on the first read, when the block hasn't been completely filled
            to_read = min(sizeof(data_block) - cur_block_offset, count);
        } else if (cur_block_offset > 0 && bytes_read != 0) {
            perror("minixfs_read: This should not happen\n");
            return -1;
        } else {
            to_read = min(count - bytes_read, sizeof(data_block));
        }

        // Copy over the data
        memcpy(buf + bytes_read, cur_block.data + cur_block_offset, to_read);

        // Update the bytes that have been written
        bytes_read += to_read;

        // Update the offset for the next write
        cur_block_offset += to_read;
        if (cur_block_offset == sizeof(data_block)) {
            cur_block_index++;
            cur_block_offset = 0;
        } else if (cur_block_offset > sizeof(data_block)) {
            perror("minixfs_write: Block overflow\n");
            return -1;
        }
    }
    return -1;
}
