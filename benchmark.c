// benchmark.c - Comprehensive MLFQ scheduler benchmark
// Measures turnaround time, response time, and fairness

#include "types.h"
#include "stat.h"
#include "user.h"
#include "pstat.h"

#define NUM_TRIALS 3

// Performance metrics
struct metrics {
  int turnaround_time;  // Total time from creation to completion
  int response_time;    // Time from creation to first run
  int cpu_time;         // Total CPU time used
  int wait_time;        // Time spent waiting
};

// CPU-intensive work
void
cpu_intensive(int iterations)
{
  int i, j;
  volatile int x = 0;
  
  for(i = 0; i < iterations; i++) {
    for(j = 0; j < 500; j++) {
      x = x + i * j;
      x = x / (j + 1);
    }
  }
}

// I/O-intensive work
void
io_intensive(int operations)
{
  int i, j;
  
  for(i = 0; i < operations; i++) {
    // Small computation
    volatile int x = 0;
    for(j = 0; j < 50; j++) {
      x = x + j;
    }
    
    // Simulate I/O
    sleep(1);
  }
}

// Interactive work (short bursts)
void
interactive(int bursts)
{
  int i, j;
  
  for(i = 0; i < bursts; i++) {
    // Very short computation
    volatile int x = 0;
    for(j = 0; j < 20; j++) {
      x = x + j;
    }
    
    // Sleep to simulate user think time
    sleep(2);
  }
}

void
run_cpu_benchmark(void)
{
  int i, pid;
  int start_time, end_time;
  int num_processes = 4;
  
  printf(1, "\n=== CPU-Bound Benchmark ===\n");
  printf(1, "Running %d CPU-intensive processes...\n", num_processes);
  
  start_time = uptime();
  
  for(i = 0; i < num_processes; i++) {
    pid = fork();
    if(pid == 0) {
      int my_start = uptime();
      cpu_intensive(500);
      int my_end = uptime();
      printf(1, "  CPU Process %d: turnaround=%d ticks\n", 
             getpid(), my_end - my_start);
      exit();
    }
  }
  
  // Wait for all
  for(i = 0; i < num_processes; i++) {
    wait();
  }
  
  end_time = uptime();
  printf(1, "Total time: %d ticks\n", end_time - start_time);
  printf(1, "Average per process: %d ticks\n", 
         (end_time - start_time) / num_processes);
}

void
run_io_benchmark(void)
{
  int i, pid;
  int start_time, end_time;
  int num_processes = 4;
  
  printf(1, "\n=== I/O-Bound Benchmark ===\n");
  printf(1, "Running %d I/O-intensive processes...\n", num_processes);
  
  start_time = uptime();
  
  for(i = 0; i < num_processes; i++) {
    pid = fork();
    if(pid == 0) {
      int my_start = uptime();
      io_intensive(10);
      int my_end = uptime();
      printf(1, "  I/O Process %d: turnaround=%d ticks\n", 
             getpid(), my_end - my_start);
      exit();
    }
  }
  
  // Wait for all
  for(i = 0; i < num_processes; i++) {
    wait();
  }
  
  end_time = uptime();
  printf(1, "Total time: %d ticks\n", end_time - start_time);
  printf(1, "Average per process: %d ticks\n", 
         (end_time - start_time) / num_processes);
}

