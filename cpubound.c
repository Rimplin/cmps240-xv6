// cpubound.c - CPU-intensive test program for MLFQ scheduler
// This program continuously performs calculations without yielding

#include "types.h"
#include "stat.h"
#include "user.h"

// Perform CPU-intensive work
void
compute(int iterations)
{
  int i, j;
  volatile int x = 0;  // volatile to prevent optimization
  
  for(i = 0; i < iterations; i++) {
    for(j = 0; j < 1000; j++) {
      x = x + i * j;
      x = x - i / (j + 1);
    }
  }
}

int
main(int argc, char *argv[])
{
  int i;
  int pid;
  int num_processes = 3;  // Default number of CPU-bound processes
  int iterations = 1000;   // Number of iterations
  
  if(argc > 1)
    num_processes = atoi(argv[1]);
  if(argc > 2)
    iterations = atoi(argv[2]);
  
  printf(1, "Starting %d CPU-bound processes with %d iterations each\n", 
         num_processes, iterations);
  
  for(i = 0; i < num_processes; i++) {
    pid = fork();
    
    if(pid < 0) {
      printf(1, "Fork failed\n");
      exit();
    }
    
    if(pid == 0) {
      // Child process
      printf(1, "CPU-bound process %d (pid=%d) starting\n", i, getpid());
      
      int start_time = uptime();
      compute(iterations);
      int end_time = uptime();
      
      printf(1, "CPU-bound process %d (pid=%d) finished in %d ticks\n", 
             i, getpid(), end_time - start_time);
      exit();
    }
  }
  
  // Parent waits for all children
  for(i = 0; i < num_processes; i++) {
    wait();
  }
  
  printf(1, "All CPU-bound processes completed\n");
  exit();
}
