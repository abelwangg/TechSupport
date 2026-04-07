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
    s->size = 0;
    s->capacity = 16;
    s->edits = (Edit*)malloc(s->capacity * sizeof(Edit));
}

/* TODO 11 */
void es_push(EditStack *s, Edit e) {
    if (s->size >= s->capacity) {
        s->capacity *= 2;
        s->edits = (Edit*)realloc(s->edits, s->capacity * sizeof(Edit));
    }
    s->edits[s->size] = e;
    s->size++;
}

/* TODO 12 */
Edit es_pop(EditStack *s) {
    if (es_empty(s)) {
        Edit dummy = {0}; //think we should initialize all to 0?
        return dummy;
    }
    s->size--;
    return s->edits[s->size];
}

/* TODO 13 */
int es_empty(EditStack *s) {
    return s->size == 0;
}

/* TODO 14 */
void es_clear(EditStack *s) {
    //don't free memory array here, reset the size tracker so existing memory can be safely overwritten
    s->size = 0;
}

/* provided -- do not modifyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy */
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
    q->front = NULL;
    q->rear = NULL;
    q->size = 0;
}

/* TODO 16 */
void q_enqueue(Queue *q, Node *node, int id) {
    //create new linked list node
    QueueNode *qn = (QueueNode*)malloc(sizeof(QueueNode));
    qn->treeNode = node;
    qn->id = id;
    qn->next = NULL;

    //if queue was empty, front and rear points to this new node
    if (q->rear == NULL) {
        q->front = qn;
        q->rear = qn;
    } else {    //else link to end of existing queue
        q->rear->next = qn;
        q->rear = qn;
    }

    q->size++;
}

/* TODO 17 */
int q_dequeue(Queue *q, Node **node, int *id) {
    if (q_empty(q)) {
        return 0;
    }

    //grab data from front node before deleting it
    QueueNode *temp = q->front;
    *node = temp->treeNode;
    *id = temp->id;

    //move front pointer back one
    q->front = q->front->next;

    //fromt HINT 17: if removing item made queue empty, rear pointer must also be set to NULL
    if (q->front == NULL) {
        q->rear = NULL;
    }

    free(temp);
    q->size--;
    
    return 1;
}

/* TODO 18 */
int q_empty(Queue *q) {
    return (q->front == NULL);
}

/* TODO 19 */
void q_free(Queue *q) {
    Node *dummyNode;
    int dummyId;

    while (!q_empty(q)) {
        q_dequeue(q, &dummyNode, &dummyId);
    }
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
    
    //hint 20 says output will never be longer than input string, so can
    //safely allocate memory equal to input's length + 1 (for null terminator)
    char* result = (char*)malloc(strlen(s) + 1);
    if (!result) return NULL;

    int j = 0;
    for (int i = 0; s[i] != '\0'; i++) {
        if (isalpha((unsigned char)s[i])) {
            result[j++] = tolower((unsigned char)s[i]); //make lowercase
            //the j++ increments j after this instruction executes i think
        } else if (s[i] == ' ') {
            result[j++] = '_'; //replace spaces
        }
        //if it's punctuation or number, do nothing and js drop it lol
    }
    //cap off string
    result[j] = '\0';
    
    return result;
}

/* TODO 21  (djb2: hash = hash*33 + c, seed 5381) */
unsigned h_hash(const char *s) {
    //famous djb2 alg by Dan Bernstein
    //good at distributing strings evenly across buckets
    unsigned hash = 5381;
    int c;

    //loops thru string char by char until it hits null terminator
    while ((c = *s++)) {
        //hash * 33 + c. bit shifting is faster way to multiply
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

/* TODO 22 */
void h_init(Hash *h, int nbuckets) {
    h->nbuckets = nbuckets;
    h->size = 0;
    //calloc bc initializing all the bucket pointers to NULL
    h->buckets = (Entry**)calloc(nbuckets, sizeof(Entry*));
}

/* TODO 23 */
//bruh
int h_put(Hash *h, const char *key, int solutionId) {
    unsigned idx = h_hash(key) % h->nbuckets;
    Entry* curr = h->buckets[idx];

    //case 1: key exists somewhere in this bucket's linked list
    while (curr != NULL) {
        //if we find word
        if (strcmp(curr->key, key) == 0) {
            //found key, check if solutionId is already attached
            for (int i =  0; i < curr->vals.count; i++) {
                if (curr->vals.ids[i] == solutionId) {
                    //already there, don't do anything
                    return 0;  
                }
            }

            //if Idlist full, double capacity
            if (curr->vals.count >= curr->vals.capacity) {
                curr->vals.capacity = ((curr->vals.capacity == 0) ? 4 : curr->vals.capacity * 2);   //ternary operator for simplicity
                curr->vals.ids = (int *)realloc(curr->vals.ids, curr->vals.capacity * sizeof(int));
            }

            //add new solutionId to the list
            curr->vals.ids[curr->vals.count] = solutionId;
            curr->vals.count++;
            return 1;
            
        }
        curr = curr->next;
    }

    //case 2: key does not exist yet. we must create new Entry bruh
    Entry *new_entry = (Entry*)malloc(sizeof(Entry));

    //hint23 says store a copy of string using strdup, not the pointer
    new_entry->key = strdup(key);

    new_entry->vals.capacity = 4;
    new_entry->vals.count = 1;
    new_entry->vals.ids = (int*)malloc(4 * sizeof(int)); //allocate this array
    new_entry->vals.ids[0] = solutionId;

    //push it to fromt of bucket's linked list
    new_entry->next = h->buckets[idx];
    h->buckets[idx] = new_entry;
    h->size++;

    return 1;

    //case 3:
}

/* TODO 24 */
int h_contains(const Hash *h, const char *key, int solutionId) {
    unsigned idx = h_hash(key) % h->nbuckets;
    Entry *curr = h->buckets[idx];

    while (curr != NULL) {
        //if find word
        if (strcmp(curr->key, key) == 0) {
            //check if the solutionId is in its list
            for (int i = 0; i < curr->vals.count; i++) {
                if (curr->vals.ids[i] == solutionId) return 1;
            }
            return 0; //found word, not this specific solution
        }
        curr = curr->next;
    }
    return 0; //word doesn't exist
}

/* TODO 25 */
int *h_get_ids(const Hash *h, const char *key, int *outCount) {
    //these two lines again
    unsigned idx = h_hash(key) % h->nbuckets;
    Entry *curr = h->buckets[idx];

    while (curr != NULL) {
        if (strcmp(curr->key, key) == 0) {
            //pass size back through the outCount pointer
            *outCount = curr->vals.count;
            return curr->vals.ids;
        }
        curr = curr->next;
    }
    
    *outCount = 0;
    return NULL;
}

/* TODO 26 */
void h_free(Hash *h) {
    if (!h || !h->buckets) return;

    //loop thru every bucket
    for (int i = 0; i < h->nbuckets; i++) {
        Entry *curr = h->buckets[i];

        //loop thru linked list in this bucket
        while (curr != NULL) {
            //move curr to next one
            Entry *temp = curr;
            curr = curr->next;

            //free everything
            //duplicated string key
            //dynamic arr of integers
            //entry struct itself
            free(temp->key);
            free(temp->vals.ids);
            free(temp);
        }
    }

    //almost forgot buckets array itself
    free(h->buckets);
    h->buckets = NULL;
    h->size = 0;
    h->nbuckets = 0;
}
