#include "types.h" 
#include "stat.h" 
#include "user.h" 

int main(void) { 
  int num_write = 16; // 16: SYS_WRITE 
  int num_read = 5; // 5: SYS_READ 
  
  int count_write0 = getsyscount(num_write); 
  int count_read0 = getsyscount(num_read); 
  
  write(1, "Hello from count.c!\n", 20); // write count will increase by 1 
  
  char buf[1];
  const char *q = "From 1 to 5, how much are you enjoying CMPS 240 (5 = most)?\n";
  write(1, q, strlen(q));               // write count will again increase by 1 
  read(0, buf, 1); // read count will increase by 1 
  
  
  int count_write1 = getsyscount(num_write); 
  int count_read1 = getsyscount(num_read); 
  
  printf(1, "write: %d -> %d\n", count_write0, count_write1); 
  printf(1, "read : %d -> %d\n", count_read0, count_read1); 
  
  exit(); 
}
