#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "lab4.h"

extern Node      *g_root;
extern EditStack  g_undo;
extern EditStack  g_redo;
extern Hash       g_index;

/* ------ color pairs ------------------------------------------------------------------------------------------------------------------------------------------ */
#define COLOR_HEADER   1
#define COLOR_QUESTION 2
#define COLOR_SUCCESS  3
#define COLOR_ERROR    4
#define COLOR_INFO     5

/* ----------------------------------------------------------------
 * TODO 31  run_diagnosis
 *
 * Walk the decision tree iteratively (no recursion) using a
 * FrameStack.  At each question node ask the user yes/no and push
 * the appropriate child.  At each solution leaf display the fix and
 * ask whether it solved the problem.
 *
 * If the fix did not help, enter the learning phase:
 *   - Ask the user what would actually fix the problem.
 *   - Ask for a yes/no question that distinguishes their problem
 *     from the solution just shown.
 *   - Ask which answer applies to their problem.
 *   - Create a new question node and a new solution node, wire them
 *     correctly, graft them into the tree, record an Edit for
 *     undo/redo, and index the new question with canonicalize/h_put.
 *
 * Edge case: if parent is NULL the root itself must be replaced.
 * ---------------------------------------------------------------- */
void run_diagnosis(void) {
    clear();
    attron(COLOR_PAIR(5) | A_BOLD);
    mvprintw(0, 0, "%-80s", " Tech Support Diagnosis");
    attroff(COLOR_PAIR(5) | A_BOLD);

    mvprintw(2, 2, "I'll help diagnose your tech problem.");
    mvprintw(3, 2, "Answer each question with y or n.");
    mvprintw(4, 2, "Press any key to start...");
    refresh();
    getch();

    FrameStack stack;
    fs_init(&stack);

    /* TODO: implement */
    if (g_root != NULL) {
        fs_push(&stack, g_root, -1);
    }

    //crucial for learning: must remember how we got to the current node
    Node *parent = NULL;
    int parentAnswer = -1;

    int row = 6;    //will start printing the dialogue at row 6

    while (!fs_empty(&stack)) {
        Frame f = fs_pop(&stack);
        Node *curr = f.node;

        if (curr->isQuestion) {
            //ask question and get 1 (yes) or 0 (no)
            int ans = get_yes_no(row, 2, curr->text);
            row += 2; //move down two rows for next print

            //update breadcrumbs BEFORE move to next node
            parent = curr;
            parentAnswer = ans;

            //push chosen child onto stack so we process it next loop
            if (ans == 1) {
                fs_push(&stack, curr->yes, ans);
            } else {
                fs_push(&stack, curr->no, ans);
            }

        } else {    //reached a solution leaf
            mvprintw(row, 2, "Suggested fix: %s", curr->text);
            int worked = get_yes_no(row + 1, 2, "Did this solve your problem? (y/n): ");

            if (worked) {
                attron(COLOR_PAIR(COLOR_SUCCESS));
                mvprintw(row + 3, 2, "Great! Glad I could help.");
                attroff(COLOR_PAIR(COLOR_SUCCESS));
                refresh();
                getch();
            } else {
                //LEARNNNNNNNN
                row += 3;

                //hint31: must immediately copy string buffer from get_input before call get_input again, or it will overwrite itself
                char *rawFix = get_input(row, 2, "What would actually fix this problem? ");
                char newFix[512];
                strncpy(newFix, rawFix, sizeof(newFix) - 1);
                newFix[511] = '\0'; //ensure null termination
                row += 2;

                mvprintw(row, 2, "Give me a yes/no question that distinguishes your problem");
                char prompt[600];
                snprintf(prompt, sizeof(prompt), "from \"%s\": ", curr->text);
                char *rawQ = get_input(row + 1, 4, prompt);
                char newQ[512];
                strncpy(newQ, rawQ, sizeof(newQ) - 1);
                newQ[511] = '\0'; //ensure null termination
                row += 3;

                int yesForNew = get_yes_no(row, 2, "For your problem, is the answer yes or no? (y/n): ");

                //1. create the new node
                Node *qNode = create_question_node(newQ);
                Node *sNew = create_solution_node(newFix);
                ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                //2. wire new question to both the new fix and the old fix
                if (yesForNew) {
                    qNode->yes = sNew;
                    qNode->no = curr;   //old solution becomes the "no" branch
                } else {
                    qNode->yes = curr;  //old solution becomes the "yes" branch
                    qNode->no = sNew;
                }
                ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                //3. graft the new question sub-tree into the main tree
                if (parent == NULL) {
                    g_root = qNode;     //edge case: root itself was the solution leaf

                } else {
                    if (parentAnswer == 1) {
                        parent->yes = qNode;
                    } else {
                        parent->no = qNode;
                    }
                }
                ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                //4. record the edit so we can undo it later
                Edit e;
                e.type = EDIT_INSERT_SPLIT;
                e.parent = parent;
                e.wasYesChild = (parent == NULL) ? -1 : parentAnswer;
                e.oldLeaf = curr;
                e.newQuestion = qNode;
                e.newLeaf = sNew;

                es_push(&g_undo, e);
                es_clear(&g_redo);  //making new edit permanently deltes the redo timeline

                attron(COLOR_PAIR(COLOR_SUCCESS));
                mvprintw(row + 2, 2, "Thanks! I'll remember that.");
                attroff(COLOR_PAIR(COLOR_SUCCESS));
                refresh();
                getch();
            }
        }
    
    }
    fs_free(&stack);
}

/* ----------------------------------------------------------------
 * TODO 32  undo_last_edit
 * Return 1 on success, 0 if the undo stack is empty.
 * ---------------------------------------------------------------- */
int undo_last_edit(void) {
    if (es_empty(&g_undo)) return 0;

    Edit e = es_pop(&g_undo);

    //swap old leaf back into place
    if (e.parent == NULL) { //edge case: if no parent, put oldLeaf back at root
        g_root = e.oldLeaf;
    } else {
        if (e.wasYesChild == 1) {   //if edit was on yes branch, put oldLeaf back on yes branch. 
            e.parent->yes = e.oldLeaf;
        } else {    //Otherwise, put it back on no branch
            e.parent->no = e.oldLeaf;
        }
    }

    //push to redo stack, success
    es_push(&g_redo, e);
    return 1;
}

/* ----------------------------------------------------------------
 * TODO 33  redo_last_edit
 * Return 1 on success, 0 if the redo stack is empty.
 * ---------------------------------------------------------------- */
int redo_last_edit(void) {
    if (es_empty(&g_redo)) return 0;

    Edit e = es_pop(&g_redo);

    //swap new question back into place
    if (e.parent == NULL) {
        g_root = e.newQuestion;
    } else {
        if (e.wasYesChild == 1) {
            e.parent->yes = e.newQuestion;
        } else {
            e.parent->no = e.newQuestion;
        }
    }

    //push to undo stack, success
    es_push(&g_undo, e);
    return 1;
}
