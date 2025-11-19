#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"

void print_indent(int depth, int is_last[]) {
    for (int i = 0; i < depth; i++) {
        if (i == depth - 1)
            printf(1, "%s", is_last[i] ? "   " : "|  ");
        else
            printf(1, "%s", is_last[i] ? "   " : "|  ");
    }
}

void tree(char *path, int depth, int *is_last_parent) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, 0)) < 0) {
        printf(2, "tree: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0) {
        printf(2, "tree: cannot stat %s\n", path);
        close(fd);
        return;
    }

    // Print current path
    if (depth == 0) {
        printf(1, "[%s]\n", path);
    }

    if (st.type != T_DIR) {
        close(fd);
        return;
    }

    // Count valid entries first
    struct dirent entries[64];
    int n = 0;
    while (read(fd, &entries[n], sizeof(entries[0])) == sizeof(entries[0])) {
        if (entries[n].inum != 0 &&
            strcmp(entries[n].name, ".") != 0 &&
            strcmp(entries[n].name, "..") != 0) {
            n++;
        }
    }

    // Reset file descriptor to re-read
    close(fd);
    fd = open(path, 0);

    int count = 0;
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0)
            continue;
        if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
            continue;

        count++;
        int is_last = (count == n);

        // Prepare new path
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;

        // Get file stats
        if (stat(buf, &st) < 0) {
            printf(2, "tree: cannot stat %s\n", buf);
            continue;
        }

        // Print prefix and entry
        print_indent(depth, is_last_parent);
        printf(1, "%s%s\n", is_last ? "`- " : "|- ", de.name);

        // Recurse into subdirectory
        if (st.type == T_DIR) {
            int next_is_last[32];
            for (int i = 0; i <= depth; i++)
                next_is_last[i] = is_last_parent[i];
            next_is_last[depth] = is_last;

            tree(buf, depth + 1, next_is_last);
        }
    }

    close(fd);
}

int main(int argc, char *argv[]) {
    int is_last[32] = {0};
    if (argc < 2)
        tree(".", 0, is_last);
    else
        tree(argv[1], 0, is_last);
    exit();
}

