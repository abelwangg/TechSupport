# Lab 4 — Hints

Read each hint only after you have attempted the TODO on your own.

---

## TODOs 1–2: Node creation

What does `strdup` do that a plain pointer assignment does not?

What is the difference between `malloc(sizeof(Node*))` and `malloc(sizeof(Node))`?

What should `yes` and `no` be when you first create a node?

---

## TODO 3: `free_tree`

If you free the parent node before recursing into its children, what happens to the child pointers? Think about the order carefully before writing a single line.

---

## TODO 6: `fs_push` — resize

When exactly is the stack full? Off-by-one here writes one element past the end of the array — valgrind will catch it but a crash may not appear immediately.

After calling `realloc`, always use the return value. `realloc` may give you a different address than the one you passed in.

---

## TODO 17: `q_dequeue` — the most common bug in this lab

Draw the queue on paper with exactly one element. `front` and `rear` both point to the same node. After you remove it, what must be true of both pointers?

If you only update one of them, the next `q_enqueue` will write into freed memory. This bug often does not crash immediately, which makes it hard to find without valgrind.

---

## TODO 20: `canonicalize`

The output string is never longer than the input — characters are only dropped or replaced, never expanded. How much memory do you need to allocate?

Who owns the returned string?

---

## TODO 23: `h_put` — two cases

There are exactly two situations: the key already exists in the chain, and it does not. Handle them separately. In the "key exists" case there is still a sub-case: the specific `solutionId` may or may not already be in the list.

When you create a new entry, you must store a copy of the key string, not a pointer to the caller's buffer. The caller may free or reuse that buffer at any time.

---

## TODO 27: `save_tree`

BFS visits parents before children. The root gets id 0, its children get ids 1 and 2, their children get ids 3 4 5 6, and so on. This ordering is what makes `load_tree` reconstructable in two passes — when processing node i, all referenced child ids are guaranteed to already exist in the nodes array.

To find the id of a given child pointer, search the NodeMapping array for the entry whose `.node` field matches.

---

## TODO 28: `load_tree`

Why not link children during the read phase?

In BFS order, a child's id is always greater than its parent's id. But the child node struct may not have been `malloc`'d yet when you are reading the parent. Reading all nodes into a flat array first, then linking in a second pass, sidesteps the ordering problem entirely.

The file stores text without a null terminator. You must add one after `fread`.

On any read error, validate failure, or allocation failure, free everything you have allocated so far before returning 0. A `goto` label is clean for this.

---

## TODO 30: `find_shortest_path`

This requires the LCA (Lowest Common Ancestor) of two leaves. The approach:

1. BFS the tree. For each node, record which node was its parent and which branch (yes or no) was taken to reach it. Store this in a `PathNode` array on the heap — use the same BFS id as the array index. Stop BFS early once both target leaves are found.

2. Walk from each found leaf back to the root via `->parent` pointers, building two arrays of ancestors. Index 0 is the leaf; the last index is the root.

3. Walk both ancestor arrays inward from the root end until the two paths diverge. The node where they diverge is just below the LCA; the node before that is the LCA.

4. Print the shared path (root down to LCA) and the diverging branches.

The ancestor arrays are pointers into `pathNodes`, which must stay allocated until printing is complete. Free `pathNodes` at the very end, not before.

---

## TODO 31: `run_diagnosis`

The most important thing to get right before writing the loop is parent tracking. You need to know, when you reach a solution leaf, which question node was its parent and which branch (yes or no) was taken to reach it.

Initialize two variables *outside* the loop. Update them *every time* you visit a question node — not just the first time.

The edge case: if the very first node in the tree is a solution leaf (no questions at all), `parent` is still NULL when the learning phase runs. The root itself must be replaced. Your Edit record needs a way to signal this case so undo can reverse it correctly.

`get_input` returns a pointer to a static buffer. The next call to `get_input` overwrites that same buffer. Copy the result to a local array before calling `get_input` again.

---

## TODOs 32–33: Undo and redo

An Edit record stores enough information to reverse or reapply exactly one tree modification: which parent had a child replaced, which branch it was, what the old child was, and what the new subtree is.

Do not free any nodes during undo or redo. The nodes still exist — they are just not currently in the tree. A future redo may need them.

---

## Growing a good knowledge base

A good yes/no question splits the remaining candidates roughly in half. A question that is "yes" for every problem in a subtree except one is nearly useless.

Before teaching the program a new problem, think: what is the most meaningful yes/no question that separates this problem from the solution it incorrectly matched? That question becomes a useful internal node in the tree.

A good actionable solution tells the user exactly what to do without requiring them to search the internet. "Restart the print spooler" is not actionable. "Open services.msc, right-click Print Spooler, and select Restart" is.
