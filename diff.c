#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#define MAX_LINE 512
#define MAX_LINES 1000

char lines1[MAX_LINES][MAX_LINE];
char lines2[MAX_LINES][MAX_LINE];
int line_count1 = 0;
int line_count2 = 0;

// Read a line from file descriptor
int
readline(int fd, char *line, int max)
{
  int i = 0;
  char c;
  
  while (i < max - 1) {
    int n = read(fd, &c, 1);
    if (n <= 0)
      break;
    
    line[i++] = c;
    if (c == '\n')
      break;
  }
  
  line[i] = 0;
  return i;
}

// Read all lines from a file
int
readlines(char *filename, char lines[][MAX_LINE], int max_lines)
{
  int fd = open(filename, O_RDONLY);
  if (fd < 0) {
    printf(1, "diff: cannot open %s\n", filename);
    return -1;
  }
  
  int count = 0;
  while (count < max_lines) {
    int len = readline(fd, lines[count], MAX_LINE);
    if (len == 0)
      break;
    count++;
  }
  
  close(fd);
  return count;
}

// Compare two strings
int
strcmp_custom(char *s1, char *s2)
{
  while (*s1 && *s2 && *s1 == *s2) {
    s1++;
    s2++;
  }
  return *s1 - *s2;
}

// Simple diff algorithm - line by line comparison
void
simple_diff(void)
{
  int i = 0, j = 0;
  int found_diff = 0;
  
  while (i < line_count1 || j < line_count2) {
    if (i >= line_count1) {
      // File 1 ended, file 2 has more lines
      printf(1, "%da%d\n", i, j + 1);
      printf(1, "> %s", lines2[j]);
      j++;
      found_diff = 1;
    }
    else if (j >= line_count2) {
      // File 2 ended, file 1 has more lines
      printf(1, "%dd%d\n", i + 1, j);
      printf(1, "< %s", lines1[i]);
      i++;
      found_diff = 1;
    }
    else if (strcmp_custom(lines1[i], lines2[j]) == 0) {
      // Lines are the same
      i++;
      j++;
    }
    else {
      // Lines differ - try to find if line was changed, added, or deleted
      // Simple strategy: look ahead one line in each file
      int look_ahead = 0;
      
      // Check if line1[i] matches line2[j+1] (insertion in file2)
      if (j + 1 < line_count2 && strcmp_custom(lines1[i], lines2[j + 1]) == 0) {
        printf(1, "%da%d\n", i, j + 1);
        printf(1, "> %s", lines2[j]);
        j++;
        look_ahead = 1;
      }
      // Check if line1[i+1] matches line2[j] (deletion in file1)
      else if (i + 1 < line_count1 && strcmp_custom(lines1[i + 1], lines2[j]) == 0) {
        printf(1, "%dd%d\n", i + 1, j);
        printf(1, "< %s", lines1[i]);
        i++;
        look_ahead = 1;
      }
      // Otherwise it's a change
      else {
        printf(1, "%dc%d\n", i + 1, j + 1);
        printf(1, "< %s", lines1[i]);
        printf(1, "---\n");
        printf(1, "> %s", lines2[j]);
        i++;
        j++;
      }
      
      found_diff = 1;
    }
  }
  
  if (!found_diff) {
    // Files are identical - no output (standard diff behavior)
  }
}

int
main(int argc, char *argv[])
{
  if (argc != 3) {
    printf(1, "Usage: diff file1 file2\n");
    exit();
  }
  
  // Read both files
  line_count1 = readlines(argv[1], lines1, MAX_LINES);
  if (line_count1 < 0)
    exit();
  
  line_count2 = readlines(argv[2], lines2, MAX_LINES);
  if (line_count2 < 0)
    exit();
  
  // Perform diff
  simple_diff();
  
  exit();
}
