#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lab4.h"

/* ----------------------------------------------------------------
 * ds.c  --  all data structures for the Tech Support Diagnosis Tool
 *
 * Implement every function marked TODO.  The only functions in this
 * entire lab permitted to use recursion are free_tree and count_nodes.
 * Everything else must be iterative.
 * ---------------------------------------------------------------- */


/* ====== Tree nodes ============================================== */

/* TODO 1 */
Node *create_question_node(const char *question) {
    Node *n = (Node*)malloc(sizeof(Node));
    if (!n) {
        return NULL;
    }

    n->text = strdup(question);
    n->yes = NULL;
    n->no = NULL;
    n->isQuestion = 1;

    return n;
}

/* TODO 2 */
Node *create_solution_node(const char *solution) {
    Node *n = (Node*)malloc(sizeof(Node));
    if (!n) return NULL;

    n->text = strdup(solution);
    n->yes = NULL;
    n->no = NULL;
    n->isQuestion = 0;

    return n;
}

/* TODO 3  (recursion allowed) */
void free_tree(Node *node) {
    if (node == NULL) return;

    //have to do postorder, free children before parent
    free_tree(node->yes);
    free_tree(node->no);

    //free duplicated string, then node itself
    free(node->text);
    free(node);
}

/* TODO 4  (recursion allowed) */
int count_nodes(Node *root) {
    if (root == NULL) return 0;
    return (1 + count_nodes(root->yes) + count_nodes(root->no));
}


/* ====== FrameStack  (dynamic array, iterative traversal) ======== */

/* TODO 5 */
void fs_init(FrameStack *s) {
    s->size = 0;
    s->capacity = 16;   //16 prob a good starting default
    s->frames = (Frame*)malloc(s->capacity * sizeof(Frame));
}

/* TODO 6 */
void fs_push(FrameStack *s, Node *node, int answeredYes) {
    //check if capacity hit if so double
    if (s->size >= s->capacity) {
        s->capacity *= 2;
        s->frames = (Frame*)realloc(s->frames, s->capacity * sizeof(Frame));

    }

    //do actual function
    s->frames[s->size].node = node;
    s->frames[s->size].answeredYes = answeredYes;
    s->size++;
}

/* TODO 7 */
Frame fs_pop(FrameStack *s) {
    if (fs_empty(s)) {
        Frame dummy = {NULL, -1};
        return dummy;
    }

    s->size--;
    return s->frames[s->size];
}

/* TODO 8 */
int fs_empty(FrameStack *s) {
    return (s->size == 0);
}

/* TODO 9 */
void fs_free(FrameStack *s) {
    free(s->frames);
    s->frames = NULL;
    s->size = 0;
    s->capacity = 0;
}


/* ====== EditStack  (dynamic array, undo/redo) =================== */

/* TODO 10 */
void es_init(EditStack *s) {
}

/* TODO 11 */
void es_push(EditStack *s, Edit e) {
}

/* TODO 12 */
Edit es_pop(EditStack *s) {
    Edit dummy = {0};
    return dummy;
}

/* TODO 13 */
int es_empty(EditStack *s) {
    return 1;
}

/* TODO 14 */
void es_clear(EditStack *s) {
}

/* provided -- do not modify */
void es_free(EditStack *s) {
    free(s->edits);
    s->edits    = NULL;
    s->size     = 0;
    s->capacity = 0;
}

void free_edit_stack(EditStack *s) { es_free(s); }


/* ====== Queue  (linked list, BFS) ============================== */

/* TODO 15 */
void q_init(Queue *q) {
}

/* TODO 16 */
void q_enqueue(Queue *q, Node *node, int id) {
}

/* TODO 17 */
int q_dequeue(Queue *q, Node **node, int *id) {
    return 0;
}

/* TODO 18 */
int q_empty(Queue *q) {
    return 1;
}

/* TODO 19 */
void q_free(Queue *q) {
}


/* ====== Hash table  (separate chaining) ======================== */

/* TODO 20
 * Convert a string to a canonical key:
 *   letters  -> lowercase
 *   spaces   -> underscore
 *   anything else -> drop
 * Caller owns the returned string and must free() it.
 */
char *canonicalize(const char *s) {
    if (s == NULL) return strdup("");
    return NULL;
}

/* TODO 21  (djb2: hash = hash*33 + c, seed 5381) */
unsigned h_hash(const char *s) {
    return 0;
}

/* TODO 22 */
void h_init(Hash *h, int nbuckets) {
}

/* TODO 23 */
int h_put(Hash *h, const char *key, int solutionId) {
    return 0;
}

/* TODO 24 */
int h_contains(const Hash *h, const char *key, int solutionId) {
    return 0;
}

/* TODO 25 */
int *h_get_ids(const Hash *h, const char *key, int *outCount) {
    *outCount = 0;
    return NULL;
}

/* TODO 26 */
void h_free(Hash *h) {
}