void
run_mixed_benchmark(void)
{
  int i, pid;
  int start_time, end_time;
  int num_cpu = 2;
  int num_io = 2;
  int num_interactive = 2;
  
  printf(1, "\n=== Mixed Workload Benchmark ===\n");
  printf(1, "Running %d CPU + %d I/O + %d interactive processes...\n", 
         num_cpu, num_io, num_interactive);
  
  start_time = uptime();
  
  // CPU processes
  for(i = 0; i < num_cpu; i++) {
    pid = fork();
    if(pid == 0) {
      int my_start = uptime();
      cpu_intensive(400);
      int my_end = uptime();
      printf(1, "  [CPU] Process %d: turnaround=%d ticks\n", 
             getpid(), my_end - my_start);
      exit();
    }
  }
  
  // I/O processes
  for(i = 0; i < num_io; i++) {
    pid = fork();
    if(pid == 0) {
      int my_start = uptime();
      io_intensive(8);
      int my_end = uptime();
      printf(1, "  [I/O] Process %d: turnaround=%d ticks\n", 
             getpid(), my_end - my_start);
      exit();
    }
  }
  
  // Interactive processes
  for(i = 0; i < num_interactive; i++) {
    pid = fork();
    if(pid == 0) {
      int my_start = uptime();
      interactive(5);
      int my_end = uptime();
      printf(1, "  [INT] Process %d: turnaround=%d ticks\n", 
             getpid(), my_end - my_start);
      exit();
    }
  }
  
  // Wait for all
  for(i = 0; i < num_cpu + num_io + num_interactive; i++) {
    wait();
  }
  
  end_time = uptime();
  printf(1, "Total time: %d ticks\n", end_time - start_time);
}

void
run_fairness_test(void)
{
  int i, pid;
  int num_processes = 5;
  struct pstat ps;
  
  printf(1, "\n=== Fairness Test ===\n");
  printf(1, "Running %d identical CPU processes...\n", num_processes);
  
  for(i = 0; i < num_processes; i++) {
    pid = fork();
    if(pid == 0) {
      cpu_intensive(500);
      exit();
    }
  }
  
  // Wait for all
  for(i = 0; i < num_processes; i++) {
    wait();
  }
  
  // Get final statistics
  if(getpinfo(&ps) == 0) {
    printf(1, "\nCPU time distribution:\n");
    int total_ticks = 0;
    for(i = 0; i < NPROC; i++) {
      if(ps.inuse[i] && ps.ticks[i] > 0) {
        printf(1, "  PID %d: %d ticks\n", ps.pid[i], ps.ticks[i]);
        total_ticks += ps.ticks[i];
      }
    }
    printf(1, "Total CPU ticks: %d\n", total_ticks);
  }
}

void
print_summary_stats(void)
{
  struct pstat ps;
  int i;
  int priority_count[4] = {0};
  int total_processes = 0;
  int total_ticks = 0;
  
  if(getpinfo(&ps) < 0) {
    printf(1, "Failed to get process info\n");
    return;
  }
  
  printf(1, "\n=== Scheduler Summary Statistics ===\n");
  
  for(i = 0; i < NPROC; i++) {
    if(ps.inuse[i]) {
      total_processes++;
      total_ticks += ps.ticks[i];
      if(ps.priority[i] >= 0 && ps.priority[i] < 4) {
        priority_count[ps.priority[i]]++;
      }
    }
  }
  
  printf(1, "Total processes: %d\n", total_processes);
  printf(1, "Total CPU ticks: %d\n", total_ticks);
  printf(1, "\nPriority distribution:\n");
  for(i = 0; i < 4; i++) {
    printf(1, "  Priority %d: %d processes\n", i, priority_count[i]);
  }
}

int
main(int argc, char *argv[])
{
  int run_all = 1;
  
  printf(1, "\n");
  printf(1, "========================================\n");
  printf(1, "  MLFQ Scheduler Benchmark Suite\n");
  printf(1, "========================================\n");
  
  if(argc > 1) {
    if(strcmp(argv[1], "cpu") == 0) {
      run_cpu_benchmark();
      run_all = 0;
    } else if(strcmp(argv[1], "io") == 0) {
      run_io_benchmark();
      run_all = 0;
    } else if(strcmp(argv[1], "mixed") == 0) {
      run_mixed_benchmark();
      run_all = 0;
    } else if(strcmp(argv[1], "fairness") == 0) {
      run_fairness_test();
      run_all = 0;
    }
  }
  
  if(run_all) {
    run_cpu_benchmark();
    sleep(10);
    
    run_io_benchmark();
    sleep(10);
    
    run_mixed_benchmark();
    sleep(10);
    
    run_fairness_test();
  }
  
  print_summary_stats();
  
  printf(1, "\n========================================\n");
  printf(1, "  Benchmark Complete\n");
  printf(1, "========================================\n\n");
  
  exit();
}
