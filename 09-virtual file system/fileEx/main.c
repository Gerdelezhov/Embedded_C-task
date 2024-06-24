#include <dirent.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_FILES 100

struct FileInfo {
    char *name;
    int is_directory;
};

// Custom comparator for sorting FileInfo
int compareFileInfo(const void *a, const void *b) {
    struct FileInfo *file1 = (struct FileInfo *)a;
    struct FileInfo *file2 = (struct FileInfo *)b;

    // If both are directories or both are files, sort alphabetically
    if (file1->is_directory == file2->is_directory) {
        return strcmp(file1->name, file2->name);
    }

    // Otherwise, directories come before files
    return file2->is_directory - file1->is_directory;
}

void scanDirectory(const char *path, struct FileInfo *files, int *n_files) {
    struct dirent *d;
    DIR *dir = opendir(path);

    if (dir == NULL) {
        printw("Failed to open the directory.");
        refresh();
        getch();
        endwin();
        exit(1);
    }

    *n_files = 0;

    // Add parent directory entry
    files[*n_files].name = strdup("..");
    files[*n_files].is_directory = 1;
    (*n_files)++;

    while ((d = readdir(dir)) != NULL && *n_files < MAX_FILES) {
        if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0) {
            continue;  // Skip current directory entry, parent handled
                       // separately
        }
        files[*n_files].name = strdup(d->d_name);
        files[*n_files].is_directory = (d->d_type == DT_DIR);
        (*n_files)++;
    }

    closedir(dir);

    // Sort the files after scanning
    qsort(files, *n_files, sizeof(struct FileInfo), compareFileInfo);
}

void initUI() {
    initscr();
    keypad(stdscr, TRUE);
    noecho();
    cbreak();
    curs_set(0);

    start_color();
    init_pair(1, COLOR_CYAN, COLOR_BLACK);  // Cyan text on black background
}

void drawUI() {
    int height, width;
    getmaxyx(stdscr, height, width);

    bkgd(COLOR_PAIR(1));
    attron(COLOR_PAIR(1));
    box(stdscr, 0, 0);
    int middle_x = width / 2;
    for (int y = 1; y < height - 1; y++) {
        mvaddch(y, middle_x, ACS_VLINE);
    }
    attroff(COLOR_PAIR(1));
}

void drawFileList(struct FileInfo *files, int n_files, int highlight, int top,
                  int col_start, int current_list, int list_id) {
    int y_offset = 1;        // Offset for the starting y-coordinate of the list
    int height = LINES - 2;  // Height of the drawable area

    for (int i = 0; i < height && i + top < n_files; i++) {
        if ((i + top == highlight) && (current_list == list_id)) {
            attron(A_REVERSE);
        }
        if (files[i + top].is_directory) {
            mvprintw(i + y_offset, col_start + 1, "/%s",
                     files[i + top].name);  // 1 right and 1 down
        } else {
            mvprintw(i + y_offset, col_start + 1, "%s",
                     files[i + top].name);  // 1 right and 1 down
        }
        if ((i + top == highlight) && (current_list == list_id)) {
            attroff(A_REVERSE);
        }
    }
}

void freeFileList(struct FileInfo *files, int n_files) {
    for (int i = 0; i < n_files; i++) {
        free(files[i].name);
    }
}

int main() {
    struct FileInfo files[MAX_FILES];
    int n_files = 0;
    scanDirectory(".", files, &n_files);

    int highlight1 = 0;
    int highlight2 = 0;
    int top1 = 0;
    int top2 = 0;
    int current_list = 1;
    int choice;

    initUI();

    while (1) {
        clear();
        drawUI();

        drawFileList(files, n_files, highlight1, top1, 0, current_list, 1);
        drawFileList(files, n_files, highlight2, top2, COLS / 2, current_list,
                     2);

        choice = getch();

        switch (choice) {
            case KEY_UP:
                if (current_list == 1) {
                    if (highlight1 > 0) {
                        highlight1--;
                        if (highlight1 < top1) {
                            top1--;
                        }
                    }
                } else {
                    if (highlight2 > 0) {
                        highlight2--;
                        if (highlight2 < top2) {
                            top2--;
                        }
                    }
                }
                break;
            case KEY_DOWN:
                if (current_list == 1) {
                    if (highlight1 < n_files - 1) {
                        highlight1++;
                        if (highlight1 >= top1 + LINES - 2) {
                            top1++;
                        }
                    }
                } else {
                    if (highlight2 < n_files - 1) {
                        highlight2++;
                        if (highlight2 >= top2 + LINES - 2) {
                            top2++;
                        }
                    }
                }
                break;
            case 9:  // TAB key
                current_list = (current_list == 1) ? 2 : 1;
                break;
            case 10:  // ENTER key
                if ((current_list == 1 && files[highlight1].is_directory) ||
                    (current_list == 2 && files[highlight2].is_directory)) {
                    char path[256];
                    snprintf(path, sizeof(path), "./%s",
                             current_list == 1 ? files[highlight1].name
                                               : files[highlight2].name);
                    chdir(path);  // Change directory to the selected path
                    scanDirectory(".", files,
                                  &n_files);  // Rescan the current directory
                    highlight1 =
                        0;  // Reset highlight and top after changing directory
                    highlight2 = 0;
                    top1 = 0;
                    top2 = 0;
                }
                break;
            case 27:  // ESC key
                freeFileList(files, n_files);
                endwin();
                return 0;
            default:
                break;
        }
    }

    freeFileList(files, n_files);
    endwin();
    return 0;
}
