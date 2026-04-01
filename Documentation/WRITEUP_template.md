# ECE 312 Lab 4 Write-Up: Tech Support Diagnosis Tool

**Name:** [Your Name]
**EID:** [Your EID]
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

### 1.A — [Component]

*What I chose:* [1–2 sentences]

*What I considered instead:* [1 sentence]

*Why:* [2–4 sentences — be specific]

---

### 1.B — [Component]

*What I chose:* [1–2 sentences]

*What I considered instead:* [1 sentence]

*Why:* [2–4 sentences]

---

## Section 2 — Complexity Analysis (all four required)

Show the reasoning, not just the answer.

### 2.1 — Amortized cost of a single FrameStack push

### 2.2 — Hash table average-case lookup

### 2.3 — Diagnosis traversal (best, worst, average)

### 2.4 — `find_shortest_path` time and space

---

## Section 3 — Bugs (two required)

### 3.A — [Short name]

*Symptom:*

*Cause:* [quote the wrong line or describe the wrong logic]

*Fix:*

*Rule that would have prevented it:*

---

### 3.B — [Short name]

*Symptom:*

*Cause:*

*Fix:*

*Rule that would have prevented it:*

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
| | | |

**Total hours:** ___
