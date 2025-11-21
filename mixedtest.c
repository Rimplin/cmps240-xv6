// mixedtest.c - Mixed workload test for MLFQ scheduler
// Combines CPU-bound and I/O-bound processes

#include "types.h"
#include "stat.h"
#include "user.h"

// CPU-intensive work
void
cpu_work(int iterations)
{
  int i, j;
  volatile int x = 0;
  
  for(i = 0; i < iterations; i++) {
    for(j = 0; j < 1000; j++) {
      x = x + i * j;
    }
  }
}

// I/O work simulation
void
io_work(int operations)
{
  int i;
  for(i = 0; i < operations; i++) {
    sleep(1);
    
    // Small computation between I/O
    volatile int x = 0;
    int j;
    for(j = 0; j < 100; j++) {
      x = x + j;
    }
  }
}

int
main(int argc, char *argv[])
{
  int i;
  int pid;
  int num_cpu = 2;   // Number of CPU-bound processes
  int num_io = 2;    // Number of I/O-bound processes
  
  if(argc > 1)
    num_cpu = atoi(argv[1]);
  if(argc > 2)
    num_io = atoi(argv[2]);
  
  printf(1, "\n=== MLFQ Mixed Workload Test ===\n");
  printf(1, "Starting %d CPU-bound and %d I/O-bound processes\n\n", 
         num_cpu, num_io);
  
  // Create CPU-bound processes
  for(i = 0; i < num_cpu; i++) {
    pid = fork();
    
    if(pid == 0) {
      printf(1, "[CPU-%d] pid=%d starting (CPU-intensive)\n", i, getpid());
      int start = uptime();
      
      cpu_work(800);
      
      int elapsed = uptime() - start;
      printf(1, "[CPU-%d] pid=%d finished in %d ticks\n", i, getpid(), elapsed);
      exit();
    }
  }
  
  // Create I/O-bound processes
  for(i = 0; i < num_io; i++) {
    pid = fork();
    
    if(pid == 0) {
      printf(1, "[I/O-%d] pid=%d starting (I/O-intensive)\n", i, getpid());
      int start = uptime();
      
      io_work(15);
      
      int elapsed = uptime() - start;
      printf(1, "[I/O-%d] pid=%d finished in %d ticks\n", i, getpid(), elapsed);
      exit();
    }
  }
  
  // Wait for all children
  for(i = 0; i < num_cpu + num_io; i++) {
    wait();
  }
  
  printf(1, "\n=== All processes completed ===\n");
  exit();
}
