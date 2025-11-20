#include "types.h"
#include "stat.h"
#include "fs.h"
#include "user.h"

#define MAX_PATH 128


char* strcat(char *dst, const char *src) {
  char *p = dst;
  while (*p) p++;     // move to end of dst
  while ((*p++ = *src++) != '\0')
    ;
  return dst;
}

int strncmp(const char *s1, const char *s2, int n) {
    for (int i = 0; i < n; i++) {
        if (s1[i] != s2[i] || s1[i] == '\0' || s2[i] == '\0')
            return (unsigned char)s1[i] - (unsigned char)s2[i];
    }
    return 0;
}


// simple '*' wildcard: *, prefix*, *suffix, *middle*
int match(char *name, char *pattern) {
  char *star = strchr(pattern, '*');
  int nlen = strlen(name), plen = strlen(pattern);

  if (!star)  // no wildcard → exact match
    return strcmp(name, pattern) == 0;

  if (plen == 1 && pattern[0] == '*')  // "*" matches all
    return 1;

  int prefix_len = star - pattern;
  int suffix_len = plen - prefix_len - 1;

  // prefix check
  if (prefix_len && strncmp(name, pattern, prefix_len) != 0)
    return 0;

  // suffix check
  if (suffix_len &&
      strncmp(name + nlen - suffix_len, star + 1, suffix_len) != 0)
    return 0;

  return 1;
}

void find(char *dir, char *pattern) {
  char path[MAX_PATH];
  int fd;
  struct dirent de;
  struct stat st;

  if ((fd = open(dir, 0)) < 0) {
    printf(2, "find: cannot open %s\n", dir);
    return;
  }

  if (fstat(fd, &st) < 0) {
    printf(2, "find: cannot stat %s\n", dir);
    close(fd);
    return;
  }

  if (st.type != T_DIR) {   // allow starting at a file
    if (match(dir, pattern))
      printf(1, "%s\n", dir);
    close(fd);
    return;
  }

  while (read(fd, &de, sizeof(de)) == sizeof(de)) {
    if (de.inum == 0)
      continue;
    if (!strcmp(de.name, ".") || !strcmp(de.name, ".."))
      continue;

    // build path
    if (strlen(dir) + 1 + strlen(de.name) + 1 >= MAX_PATH)
      continue;

    strcpy(path, dir);
    if (path[strlen(path)-1] != '/')
      strcat(path, "/");
    strcat(path, de.name);

    if (stat(path, &st) < 0)
      continue;

    if (st.type == T_FILE && match(de.name, pattern))
      printf(1, "%s\n", path);

    if (st.type == T_DIR)
      find(path, pattern);
  }

  close(fd);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf(2, "Usage: find <directory> <pattern>\n");
    exit();
  }
  find(argv[1], argv[2]);
  exit();
}

