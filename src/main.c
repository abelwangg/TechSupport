/*
 * main.c -- Tech Support Diagnosis Tool
 * ECE 312 Lab 4
 *
 * This file is PROVIDED in full.  Do NOT modify it except for the
 * ONE place marked below: uncomment the body of initialize_tree()
 * after you have implemented TODOs 1 and 2 in ds.c.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <ncurses.h>
#include "lab4.h"

/* ------ globals ------------------------------------------------------------------------------------------------------------------------------------------------------ */
Node      *g_root  = NULL;
EditStack  g_undo  = {NULL, 0, 0};
EditStack  g_redo  = {NULL, 0, 0};
Hash       g_index = {NULL, 0, 0};

/* ------ color pairs ------------------------------------------------------------------------------------------------------------------------------------------ */
#define COLOR_HEADER   1
#define COLOR_QUESTION 2
#define COLOR_SUCCESS  3
#define COLOR_ERROR    4
#define COLOR_INFO     5

/* ------ UI helpers --------------------------------------------------------------------------------------------------------------------------------------------- */
void init_gui(void) {
    initscr(); start_color(); cbreak(); noecho();
    keypad(stdscr, TRUE); curs_set(1);
    init_pair(COLOR_HEADER,   COLOR_CYAN,   COLOR_BLACK);
    init_pair(COLOR_QUESTION, COLOR_YELLOW, COLOR_BLACK);
    init_pair(COLOR_SUCCESS,  COLOR_GREEN,  COLOR_BLACK);
    init_pair(COLOR_ERROR,    COLOR_RED,    COLOR_BLACK);
    init_pair(COLOR_INFO,     COLOR_WHITE,  COLOR_BLUE);
}

void draw_box(int y, int x, int h, int w, const char *title) {
    attron(COLOR_PAIR(COLOR_HEADER) | A_BOLD);
    mvhline(y,         x,         ACS_HLINE, w);
    mvhline(y + h - 1, x,         ACS_HLINE, w);
    mvvline(y,         x,         ACS_VLINE, h);
    mvvline(y,         x + w - 1, ACS_VLINE, h);
    mvaddch(y,         x,             ACS_ULCORNER);
    mvaddch(y,         x + w - 1,     ACS_URCORNER);
    mvaddch(y + h - 1, x,             ACS_LLCORNER);
    mvaddch(y + h - 1, x + w - 1,     ACS_LRCORNER);
    if (title) mvprintw(y, x + 2, "[ %s ]", title);
    attroff(COLOR_PAIR(COLOR_HEADER) | A_BOLD);
}

void display_header(void) {
    attron(COLOR_PAIR(COLOR_INFO) | A_BOLD);
    mvprintw(0, 0, "%-80s", " ECE 312 Lab 4: Tech Support Diagnosis Tool");
    attroff(COLOR_PAIR(COLOR_INFO) | A_BOLD);
}

void display_menu(void) {
    attron(COLOR_PAIR(COLOR_HEADER));
    mvprintw(LINES - 3, 2,
        "[D]iagnose | [V]iew Tree | [U]ndo | [R]edo | "
        "[S]ave | [L]oad | [I]ntegrity | [F]ind Path | [Q]uit");
    attroff(COLOR_PAIR(COLOR_HEADER));
}

char *get_input(int y, int x, const char *prompt) {
    static char buf[512];
    memset(buf, 0, sizeof(buf));
    attron(COLOR_PAIR(COLOR_QUESTION));
    mvprintw(y, x, "%s", prompt);
    attroff(COLOR_PAIR(COLOR_QUESTION));
    echo(); curs_set(1);
    mvgetnstr(y, x + (int)strlen(prompt), buf, 510);
    noecho();
    return buf;
}

int get_yes_no(int y, int x, const char *prompt) {
    char *in;
    while (1) {
        in = get_input(y, x, prompt);
        if (tolower((unsigned char)in[0]) == 'y') return 1;
        if (tolower((unsigned char)in[0]) == 'n') return 0;
        attron(COLOR_PAIR(COLOR_ERROR));
        mvprintw(y + 1, x, "Please enter y or n.          ");
        attroff(COLOR_PAIR(COLOR_ERROR));
        refresh();
    }
}

