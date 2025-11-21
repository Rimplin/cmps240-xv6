// pstat.h - Process statistics structure for MLFQ scheduler
// Used with getpinfo() system call


#ifndef _PSTAT_H_
#define _PSTAT_H_

#include "param.h"

struct pstat {
  int inuse[NPROC];            // whether this slot of the process table is in use (1 or 0)
  int pid[NPROC];              // PID of each process
  int priority[NPROC];         // current priority level of each process (0 = highest)
  int state[NPROC];            // current state (UNUSED, EMBRYO, SLEEPING, RUNNABLE, RUNNING, ZOMBIE)
  uint ticks[NPROC];           // total number of ticks each process has accumulated at each priority
  int times_scheduled[NPROC];  // number of times each process has been scheduled
};

#endif // _PSTAT_H_
