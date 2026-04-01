#ifndef LAB4_H
#define LAB4_H

#include <stdint.h>

/* ========== Tree Node ========== */
/*
 * A Node is either:
 *   - A QUESTION node (isQuestion == 1): an internal node with a yes/no question.
 *     Both yes and no children must be non-NULL.
 *   - A SOLUTION node (isQuestion == 0): a leaf node with a fix/solution string.
 *     Both yes and no children must be NULL.
 */
typedef struct Node {
    char *text;           /* question text OR solution text */
    struct Node *yes;     /* child taken when user answers YES */
    struct Node *no;      /* child taken when user answers NO  */
    int isQuestion;       /* 1 = question node, 0 = solution leaf */
} Node;

/* Node constructors / destructor */
Node *create_question_node(const char *question);
Node *create_solution_node(const char *solution);
void  free_tree(Node *node);
int   count_nodes(Node *root);

/* ========== Stack for Diagnosis Traversal ========== */
typedef struct Frame {
    Node *node;
    int   answeredYes;   /* -1 = unset, 0 = no, 1 = yes */
} Frame;

typedef struct {
    Frame *frames;
    int    size;
    int    capacity;
} FrameStack;

void  fs_init(FrameStack *s);
void  fs_push(FrameStack *s, Node *node, int answeredYes);
Frame fs_pop(FrameStack *s);
int   fs_empty(FrameStack *s);
void  fs_free(FrameStack *s);

/* ========== Edit / Undo / Redo ========== */
typedef enum {
    EDIT_INSERT_SPLIT
} EditType;

typedef struct {
    EditType  type;
    Node     *parent;        /* NULL if root was replaced */
    int       wasYesChild;   /* 1=yes branch, 0=no branch, -1=root */
    Node     *oldLeaf;       /* solution leaf that was replaced   */
    Node     *newQuestion;   /* new question node inserted        */
    Node     *newLeaf;       /* new solution leaf created         */
} Edit;

typedef struct {
    Edit *edits;
    int   size;
    int   capacity;
} EditStack;

void es_init(EditStack *s);
void es_push(EditStack *s, Edit e);
Edit es_pop(EditStack *s);
int  es_empty(EditStack *s);
void es_clear(EditStack *s);
void es_free(EditStack *s);
void free_edit_stack(EditStack *s);

extern EditStack g_undo;
extern EditStack g_redo;
extern Node     *g_root;

int undo_last_edit(void);
int redo_last_edit(void);

/* ========== Queue for BFS ========== */
typedef struct QueueNode {
    Node            *treeNode;
    int              id;
    struct QueueNode *next;
} QueueNode;

typedef struct {
    QueueNode *front;
    QueueNode *rear;
    int        size;
} Queue;

void q_init(Queue *q);
void q_enqueue(Queue *q, Node *node, int id);
int  q_dequeue(Queue *q, Node **node, int *id);
int  q_empty(Queue *q);
void q_free(Queue *q);

/* ========== Symptom Index (Hash Table) ========== */
typedef struct IdList {
    int *ids;
    int  count;
    int  capacity;
} IdList;

typedef struct Entry {
    char         *key;
    IdList        vals;
    struct Entry *next;
} Entry;

typedef struct {
    Entry **buckets;
    int     nbuckets;
    int     size;
} Hash;

void      h_init(Hash *h, int nbuckets);
unsigned  h_hash(const char *s);
int       h_put(Hash *h, const char *key, int solutionId);
int       h_contains(const Hash *h, const char *key, int solutionId);
int      *h_get_ids(const Hash *h, const char *key, int *outCount);
void      h_free(Hash *h);
char     *canonicalize(const char *s);

/* UI helpers (implemented in main.c, declared here for game.c) */
int    get_yes_no(int y, int x, const char *prompt);
char  *get_input(int y, int x, const char *prompt);

extern Hash g_index;

/* ========== Persistence ========== */
int save_tree(const char *filename);
int load_tree(const char *filename);

/* ========== Utilities ========== */
int  check_integrity(void);
void find_shortest_path(const char *sol1, const char *sol2);

/* ========== Diagnosis Session ========== */
void run_diagnosis(void);

/* ========== Visualization ========== */
void draw_tree(void);

#endif /* LAB4_H */
