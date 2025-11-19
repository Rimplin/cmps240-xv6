#include "types.h"
#include "stat.h"
#include "user.h"

#define BUF_SIZE 512   // buffer for reading file

void itos(int n, char *buf) { // itos => integer to string
    int i = 0;
    if (n == 0) { buf[0] = '0'; buf[1] = '\0'; return; }

    char temp[10];
    while (n > 0) {
        temp[i++] = (n % 10) + '0';
        n /= 10;
    }

    int j = 0;
    while (i > 0) {
        buf[j++] = temp[--i];
    }
    buf[j] = '\0';
}

int main(int argc, char *argv[]) {
    if(argc != 4){
        printf(2, "Usage: split <source_file> <chunk_size> <prefix>\n");
        exit();
    }

    char *src_file = argv[1];
    int chunk_size = atoi(argv[2]);
    char *prefix = argv[3];

    if(chunk_size <= 0){
        printf(2, "chunk_size must be positive\n");
        exit();
    }

    int src_fd = open(src_file, 0);
    if(src_fd < 0){
        printf(2, "split: cannot open %s\n", src_file);
        exit();
    }

    char buf[BUF_SIZE];
    int bytes_read;
    int part_num = 1;
    int bytes_written_in_chunk = 0;
    int dest_fd;
    char dest_name[64];
    char numbuf[16];

    // open first chunk file
    itos(part_num, numbuf);
    strcpy(dest_name, prefix);
    strcpy(dest_name + strlen(prefix), numbuf);

    dest_fd = open(dest_name, 1 | 0x200);  // 1 = write, 0x200 = O_CREATE
    if(dest_fd < 0){
        printf(2, "split: cannot create %s\n", dest_name);
        close(src_fd);
        exit();
    }

    while((bytes_read = read(src_fd, buf, BUF_SIZE)) > 0){
        int i = 0;
        while(i < bytes_read){
            int remaining_in_chunk = chunk_size - bytes_written_in_chunk;
            int to_write = bytes_read - i;
            if(to_write > remaining_in_chunk)
                to_write = remaining_in_chunk;

            write(dest_fd, buf + i, to_write);
            bytes_written_in_chunk += to_write;
            i += to_write;

            if(bytes_written_in_chunk >= chunk_size){
                close(dest_fd);
                part_num++;

                itos(part_num, numbuf);
                strcpy(dest_name, prefix);
                strcpy(dest_name + strlen(prefix), numbuf);

                dest_fd = open(dest_name, 1| 0x200);
                if(dest_fd < 0){
                    printf(2, "split: cannot create %s\n", dest_name);
                    close(src_fd);
                    exit();
                }
                bytes_written_in_chunk = 0;
            }
        }
    }

    close(src_fd);
    close(dest_fd);
    exit();
}

