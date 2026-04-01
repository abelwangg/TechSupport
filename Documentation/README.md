# ECE 312 — Lab 4: Tech Support Diagnosis Tool

**Due Wednesday, April 8, 2026 @ 11:59pm**
## Overview

You will build an interactive tech support decision tree in C. The program starts with a single yes/no question and two solution leaves. Every time it suggests a fix that does not solve the user's problem, it **learns**: it asks for a distinguishing question and a new solution, then permanently grafts both into the tree.

The tree grows entirely through use. You do not hardcode questions. By the end of the lab your submitted knowledge base should reflect real diagnostic sessions — the tree you turn in is evidence of the program working correctly.

**Features:**
- Iterative yes/no diagnosis with a suggested fix at each leaf
- Learns new problems and solutions at runtime, no restart required
- Saves and loads the knowledge base to/from a binary file
- Full undo and redo of every learned edit
- Visual tree explorer
- Tree integrity checker
- Shortest distinguishing path finder between any two solutions

---

## What You Will Build

### Data structures — `ds.c` (TODOs 1–26)
- **Binary decision tree** — question nodes and solution nodes
- **FrameStack** — dynamic array stack for iterative traversal
- **EditStack** — dynamic array stack for undo/redo
- **Queue** — linked-list queue for BFS
- **Hash table** — maps canonicalized symptom keywords to solution IDs

### Logic — `game.c` (TODOs 31–33)
- **`run_diagnosis`** — iterative traversal and learning phase
- **`undo_last_edit`** / **`redo_last_edit`**

### Persistence — `persist.c` (TODOs 27–28)
- **`save_tree`** / **`load_tree`** — BFS binary serialization

### Utilities — `utils.c` (TODOs 29–30)
- **`check_integrity`** — BFS structural validation
- **`find_shortest_path`** — LCA-based path between two solutions

---

## How the Tree Grows

The program ships with this minimal tree:

```
"Is the problem with a network device?"
    YES  ->  "Reboot your router and try again."
    NO   ->  "Restart your computer and try again."
```

The first time a user says a suggested fix did not work, the learning phase fires:

```
Suggested fix: Restart your computer and try again.
Did this solve your problem? (y/n): n

What would fix this problem?
  > Update your graphics driver.

Give me a yes/no question that distinguishes your problem
from "Restart your computer and try again.":
  > Is the display flickering or showing artifacts?

For your problem, is the answer yes or no? (y/n): y

Thanks! I'll remember that.
```

The tree now looks like:

```
"Is the problem with a network device?"
    YES  ->  "Reboot your router and try again."
    NO   ->  "Is the display flickering or showing artifacts?"
                 YES  ->  "Update your graphics driver."
                 NO   ->  "Restart your computer and try again."
```

Every subsequent session starts from wherever the tree was left, provided the user saved before quitting. This is why save/load is essential: without saving, the learned knowledge is lost when the program exits.

---

## Section 1 — Implementation Plan

### Phase 1: Data structures — Week 1 (~20 hours)

**TODOs 1–4: Tree nodes** (~1–2 hours)

Implement `create_question_node`, `create_solution_node`, `free_tree`, `count_nodes` in `ds.c`. After TODOs 1–2 work, uncomment `initialize_tree()` in `main.c` and run `make run` to see the starting tree.

**TODOs 5–9: FrameStack** (~2–3 hours)

Dynamic array that doubles in capacity when full. After this, the stack unit tests should pass.

**TODOs 10–14: EditStack** (~1 hour)

Structurally identical to FrameStack but stores `Edit` structs. The pattern is the same.

**TODOs 15–19: Queue** (~2–3 hours)

Linked-list queue. Pay close attention to what must happen when the last element is dequeued — this is the most common source of crashes in this section.

**TODOs 20–26: Hash table** (~4–6 hours)

Implement in order: `canonicalize` → `h_hash` → `h_init` → `h_put` → `h_contains` → `h_get_ids` → `h_free`. After `h_free`, all unit tests should pass.

---

### Phase 2: Logic and persistence — Week 2 (~14 hours)

**TODOs 27–28: Save and load** (`persist.c`, ~4–6 hours)

BFS serialization to a binary file. See the format in `persist.c`. `load_tree` is the most complex single function in the lab — the two-pass design (read all nodes, then link) is essential.

**TODO 29: Integrity checker** (`utils.c`, ~1–2 hours)

BFS structural validation. Short function, good BFS practice.

**TODO 30: `find_shortest_path`** (`utils.c`, ~3–5 hours)

LCA algorithm using BFS with parent tracking. Given the text of two solution nodes, displays the shared diagnostic path and the divergence point. See `HINTS.md`.

**TODO 31: `run_diagnosis`** (`game.c`, ~4–6 hours) ⭐ hardest

Iterative tree traversal plus the learning phase. No recursion. Draw the tree transformation on paper before writing any code.

**TODOs 32–33: Undo and redo** (`game.c`, ~1–2 hours)

Each is about 12 lines. Pop from one stack, update one pointer, push to the other.

---

### Phase 3: Testing, sessions, write-up — Week 3 (~8 hours)

**Run diagnosis sessions** to grow your knowledge base.

Your submitted `techsupport.dat` must contain at least **25 nodes**. Run the program, play through the diagnosis loop, teach it new problems, and save. The node count is shown on the main screen. Examples of useful problems to teach it:

