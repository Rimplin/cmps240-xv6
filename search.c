#include "types.h"
#include "user.h"

// Count substring occurrences (overlapping allowed)
int count_occ(char *line, char *word)
{
    int count = 0;
    int n = strlen(line);
    int m = strlen(word);

    if(m == 0) return 0;

    for(int i = 0; i <= n - m; i++){
        int j;
        for(j = 0; j < m; j++){
            if(line[i + j] != word[j])
                break;
        }
        if(j == m){
            count++;
            // If you want overlapping matches, keep i same
            // If you want non-overlapping, use: i += m - 1;
        }
    }

    return count;
}

void search_file(char *filename, char *word)
{
    int fd = open(filename, 0);
    if(fd < 0){
        printf(1, "search: cannot open %s\n", filename);
        return;
    }

    char line[512];
    int len = 0;
    int line_no = 1;
    char c;
    int n;

    int total_occ = 0;     // total occurrences in file
    int matched_lines = 0; // how many lines matched

    while((n = read(fd, &c, 1)) == 1){
        if(c == '\n' || len >= sizeof(line)-1){
            line[len] = '\0';

            int occ = count_occ(line, word);
            if(occ > 0){
                printf(1, "%s:%d: %s\n", filename, line_no, line);
                total_occ += occ;
                matched_lines++;
            }

            len = 0;
            line_no++;
        } else {
            line[len++] = c;
        }
    }

    // Last line if no \n
    if(len > 0){
        line[len] = '\0';
        int occ = count_occ(line, word);
        if(occ > 0){
            printf(1, "%s:%d: %s\n", filename, line_no, line);
            total_occ += occ;
            matched_lines++;
        }
    }

    if(matched_lines == 0){
        printf(1, "search: no occurrences found in %s\n", filename);
    } else {
        printf(1, "Total occurrences of \"%s\": %d\n", word, total_occ);
        printf(1, "Total matching lines: %d\n", matched_lines);
    }

    close(fd);
}

int main(int argc, char *argv[])
{
    if(argc != 3){
        printf(1, "usage: search word file\n");
        exit();
    }

    search_file(argv[2], argv[1]);

    exit();
}
