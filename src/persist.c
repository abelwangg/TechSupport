#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "lab4.h"

extern Node *g_root;

#define MAGIC   0x54454348u   /* "TECH" */
#define VERSION 1u

typedef struct { Node *node; int id; } NodeMapping;

/* ----------------------------------------------------------------
 * TODO 27  save_tree
 *
 * Serialize the entire tree to a binary file using BFS order.
 *
 * File format:
 *   Header:  uint32 magic | uint32 version | uint32 nodeCount
 *   Per node (BFS order):
 *     uint8  isQuestion
 *     uint32 textLen          (bytes, no null terminator in file)
 *     char[] text             (exactly textLen bytes)
 *     int32  yesId            (-1 if NULL)
 *     int32  noId             (-1 if NULL)
 *
 * Return 1 on success, 0 on failure.
 * ---------------------------------------------------------------- */
int save_tree(const char *filename) {
    return 0;
}

/* ----------------------------------------------------------------
 * TODO 28  load_tree
 *
 * Read a file written by save_tree and reconstruct the tree.
 * Validate the magic number.  Read all nodes into a flat array
 * first, then link children in a second pass.
 * Free any existing g_root before installing the new one.
 * Return 1 on success, 0 on any error (free partial allocations).
 * ---------------------------------------------------------------- */
int load_tree(const char *filename) {
    return 0;
}
