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
    if (g_root == NULL) return 0;

    //file stuff starts now
    FILE *f = fopen(filename, "wb");    //wb is write binrary
    if (!f) return 0;

    uint32_t magic = MAGIC;
    uint32_t version = VERSION;
    uint32_t nodeCount = count_nodes(g_root);

    //write header
    fwrite(&magic, sizeof(uint32_t), 1, f);
    fwrite(&version, sizeof(uint32_t), 1, f);
    fwrite(&nodeCount, sizeof(uint32_t), 1, f);

    //BFS queue
    Queue q;
    q_init(&q);

    int next_id = 0;

    //put root in queue and give it ID 0
    q_enqueue(&q, g_root, next_id++);

    Node *n;
    int id;

    //keep popping nodes out of the queue until it's completely empty
    while (q_dequeue(&q, &n, &id)) {
        //bruh
        //convert integer boolean into 1-bite unsigned int
        uint8_t isQuestion = (n->isQuestion ? 1 : 0);

        uint32_t textLen = strlen(n->text);

        int32_t yesId = -1;
        int32_t noId = -1;

        //if node has 'yes' child, assign child very next available ID
        //throw child to back of the queue
        if (n->yes) {
            yesId = next_id;
            q_enqueue(&q, n->yes, next_id++);
        }
        //no child same
        if (n->no) {
            noId = next_id;
            q_enqueue(&q, n->no, next_id++);
        }

        // write this node's data into binary file
        fwrite(&isQuestion, sizeof(uint8_t), 1, f);
        fwrite(&textLen, sizeof(uint32_t), 1, f);

        //will manually add '\0' back during load
        fwrite(n->text, 1, textLen, f);

        fwrite(&yesId, sizeof(int32_t), 1, f);
        fwrite(&noId, sizeof(int32_t), 1, f);
        
    }

    //clean up queue, close file, save to hard drive
    q_free(&q);
    fclose(f);
    return 1;

}

/* ----------------------------------------------------------------
 * TODO 28  load_tree
 *
 * Read a file written by save_tree and reconstruct the tree.
 * Validate the magic number.  Read all nodes into a flat array
 * first, then link children in a second pass.
 * 
 * Free any existing g_root before installing the new one.
 * 
 * Return 1 on success, 0 on any error (free partial allocations).
 * ---------------------------------------------------------------- */
int load_tree(const char *filename) {
    //rb: read binary, prevent OS char translation
    FILE *f = fopen(filename, "rb");
    if (!f) return 0;

    uint32_t magic, version, nodeCount;

    //read first 4 bytes, if fread doesn't return 1, file is broken. jump to error handling
    if (fread(&magic, sizeof(uint32_t), 1, f) != 1) goto error;

    if (magic != MAGIC) goto error;

    if (fread(&version, sizeof(uint32_t), 1, f) != 1) goto error;
    if (fread(&nodeCount, sizeof(uint32_t), 1, f) != 1) goto error;

    //if valid file but completely empty
    if (nodeCount == 0) {
        fclose(f);
        return 1;
    }

    //******************SET UP TWO-PASS RECONSTRUCTION
    //read everything into flat arrays first (pass 1)
    //then loop through arrays and link them together (pass 2)

    //arrays to hold physical node pointers
    Node **nodes = calloc(nodeCount, sizeof(Node*));
    //arrays to remember what child IDs each node is supposed to connect to
    int32_t *yesIds = calloc(nodeCount, sizeof(int32_t));
    int32_t *noIds = calloc(nodeCount, sizeof(int32_t));

    //computer ran out of memory?????
    if (!nodes || !yesIds || !noIds) goto error_alloc;

    //PASS 11111111111111111111111111111111111111111111
    //read all data from file

    for (uint32_t i = 0; i < nodeCount; i++) {
        uint8_t isQuestion;
        uint32_t textLen;

        //read basic info
        if (fread(&isQuestion, sizeof(uint8_t), 1, f) != 1) goto error_pass1;
        if (fread(&textLen, sizeof(uint32_t), 1, f) != 1) goto error_pass1;

        //temp buffer to hold text
        char *text = malloc(textLen + 1);
        if (!text) goto error_pass1;

        if (fread(text, 1, textLen, f) != textLen) {
            free(text);
            goto error_pass1;
        }

        text[textLen] = '\0';

        //read target IDs for children
        if (fread(&yesIds[i], sizeof(int32_t), 1, f) != 1) { 
            free(text); 
            goto error_pass1; 
        }

        if (fread(&noIds[i], sizeof(int32_t), 1, f) != 1) { 
            free(text); 
            goto error_pass1; 
        }

        //remember strdup
        if (isQuestion) {
            nodes[i] = create_question_node(text);
        } else {
            nodes[i] = create_solution_node(text);
        }

        free(text);
        if (!nodes[i]) goto error_pass1;

    }


    //PASS 22222222222222222222222222222222222222222222
    //linke tree together

    for (uint32_t i = 0; i < nodeCount; i++) {
        if (yesIds[i] != -1) {
            //point 'yes' pointer to node at that index
            nodes[i]->yes = nodes[yesIds[i]];
        }
        if (noIds[i] != -1) {
            //point 'no' pointer to node at that index
            nodes[i]->no = nodes[noIds[i]];
        }
    }


    //SUCCESFUL
    free_tree(g_root);  //destroy old tree
    g_root = nodes[0];  //bc BFS root is first node
    //free helper arrays
    free(nodes);
    free(yesIds);
    free(noIds);
    fclose(f);
    return 1;

    error_pass1:
        for (uint32_t i = 0; i < nodeCount; i++) {
            if (nodes[i]) free_tree(nodes[i]); 
        }
    error_alloc:
        //free helper arrays
        free(nodes);
        free(yesIds);
        free(noIds);
    error:
        fclose(f);
        return 0;

}
