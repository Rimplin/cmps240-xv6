#include "types.h"
#include "stat.h"
#include "user.h"

#define BUF_SIZE 512

int
strstr_simple(char *haystack, char *needle) {
  int hlen = strlen(haystack);
  int nlen = strlen(needle);

  for (int i = 0; i <= hlen - nlen; i++) {
    int match = 1;
    for (int j = 0; j < nlen; j++) {
      if (haystack[i+j] != needle[j]) {
        match = 0;
        break;
      }
    }
    if (match) return 1;
  }
  return 0;
}

int
main(int argc, char *argv[]) {
  if (argc < 3) {
    printf(1, "Usage: search <keyword> <filename>\n");
    exit();
  }

  char *keyword = argv[1];
  char *filename = argv[2];

  int fd = open(filename, 0);
  if (fd < 0) {
    printf(1, "search: cannot open %s\n", filename);
    exit();
  }

  char buf[BUF_SIZE];
  int n;
  char line[1024];
  int idx = 0;

  while ((n = read(fd, buf, sizeof(buf))) > 0) {
    for (int i = 0; i < n; i++) {
      if (buf[i] == '\n' || idx >= sizeof(line)-1) {
        line[idx] = '\0';

        if (strstr_simple(line, keyword)) {
          printf(1, "%s\n", line);
        }
        idx = 0;
      } else {
        line[idx++] = buf[i];
      }
    }
  }

  close(fd);
  exit();
}

