#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#define HISTORY_FILE ".sh_history"
#define MAX_LINE 128

int
main(int argc, char *argv[])
{
  int fd;
  char buf[MAX_LINE];
  int n, i;
  int line_num = 1;
  int lines_to_show = 10; // default: show last 10 commands
  
  // Parse optional argument for number of lines
  if (argc > 1) {
    lines_to_show = atoi(argv[1]);
    if (lines_to_show <= 0)
      lines_to_show = 10;
  }
  
  // Open history file
  fd = open(HISTORY_FILE, O_RDONLY);
  if (fd < 0) {
    printf(1, "history: no command history found\n");
    exit();
  }
  
  // First pass: count total lines
  int total_lines = 0;
  while ((n = read(fd, buf, sizeof(buf))) > 0) {
    for (i = 0; i < n; i++) {
      if (buf[i] == '\n')
        total_lines++;
    }
  }
  
  close(fd);
  
  // Determine starting line number
  int start_line = total_lines - lines_to_show + 1;
  if (start_line < 1)
    start_line = 1;
  
  // Second pass: display lines
  fd = open(HISTORY_FILE, O_RDONLY);
  if (fd < 0) {
    printf(1, "history: cannot reopen history file\n");
    exit();
  }
  
  line_num = 1;
  char line_buf[MAX_LINE];
  int line_pos = 0;
  
  while ((n = read(fd, buf, sizeof(buf))) > 0) {
    for (i = 0; i < n; i++) {
      if (buf[i] == '\n') {
        line_buf[line_pos] = 0;
        if (line_num >= start_line) {
          printf(1, "%d  %s\n", line_num, line_buf);
        }
        line_num++;
        line_pos = 0;
      } else if (line_pos < MAX_LINE - 1) {
        line_buf[line_pos++] = buf[i];
      }
    }
  }
  
  close(fd);
  exit();
}
