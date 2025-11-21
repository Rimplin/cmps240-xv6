// schedtest.c - Display MLFQ scheduler statistics
// Uses getpinfo() system call to show process priorities and CPU usage

#include "types.h"
#include "stat.h"
#include "user.h"
#include "pstat.h"

void
print_header(void)
{
  printf(1, "\n%-6s %-8s %-10s %-12s %-10s %-10s\n", 
         "PID", "STATE", "PRIORITY", "CPU TICKS", "SCHEDULED", "STATE");
  printf(1, "--------------------------------------------------------------\n");
}

char*
state_string(int state)
{
  switch(state) {
    case 0: return "UNUSED";
    case 1: return "EMBRYO";
    case 2: return "SLEEPING";
    case 3: return "RUNNABLE";
    case 4: return "RUNNING";
    case 5: return "ZOMBIE";
    default: return "UNKNOWN";
  }
}

void
print_pinfo(struct pstat *ps)
{
  int i;
  
  print_header();
  
  for(i = 0; i < NPROC; i++) {
    if(ps->inuse[i]) {
      printf(1, "%-6d %-8s %-10d %-12d %-10d %-10s\n",
             ps->pid[i],
             "",
             ps->priority[i],
             ps->ticks[i],
             ps->times_scheduled[i],
             state_string(ps->state[i]));
    }
  }
  printf(1, "\n");
}

void
analyze_priority_distribution(struct pstat *ps)
{
  int priority_count[4] = {0};  // Assuming NPRIO = 4
  int total_active = 0;
  int i;
  
  for(i = 0; i < NPROC; i++) {
    if(ps->inuse[i] && ps->state[i] != 5) {  // Not ZOMBIE
      if(ps->priority[i] >= 0 && ps->priority[i] < 4)
        priority_count[ps->priority[i]]++;
      total_active++;
    }
  }
  
  printf(1, "=== Priority Distribution ===\n");
  for(i = 0; i < 4; i++) {
    printf(1, "Priority %d: %d processes", i, priority_count[i]);
    if(total_active > 0) {
      printf(1, " (%.1d%%)\n", (priority_count[i] * 100) / total_active);
    } else {
      printf(1, "\n");
    }
  }
  printf(1, "Total active: %d\n\n", total_active);
}

int
main(int argc, char *argv[])
{
  struct pstat ps;
  int continuous = 0;
  int interval = 10;  // Default interval in ticks
  
  if(argc > 1 && strcmp(argv[1], "-c") == 0) {
    continuous = 1;
    if(argc > 2)
      interval = atoi(argv[2]);
  }
  
  printf(1, "\n=== MLFQ Scheduler Statistics ===\n");
  
  if(continuous) {
    printf(1, "Continuous mode (Ctrl-C to stop), interval=%d ticks\n", interval);
    
    while(1) {
      if(getpinfo(&ps) < 0) {
        printf(1, "getpinfo failed\n");
        exit();
      }
      
      printf(1, "\n--- Update at tick %d ---\n", uptime());
      print_pinfo(&ps);
      analyze_priority_distribution(&ps);
      
      sleep(interval);
    }
  } else {
    // Single snapshot
    if(getpinfo(&ps) < 0) {
      printf(1, "getpinfo failed\n");
      exit();
    }
    
    print_pinfo(&ps);
    analyze_priority_distribution(&ps);
  }
  
  exit();
}
