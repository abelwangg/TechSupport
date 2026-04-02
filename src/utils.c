#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "lab4.h"

extern Node *g_root;

/* ----------------------------------------------------------------
 * TODO 29  check_integrity
 *
 * Use BFS to verify:
 *   - Every question node has both yes and no children (non-NULL).
 *   - Every solution node has both children NULL.
 * Return 1 if valid, 0 if any violation is found.
 * ---------------------------------------------------------------- */
int check_integrity(void) {
    if (g_root == NULL) return 1;   //empty tree unbroken i guess?

    Queue q;
    q_init(&q);
    q_enqueue(&q, g_root, 0);

    Node *n;
    int id;

    while (q_dequeue(&q, &n, &id)) {
        //if question, must have both yes and no branch
        if (n->isQuestion) {
            //must have both children. if either is missing, it's broken
            if (n->yes == NULL || n->no == NULL) {
                q_free(&q);
                return 0;
            }
            //run children in the queue so we can check them on future loops
            q_enqueue(&q, n->yes, 0);
            q_enqueue(&q, n->no, 0);
        } else {
            //isSolution. cannot have children
            if (n->yes != NULL || n->no != NULL) {
                q_free(&q);
                return 0;
            }
        }
    }

    //good integrity
    q_free(&q);
    return 1;
}

/*
abel wang
thinking process for find_shortest_path
trying to find LOWEST COMMON ANCESTOR of two leaves in a binary tree

step 1: explore and leave bread crumbs
    Use Queue to explore the tree top-down, lvl-by-lvl. As we visit every single question and solution, we assign it a unique ID.
    Every time move parent -> child, drop "breadcrumb" in array called pathNodes.
    breadcrumb says: "I am node ID 5. I got here from parent ID 2, and I took the YES branch."
    Stop exploring as soon as we find both of the solution strings user asked for
step 2:
    IDs of two target solutions acquired.
    walk backwards.
    Save backwards journey into two separate lists.
step 3:
    Lists will be built backwards leaf -> root
    End: root.
    Start at end of list and walk "forward"
    Walk until lists differ -> we found LCA. This is the divergence question.
step 4: print
    mvprintw to print all shared nodes we walked past
    Then print divergence question
    Finally look at breadcrumbs to see which branch led to Solution A and Solution B.
*/


/* ----------------------------------------------------------------
 * TODO 30  find_shortest_path
 *
 * Given the exact text of two solution leaves, display the
 * questions that distinguish them.  Use BFS with a parent-tracking
 * PathNode array to find both leaves, build ancestor arrays for
 * each, find the Lowest Common Ancestor (LCA), then print:
 *   - The shared path of questions both solutions pass through.
 *   - The divergence question (LCA) and which branch leads where.
 *
 * Display results with mvprintw.  Print an error if either
 * solution is not found.  Free all allocations before returning.
 * ---------------------------------------------------------------- */

//need to remember how we reached each node during BFS
//helper struct?
typedef struct {
    Node *node;     //actual tree node
    int parent_id;  //BFS ID of node that led us here
    int branch_from_parent;     //1 if YES, 0 if NO
} PathNode;

void find_shortest_path(const char *sol1, const char *sol2) {
    if (g_root == NULL) {
        mvprintw(10, 2, "Error: knowledge base is empty.");
        refresh();
        return;
    }
    
    int total_nodes = count_nodes(g_root);

    //allocate breadcrump trail array on heap. use BFS ID as index for instant loopups
    PathNode *pathNodes = calloc(total_nodes, sizeof(PathNode));

    Queue q;
    q_init(&q);

    //root setup
    q_enqueue(&q, g_root, 0);
    pathNodes[0].parent_id = -1;

    //tracker for if we found the targets
    int t1_id = -1;
    int t2_id = -1;

    int next_id = 1;
    Node *n;
    int id;

    //BFS TO FIND SOLUTIONS AND DROP BREADCRUMBS
        while (q_dequeue(&q, &n, &id)) {
            pathNodes[id].node = n;

            if (!n->isQuestion) {
                //check if solution matches either of user's inputs
                if (strcmp(n->text, sol1) == 0) t1_id = id;
                if (strcmp(n->text, sol2) == 0) t2_id = id;

                //stop searching early if we found both
                if (t1_id != -1 && t2_id != -1) break;

            } else {
                //drop breadcrumbs for the children before queueing them
                if (n->yes) {
                    pathNodes[next_id].parent_id = id;
                    pathNodes[next_id].branch_from_parent = 1;
                    q_enqueue(&q, n->yes, next_id++);
                }
                if (n->no) {
                    pathNodes[next_id].parent_id = id;
                    pathNodes[next_id].branch_from_parent = 0;
                    q_enqueue(&q, n->no, next_id++);
                }
            }
        }
        q_free(&q);

        //handle solutions not found
        if (t1_id == -1 || t2_id == -1) {
            mvprintw(10, 2, "Error: One of both solutions not found in the tree.");
            refresh();
            free(pathNodes);
            return;
        }

    //WALK BACKWARD FROM LEAVES TO ROOT
        //arrays will store IDs of the path from leaf -> root
        int *p1 = malloc(total_nodes * sizeof(int));
        int *p2 = malloc(total_nodes * sizeof(int));
        int len1 = 0, len2 = 0;

        int curr = t1_id;
        while (curr != -1) {
            p1[len1++] = curr;
            curr = pathNodes[curr].parent_id;
        }

        curr = t2_id;
        while (curr != -1) {
            p2[len2++] = curr;
            curr = pathNodes[curr].parent_id;
        }

    //WALK INWARD FROM ROOT TO FIND LCA (lowest common ancestor i alw forget this)
        //p1 and p2 are leaf -> root. End of the arrays are root. Walk backward down the arrasy simultaneously
        int idx1 = len1 - 1;
        int idx2 = len2 - 1;
        int lca_id = -1;

        int print_row = 10;
        mvprintw(print_row++, 2, "Shared Path:");

        //while both paths share exact same node ID, we are still on the shared trunk
        while (idx1 >= 0 && idx2 >= 0 && p1[idx1] == p2[idx2]) {
            lca_id = p1[idx1];
            Node *lca_node = pathNodes[lca_id].node;

            //only print if they are about to take the exact same branch to the next node
            if (idx1 > 0 && idx2 > 0 && p1[idx1 - 1] == p2[idx2 - 1]) {
                int next_node_id = p1[idx1 - 1];
                char *branch = pathNodes[next_node_id].branch_from_parent ? "Yes" : "No";
                mvprintw(print_row++, 4, "[%s] -> %s", branch, lca_node->text);
            }
            idx1--;
            idx2--;
        }


    //PRINT THE DIVERGENCE
    //loop broke -> paths split. lca_id holds the junction node
    mvprintw(print_row++, 2, "Divergence Question: %s", pathNodes[lca_id].node->text);

    //looki at next step each path took to see how they split
    int branch1 = pathNodes[p1[idx1]].branch_from_parent;
    mvprintw(print_row++, 4, "If %s  -> leads to: %s", branch1? "YES" : "NO", sol1);

    int branch2 = pathNodes[p2[idx2]].branch_from_parent;
    mvprintw(print_row++, 4, "If %s  -> leads to: %s", branch2? "YES" : "NO", sol2);


    //CLEANUP
    free(p1);
    free(p2);
    free(pathNodes);
}
