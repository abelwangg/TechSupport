#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "lab4.h"

void test_nodes(void) {
    printf("Testing node functions...\n");
    Node *q = create_question_node("Is it plugged in?");
    assert(q && q->isQuestion == 1);
    assert(strcmp(q->text, "Is it plugged in?") == 0);
    assert(q->yes == NULL && q->no == NULL);

    Node *s = create_solution_node("Plug it in.");
    assert(s && s->isQuestion == 0);
    assert(strcmp(s->text, "Plug it in.") == 0);

    q->yes = s;
    q->no  = create_solution_node("Check the power button.");
    assert(count_nodes(q) == 3);
    assert(count_nodes(NULL) == 0);

    free_tree(q);
    printf("  OK\n");
}

void test_stack(void) {
    printf("Testing FrameStack...\n");
    FrameStack s;
    fs_init(&s);
    assert(fs_empty(&s));

    Node d1 = {0}, d2 = {0};
    fs_push(&s, &d1, 1);
    fs_push(&s, &d2, 0);
    assert(s.size == 2 && !fs_empty(&s));

    Frame f = fs_pop(&s);
    assert(f.node == &d2 && f.answeredYes == 0);
    f = fs_pop(&s);
    assert(f.node == &d1 && f.answeredYes == 1);
    assert(fs_empty(&s));

    for (int i = 0; i < 100; i++) fs_push(&s, &d1, i % 2);
    assert(s.size == 100 && s.capacity >= 100);
    fs_free(&s);
    printf("  OK\n");
}

void test_edit_stack(void) {
    printf("Testing EditStack...\n");
    EditStack s;
    es_init(&s);
    assert(es_empty(&s));

    Edit e1 = {0}; e1.parent = NULL;
    Edit e2 = {0}; e2.parent = (Node *)0xDEAD;
    es_push(&s, e1); es_push(&s, e2);

    Edit p = es_pop(&s); assert(p.parent == (Node *)0xDEAD);
    p = es_pop(&s);      assert(p.parent == NULL);
    assert(es_empty(&s));

    es_push(&s, e1); es_push(&s, e2);
    es_clear(&s);
    assert(s.size == 0 && es_empty(&s));
    es_free(&s);
    printf("  OK\n");
}

void test_queue(void) {
    printf("Testing Queue...\n");
    Queue q;
    q_init(&q);
    assert(q_empty(&q));

    Node d1 = {0}, d2 = {0}, d3 = {0};
    q_enqueue(&q, &d1, 1);
    q_enqueue(&q, &d2, 2);
    q_enqueue(&q, &d3, 3);

    Node *n; int id;
    assert(q_dequeue(&q, &n, &id) && n == &d1 && id == 1);
    assert(q_dequeue(&q, &n, &id) && n == &d2 && id == 2);
    assert(q_dequeue(&q, &n, &id) && n == &d3 && id == 3);
    assert(q_empty(&q) && q.rear == NULL);
    assert(!q_dequeue(&q, &n, &id));

    q_enqueue(&q, &d1, 99);
    assert(q.size == 1);
    q_free(&q);
    printf("  OK\n");
}

void test_canonicalize(void) {
    printf("Testing canonicalize...\n");
    char *c;

    c = canonicalize("Is the WiFi light on?");
    assert(strcmp(c, "is_the_wifi_light_on") == 0); free(c);

    c = canonicalize("OVERHEATING??");
    assert(strcmp(c, "overheating") == 0); free(c);

    c = canonicalize("ABC123");
    assert(strcmp(c, "abc") == 0); free(c);   /* digits dropped */

    c = canonicalize("");
    assert(strcmp(c, "") == 0); free(c);

    printf("  OK\n");
}

void test_hash(void) {
    printf("Testing hash table...\n");
    Hash h;
    h_init(&h, 7);
    assert(h.size == 0);

    h_put(&h, "no_wifi",     1);
    h_put(&h, "blue_screen", 2);
    h_put(&h, "no_wifi",     3);

    assert(h.size == 2);
    assert( h_contains(&h, "no_wifi",     1));
    assert( h_contains(&h, "no_wifi",     3));
    assert( h_contains(&h, "blue_screen", 2));
    assert(!h_contains(&h, "blue_screen", 1));
    assert(!h_contains(&h, "missing",     1));

    int cnt;
    int *ids = h_get_ids(&h, "no_wifi", &cnt);
    assert(cnt == 2);
    assert((ids[0]==1 && ids[1]==3) || (ids[0]==3 && ids[1]==1));

    ids = h_get_ids(&h, "missing", &cnt);
    assert(ids == NULL && cnt == 0);

    for (int i = 0; i < 50; i++) {
        char key[32]; sprintf(key, "k%d", i);
        h_put(&h, key, i);
    }
    assert(h.size > 2);
    h_free(&h);
    printf("  OK\n");
}

void test_persistence(void) {
    printf("Testing persistence...\n");
    Node *root    = create_question_node("Test question?");
    root->yes     = create_solution_node("Solution A");
    root->no      = create_question_node("Second question?");
    root->no->yes = create_solution_node("Solution B");
    root->no->no  = create_solution_node("Solution C");

    Node *saved = g_root; g_root = root;

    assert(save_tree("test_ts.dat"));
    free_tree(g_root); g_root = NULL;

    assert(load_tree("test_ts.dat"));
    assert(g_root && g_root->isQuestion);
    assert(strcmp(g_root->text,      "Test question?") == 0);
    assert(strcmp(g_root->yes->text, "Solution A")     == 0);

    assert(save_tree("test_ts2.dat"));
    FILE *f1 = fopen("test_ts.dat",  "rb");
    FILE *f2 = fopen("test_ts2.dat", "rb");
    fseek(f1,0,SEEK_END); fseek(f2,0,SEEK_END);
    assert(ftell(f1) == ftell(f2));
    fclose(f1); fclose(f2);

    free_tree(g_root); g_root = saved;
    remove("test_ts.dat"); remove("test_ts2.dat");
    printf("  OK\n");
}

void test_integrity(void) {
    printf("Testing check_integrity...\n");
    Node *root = create_question_node("Q1");
    root->yes  = create_solution_node("S1");
    root->no   = create_solution_node("S2");

    Node *saved = g_root; g_root = root;
    assert(check_integrity());

    free_tree(root->no);    //original test.c file didn't have this line, but it should to avoid memory leak
    root->no = NULL;
    assert(!check_integrity());

    root->no = create_solution_node("S2");
    root->yes->yes = create_solution_node("ghost");
    assert(!check_integrity());
    free_tree(root->yes->yes); root->yes->yes = NULL;
    assert(check_integrity());

    free_tree(g_root); g_root = saved;
    printf("  OK\n");
}

int main(void) {
    printf("\n=== ECE 312 Lab 4 Unit Tests ===\n\n");
    test_nodes();
    test_stack();
    test_edit_stack();
    test_queue();
    test_canonicalize();
    test_hash();
    test_persistence();
    test_integrity();
    printf("\n=== All tests passed ===\n\n");
    printf("Next: make run  |  implement run_diagnosis (TODO 31)  "
           "|  implement find_shortest_path (TODO 30)\n\n");
    return 0;
}
