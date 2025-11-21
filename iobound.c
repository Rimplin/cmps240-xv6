// iobound.c - I/O-intensive test program for MLFQ scheduler
// This program frequently yields CPU to simulate I/O operations

#include "types.h"
#include "stat.h"
#include "user.h"

// Simulate I/O by sleeping
void
simulate_io(int duration)
{
  sleep(duration);
}

// Perform small amount of computation between I/O
void
small_compute(void)
{
  volatile int x = 0;
  int i;
  
  for(i = 0; i < 100; i++) {
    x = x + i;
  }
}

int
main(int argc, char *argv[])
{
  int i, j;
  int pid;
  int num_processes = 3;   // Default number of I/O-bound processes
  int io_operations = 20;  // Number of I/O operations per process
  int sleep_duration = 1;  // Sleep duration (ticks)
  
  if(argc > 1)
    num_processes = atoi(argv[1]);
  if(argc > 2)
    io_operations = atoi(argv[2]);
  
  printf(1, "Starting %d I/O-bound processes with %d I/O operations each\n", 
         num_processes, io_operations);
  
  for(i = 0; i < num_processes; i++) {
    pid = fork();
    
    if(pid < 0) {
      printf(1, "Fork failed\n");
      exit();
    }
    
    if(pid == 0) {
      // Child process
      printf(1, "I/O-bound process %d (pid=%d) starting\n", i, getpid());
      
      int start_time = uptime();
      
      for(j = 0; j < io_operations; j++) {
        // Small computation
        small_compute();
        
        // Simulate I/O operation
        simulate_io(sleep_duration);
        
        // More small computation
        small_compute();
      }
      
      int end_time = uptime();
      
      printf(1, "I/O-bound process %d (pid=%d) finished in %d ticks\n", 
             i, getpid(), end_time - start_time);
      exit();
    }
  }
  
  // Parent waits for all children
  for(i = 0; i < num_processes; i++) {
    wait();
  }
  
  printf(1, "All I/O-bound processes completed\n");
  exit();
}