- Wi-Fi connected but no internet
- Printer showing offline
- No sound from speakers
- Phone won't charge
- Browser crashes on startup
- Outlook won't open
- External monitor not detected
- Keyboard keys not responding
- Fan running at full speed constantly
- Computer won't wake from sleep

You are not limited to these. Teach it anything that produces a valid yes/no question and a concrete actionable fix. The quality of your questions is assessed in the write-up, not by an automated check.

**Memory audit** (~2–3 hours)

Run `make valgrind-test` and `make valgrind`. Fix every "definitely lost" report. Common sources: forgetting `free(canon)` after `canonicalize` in the learning phase; error exit paths in `load_tree` that leak partially allocated nodes.

**Write-up** (~2–3 hours)

See `WRITEUP_template.md`.

---

## Section 2 — Suggested Timeline

```
  Week 1
  Mon       Tue       Wed       Thu       Fri       Sat
  ----------------------------------------------------------------
  TODOs     TODOs     Frame     Edit      Queue     Hash
  1-4       1-4       Stack     Stack     15-19     Table
  nodes     tests     5-9       10-14               20-26

  Week 2
  Mon       Tue       Wed       Thu       Fri       Sat
  ----------------------------------------------------------------
  Save      Load      Integrity Find      Diagnosis Undo
  TODO 27   TODO 28   TODO 29   Path 30   TODO 31   Redo 32-33

  Week 3
  Mon       Tue       Wed       Thu       Fri
  -----------------------------------------------
  Sessions  Sessions  Valgrind  Write-up  Submit
  (grow KB) (grow KB) leaks
```

**Total estimated time: 36–52 hours. Start early.**

---

## Section 3 — File Organization

### You implement:
| File | TODOs |
|---|---|
| `ds.c` | 1–26 |
| `game.c` | 31–33 |
| `persist.c` | 27–28 |
| `utils.c` | 29–30 |

### You modify (one place only):
| File | What |
|---|---|
| `main.c` | Uncomment `initialize_tree()` body after TODOs 1–2 |

### Do not modify:
`lab4.h`, `visualize.c`, `tests.c`, `Makefile`

---

## Section 4 — Common Mistakes

### Memory
| Wrong | Correct |
|---|---|
| `malloc(sizeof(Node*))` | `malloc(sizeof(Node))` |
| `free(node)` before children | free children first, then text, then node |
| `node->text = text` | `node->text = strdup(text)` |
| `malloc(textLen)` for a string | `malloc(textLen + 1)` |

### Queue
| Wrong | Correct |
|---|---|
| `if (size > capacity)` | `if (size >= capacity)` |
| Only null `front` on last dequeue | Null **both** `front` and `rear` |

### Learning phase
| Wrong | Correct |
|---|---|
| Hold `get_input` pointer across two calls | Copy to local buffer immediately |
| `entry->key = key` in `h_put` | `entry->key = strdup(key)` |
| Forget `free(canon)` after `h_put` | Free the canonicalized string |

### Persistence
| Wrong | Correct |
|---|---|
| Link children during the read pass | Two passes: read all, then link |
| `fread(text, ...)` without null-term | Set `text[textLen] = '\0'` after read |

---

## Section 5 — Build and Debug

```bash
make              # build
make test         # run unit tests
make run          # launch interactive tool
make valgrind     # check for leaks in the interactive run
make valgrind-test# check for leaks in the unit tests
make clean        # remove build artifacts
```

```bash
# Segfault
gdb ./techsupport
(gdb) run
(gdb) bt

# Leak
valgrind --leak-check=full --track-origins=yes ./techsupport
# Fix every "definitely lost" -- ignore "still reachable" from ncurses
```

---

## Section 6 — Grading

| Component | Points |
|---|---|
| All unit tests pass | 15 |
| Interactive diagnosis, learn, undo, redo | 15 |
| Save and load round-trip | 10 |
| `find_shortest_path` (TODO 30) | 10 |
| Knowledge base: submitted `techsupport.dat` has >= 25 nodes | 10 |
| Memory safety: zero "definitely lost" under valgrind | 20 |
| Write-up | 10 |
| Code quality: no warnings, readable names, comments | 10 |
| **Total** | **100** |

**Submission checklist:**
- [ ] All 34 TODOs implemented (1–33 + TODO 30)
- [ ] `make test` — all tests pass
- [ ] `make valgrind-test` — no "definitely lost"
- [ ] Diagnosis, learn, undo, redo all work interactively
- [ ] `[F]ind Path` shows correct shared path and divergence
- [ ] Save and load work across separate runs
- [ ] `techsupport.dat` submitted with >= 25 nodes
- [ ] `WRITEUP.md` included

---

## FAQ

**Q: Can I use recursion?**
Only `free_tree` and `count_nodes`. Everything else must be iterative.

**Q: What if I teach the program something wrong during a session?**
Use `[U]ndo` to revert the last learned edit, then save.

**Q: How do I know how many nodes my knowledge base has?**
The node count is shown on the main screen. It updates live. Save and recheck after loading to confirm persistence.

**Q: My tests pass but the program crashes during a diagnosis session.**
`run_diagnosis` is not unit tested — it requires interactive testing. Use `gdb` and check your parent tracking logic (the `parent` and `parentAnswer` variables).

**Q: Does the submitted `techsupport.dat` need to cover specific topics?**
No. Teach it whatever you like. The write-up asks you to evaluate the quality of the questions you taught it — that is where domain judgment is assessed.

**Q: Valgrind shows "still reachable". Do I need to fix that?**
No. That is ncurses internal memory. Fix only "definitely lost".
