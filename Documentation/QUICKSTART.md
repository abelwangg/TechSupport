# Quick Start Guide - Lab 4: Tech Diagnosis 

Get up and running in 5 minutes!

## Step 1: Install Dependencies

*If not using the ECE Linux lab machines, you must install the ncurses library to compile and run the GUI.*
### Ubuntu/Debian:
```bash
sudo apt-get update
sudo apt-get install build-essential libncurses-dev
```

### macOS:
```bash
brew install ncurses
```

### Verify Installation:
```bash
gcc --version
pkg-config --libs ncurses
```

## Step 2: Build

```bash
cd src 
make
```

You should see:
```
gcc -Wall -Wextra -g -std=c99 -c main.c -o main.o
gcc -Wall -Wextra -g -std=c99 -c ds.c -o ds.o
...
gcc main.o ds.o game.o persist.o utils.o -o techsupport -lncurses
```

## Step 3: Run Tests

```bash
make test
./run_tests
```

Expected output:
```
=== Running Unit Tests ===

Testing Frame Stack...
  --- Stack tests passed
Testing Queue...
  --- Queue tests passed
Testing Hash Table...
  --- Hash table tests passed
...
=== All Tests Passed! ===
```

## Step 4: Play!

```bash
./techsupport
```

### First Run:
1. Press **P** to play
2. Think of a problem (e.g., "my WiFi is down")
3. Answer the questions
4. Teach it new problems and solutions!

### Try These Features:
- Press **S** to save your tree
- Press **U** to undo the last thing you taught
- Press **R** to redo
- Press **I** to check tree integrity
- Press **Q** to quit

## Common Issues

### "ncurses.h not found"
**Fix:** Install ncurses development library
```bash
sudo apt-get install libncurses-dev
```

### Weird characters in terminal
**Fix:** Your terminal might not support ncurses properly. Try:
```bash
export TERM=xterm-256color
./techsupport
```

### Terminal doesn't reset after crash
**Fix:** Type `reset` to restore terminal
```bash
reset
```

## Next Steps

1. **Read the full README.md** for detailed documentation
2. **Review WRITEUP.md** for design decisions and complexity analysis
3. **Modify and experiment:**
   - Change the initial tree in `main.c`
   - Add more colors to the GUI
   - Implement question ordering based on information gain
   - Add tree visualization

## Quick Reference

### Keyboard Shortcuts:
| Key | Action |
|-----|--------|
| P | Play a game |
| U | Undo last learn |
| R | Redo last undo |
| S | Save tree |
| L | Load tree |
| I | Integrity check |
| Q | Quit |

### File Outputs:
- `techsupport.dat` - Saved tree in binary format
- `test.dat`, `test2.dat` - Test files (auto-cleaned)

### Debugging:
```bash
# Check for memory leaks
make valgrind

# Run with GDB
gdb ./techsupport
(gdb) run
```

## Learning Path for Students

### Week 1: Understand the Basics
- Study the Node structure and tree representation
- Trace through a simple gameplay session on paper
- Understand how the Frame stack replaces recursion

### Week 2: Implement Data Structures
- Implement stack operations (push/pop)
- Implement queue operations (enqueue/dequeue)
- Implement hash table with separate chaining
- Write unit tests for each

### Week 3: Build Core Features
- Implement iterative gameplay loop
- Add learning functionality
- Implement undo/redo stacks
- Test thoroughly

### Week 4: Add Persistence and Polish
- Implement BFS save/load
- Add integrity checker
- Build the GUI
- Write complexity analysis

## Tips for Success

1. **Test Early, Test Often:** Run unit tests after each feature
2. **Use Valgrind:** Check for memory leaks frequently
3. **Draw Pictures:** Visualize the tree on paper
4. **Print Debugging:** Use printf to trace execution
5. **Read the Code:** All functions are well-commented

## Getting Help

1. Review the write-up for design rationale
2. Check test cases for usage examples
3. Use GDB to step through code
4. Draw the tree state on paper
5. Ask your TA during office hours!

## Congratulations!

You've built a sophisticated learning system that demonstrates:
- --- Dynamic memory management
- --- Custom data structures
- --- Iterative algorithms
- --- Binary file I/O
- --- GUI programming
- --- Software engineering best practices

Great work! ----