void show_message(const char *msg, int is_error) {
    int color = is_error ? COLOR_ERROR : COLOR_SUCCESS;
    attron(COLOR_PAIR(color) | A_BOLD);
    mvprintw(LINES - 5, 2, "%-76s", msg);
    attroff(COLOR_PAIR(color) | A_BOLD);
    refresh(); napms(1500);
    mvprintw(LINES - 5, 2, "%-76s", "");
}

/* ------ initial knowledge base ---------------------------------------------------------------------------------------------------------------
 *
 * The tree starts with a single question and two solution leaves.
 * The program learns everything else through the diagnosis loop.
 *
 * UNCOMMENT the body of initialize_tree() once you have implemented
 * create_question_node and create_solution_node (TODOs 1-2).
 * --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
static void initialize_tree(void) {

    /* UNCOMMENT AFTER IMPLEMENTING TODOs 1-2:

    h_init(&g_index, 31);

    Node *root  = create_question_node(
                      "Is the problem with a network device?");
    root->yes   = create_solution_node(
                      "Reboot your router and try again.");
    root->no    = create_solution_node(
                      "Restart your computer and try again.");
    g_root = root;

    */
}

/* ------ find path helper --------------------------------------------------------------------------------------------------------------------------- */
static void run_find_path(void) {
    clear();
    attron(COLOR_PAIR(5) | A_BOLD);
    mvprintw(0, 0, "%-80s", " Find Distinguishing Path");
    attroff(COLOR_PAIR(5) | A_BOLD);

    mvprintw(2, 2, "Enter the exact text of two solution nodes.");

    char sol1[512], sol2[512], *tmp;

    tmp = get_input(4, 2, "Solution A: ");
    strncpy(sol1, tmp, sizeof(sol1) - 1); sol1[511] = '\0';

    tmp = get_input(6, 2, "Solution B: ");
    strncpy(sol2, tmp, sizeof(sol2) - 1); sol2[511] = '\0';

    find_shortest_path(sol1, sol2);

    mvprintw(LINES - 4, 2, "Press any key to continue...");
    refresh(); getch();
}

/* ------ main --------------------------------------------------------------------------------------------------------------------------------------------------------------- */
int main(void) {
    init_gui();
    es_init(&g_undo);
    es_init(&g_redo);
    initialize_tree();

    int running = 1;
    while (running) {
        clear();
        display_header();
        draw_box(2, 1, LINES - 6, COLS - 2, "Diagnosis Tool");
        display_menu();

        mvprintw(4, 3, "Knowledge base: %d nodes",
                 g_root ? count_nodes(g_root) : 0);
        mvprintw(5, 3, "Undo stack: %d  |  Redo stack: %d",
                 g_undo.size, g_redo.size);

        if (g_root == NULL) {
            attron(COLOR_PAIR(COLOR_ERROR));
            mvprintw(7, 3,
                "Tree not initialized -- implement TODOs 1-2 "
                "and uncomment initialize_tree() in main.c");
            attroff(COLOR_PAIR(COLOR_ERROR));
        } else {
            mvprintw(7, 3, "Choose an option:");
        }
        refresh();

        switch (tolower((unsigned char)getch())) {
            case 'd':
                if (!g_root)
                    show_message("Initialize the tree first.", 1);
                else
                    run_diagnosis();
                break;
            case 'v': draw_tree(); break;
            case 'u':
                { int r = undo_last_edit();
                  show_message(r ? "Undo successful." : "Nothing to undo.", !r); }
                break;
            case 'r':
                { int r = redo_last_edit();
                  show_message(r ? "Redo successful." : "Nothing to redo.", !r); }
                break;
            case 's':
                if (!g_root) show_message("Nothing to save.", 1);
                else { int r = save_tree("techsupport.dat");
                       show_message(r ? "Saved." : "Save failed.", !r); }
                break;
            case 'l':
                { int r = load_tree("techsupport.dat");
                  show_message(r ? "Loaded." : "Load failed.", !r); }
                break;
            case 'i':
                if (!g_root) show_message("Nothing to check.", 1);
                else { int r = check_integrity();
                       show_message(r ? "Integrity check passed."
                                      : "Integrity check FAILED.", !r); }
                break;
            case 'f':
                if (!g_root) show_message("Nothing to search.", 1);
                else run_find_path();
                break;
            case 'q': running = 0; break;
        }
    }

    endwin();
    free_tree(g_root);
    free_edit_stack(&g_undo);
    free_edit_stack(&g_redo);
    h_free(&g_index);
    return 0;
}
