# ECE 312 Lab 4 Write-Up: Tech Support Diagnosis Tool

**Name:** Jingyuan Wang
**EID:** jw62882
**Date:** [Submission Date]

---

## What This Document Is

1–2 pages of honest reflection on the decisions you made and the problems you hit. Not a summary of the lab spec.

Full credit requires:
- Two specific design choices with a stated reason and a named alternative
- Four Big-O analyses with reasoning shown
- Two concrete bugs (symptom → cause → fix → rule)
- Reflection on the knowledge base you grew through sessions
- A note on `find_shortest_path`

Vague entries ("I had a leak and fixed it") earn no credit.

---

## Section 1 — Design Choices (two required, ~100 words each)

For each: what did you choose, what was the alternative, and why?

Candidate topics:
- Array-backed stack vs. linked-list stack
- Two-pass design in `load_tree` — why not link during the read phase?
- Dynamic `PathNode` array in `find_shortest_path` vs. fixed-size stack array
- Ownership model for nodes in undo/redo — why not free on undo?
- Iterative diagnosis loop — what state did you have to track explicitly?

### 1.A — Two-pass design in `load_tree`

*What I chose:* I used a two-pass approach where I first read all nodes from the file into a flat array, and then looped through a second time to link the yes and no pointers.

*What I considered instead:* Pretty obvious, the I considered trying to link the parent and child nodes immediately in a single pass as I read them from the file.

*Why:* Because the tree is saved using BFS, child nodes are naturally stored later in the file than their parents. If I tried a one-pass design, a parent node might need to point to a child ID that hsn't even been read into memory yet. The two-pass method avoids this by making sure every single node actually exists before I try to connect the nodes together.

---

### 1.B — Ownership model for nodes in undo/redo

*What I chose:* I chose to keep the disconnected nodes alive in memory when the user hits undo, rather than calling `free()` on them.

*What I considered instead:* Freeing the newly learned question and solution nodes completelly during an undo to save memory space.

*Why:* If I freed the nodes, the `g_redo` stack would be left holding dangling pointers to dead memory. If the user wanted the redo, the program would crash instantly. By keeping them alive and just swapping the pointers, I can let the user travel safely back and forth in time. Honestly, the only time we permanently delete future edits is when a brand new edit creates an alternate timeline. This logic is kind of complicated, but it needs to be this way.

---

## Section 2 — Complexity Analysis (all four required)

Show the reasoning, not just the answer.

### 2.1 — Amortized cost of a single FrameStack push

Amortized O(1). When pushing to the stack, we usually just drop the item into the next open array slot, which takes constant time. If the array is completely full, we double its capacity using realloc. Doubling takes O(n) time because it copies all existing elements to a new memory block. However, since this doubling happens rarely, the heavy cost is very spread out, making the avg time per push O(1).

### 2.2 — Hash table average-case lookup

Avg case is O(1). djb2 hash function mathematically scramble the string and jump instantly to the correct bucket index. B/c we're separate chaining and our hash function spreads the strings out evenly across the table, the linked list inside any single buckiet stays very short. Finding the item takes constant time.

### 2.3 — Diagnosis traversal (best, worst, average)

h - height of tree | n - total num of nodes

Best case: O(1). Root itself is a solution leaf. Loop runs once.

Worst case: O(h). Tree is unbalanced like a straight line. We have to answer questions all the way down to the deepest leaf.

Average case: O(log n). Tree is relatively balanced. The height is logarithmic relative to the total number of nodes.

### 2.4 — `find_shortest_path` time and space

Time complexity: O(n). The BFS explores the tree to drop breadcrumps, checking every node once in the worst case to find the two target solutions. Tracing the path arrays leaves->root takes O(h) steps at most. Overall time is linear.

Space complexity: O(n). I dynamically allocated a `PathNode` array sized perfectly to `total_nodes` to store breadcrumbs. The two integer path arrays also scale with the depth of the tree, so meaning usage scales linearly with size of the knowledge base.

---

## Section 3 — Bugs (two required)

### 3.A — Missed Null Terminator

*Symptom:* `test_canonicalize` failed because returned string was failing the `strcmp` with "abc".

*Cause:* `char* result = (char*)malloc(strlen(s) + 1);`
I literally allocated space for the null terminator, but I completely forgot to actually write it into the array at the end of the for loop. It was reading garbage past the end of my characters.

*Fix:* Added `result[j] = '\0';` right before returning that string. 

*Rule that would have prevented it:* I should always explicitly cap off dynamically allocated string with a null terminator if building them char by char in a loop. I somehow very easily forget this.

---

### 3.B — Buffer Underflow Crash

*Symptom:* Game crashed with a `double free or corruption (out)` error. Core dumped. Tried to find a path that didn't exist in the tree.

*Cause:* `int next_id = -1;`
In `find_shortest_path`, I initialized the child ID tracker to -1. When recording breadcrumbs for the root's children, the code executed `pathNodes[-1].parent_id = id;`. This illegally wrote data right before the start of the heap array, so `free()` crashed later.

*Fix:* Initialized `next_id` to 1 instead, since the root was already manually given ID 0.

*Rule that would have prevented it:* Double-check array index initializations, especially, when manually sequential BFS IDs to physical array indices.

---

## Section 4 — Knowledge Base Reflection (~100 words)

1. How many nodes does your submitted `techsupport.dat` contain?

2. What categories of problems did you teach the program? Give one example question/solution pair for each category.

3. Look at the tree with `[V]`.  Are the questions you taught it good distinguishing questions — do they split the remaining candidates roughly in half?  Name one question you would improve and describe what you would replace it with.

4. Describe one `[F]ind Path` result.  What were the two solutions, what was the shared path, and did the output match your expectation?

---

## Section 5 — Reflection (3–5 sentences)

Answer at least two:

- What was the hardest part and why?
- What did the iterative diagnosis loop teach you about recursion?
- What would you do differently if starting over?
- Was there a moment something clicked? What was it?

The hardest part was definitely getting the learning phase pointer math right in `game.c`. Tracking the parent node and trying to surgically make a brand new sub-tree without breaking the main tree was super confusing at first. 

Writing the iterative diagnosis loop taught me that recursion is basically the computer managing a stack for you under the hood. Building the `FrameStack` manually showed that you can replicate any recursive function with a simple while loop and an array.

The biggest "click" moment was probably when my `find_shortest` path printed out the exact spit in the tree visually. Seeing the LCA logic actually working was awesome.

---

## Section 6 — Time Log

| Date | Hours | What you worked on |
|------|-------|--------------------|
| | | |
| | | |
| | | |
| | | |
| | | |
| | | |
| | | |
| | | |
| | | |
| 4/6 | 4 |bleh |

**Total hours:** ___
