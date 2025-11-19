// user/threadtest.c - Test program for kernel threads

#include "types.h"
#include "stat.h"
#include "user.h"

#define CLONE_THREAD 0x00000001
#define STACK_SIZE 4096

// Thread function 1
int
thread_func1(void *arg)
{
  int *id = (int*)arg;
  
  for(int i = 0; i < 5; i++) {
    printf(1, "Thread %d: iteration %d\n", *id, i);
    sleep(1);
  }
  
  printf(1, "Thread %d: exiting\n", *id);
  exit();
}

// Thread function 2 - prints letters
int
thread_func2(void *arg)
{
  char *msg = (char*)arg;
  
  for(int i = 0; i < 5; i++) {
    printf(1, "Thread says: %s (iteration %d)\n", msg, i);
    sleep(1);
  }
  
  printf(1, "Thread: done printing %s\n", msg);
  exit();
}

// Simple thread test
void
test_basic_threads(void)
{
  printf(1, "\n=== Basic Thread Test ===\n");
  
  char *stack = malloc(STACK_SIZE);
  if(stack == 0) {
    printf(1, "Failed to allocate stack\n");
    return;
  }
  
  int id = 1;
  int tid = clone(CLONE_THREAD, stack + STACK_SIZE);
  
  if(tid < 0) {
    printf(1, "clone failed\n");
    free(stack);
    return;
  }
  
  if(tid == 0) {
    // Child thread
    thread_func1(&id);
  } else {
    // Parent thread
    printf(1, "Parent: created thread %d\n", tid);
    
    // Do some work in parent
    for(int i = 0; i < 5; i++) {
      printf(1, "Parent: working... iteration %d\n", i);
      sleep(1);
    }
    
    // Wait for child
    int status;
    wait(&status);
    printf(1, "Parent: thread %d finished\n", tid);
    free(stack);
  }
}

// Multiple threads test
void
test_multiple_threads(void)
{
  printf(1, "\n=== Multiple Threads Test ===\n");
  
  #define NUM_THREADS 3
  int tids[NUM_THREADS];
  char *stacks[NUM_THREADS];
  
  // Create multiple threads
  for(int i = 0; i < NUM_THREADS; i++) {
    stacks[i] = malloc(STACK_SIZE);
    if(stacks[i] == 0) {
      printf(1, "Failed to allocate stack %d\n", i);
      continue;
    }
    
    tids[i] = clone(CLONE_THREAD, stacks[i] + STACK_SIZE);
    
    if(tids[i] < 0) {
      printf(1, "Failed to create thread %d\n", i);
      free(stacks[i]);
    } else if(tids[i] == 0) {
      // Child thread
      int id = i;
      thread_func1(&id);
    } else {
      printf(1, "Created thread %d with tid %d\n", i, tids[i]);
    }
  }
  
  // Wait for all threads
  for(int i = 0; i < NUM_THREADS; i++) {
    if(tids[i] > 0) {
      int status;
      wait(&status);
      free(stacks[i]);
    }
  }
  
  printf(1, "All threads completed\n");
}

// Shared memory test
int shared_counter = 0;

int
increment_counter(void *arg)
{
  int iterations = *(int*)arg;
  
  for(int i = 0; i < iterations; i++) {
    shared_counter++;
    printf(1, "Thread: counter = %d\n", shared_counter);
    sleep(1);
  }
  
  exit();
}

void
test_shared_memory(void)
{
  printf(1, "\n=== Shared Memory Test ===\n");
  printf(1, "Initial counter: %d\n", shared_counter);
  
  char *stack = malloc(STACK_SIZE);
  if(stack == 0) {
    printf(1, "Failed to allocate stack\n");
    return;
  }
  
  int iterations = 5;
  int tid = clone(CLONE_THREAD, stack + STACK_SIZE);
  
  if(tid == 0) {
    // Child thread
    increment_counter(&iterations);
  } else {
    // Parent
    printf(1, "Parent: waiting for thread to modify shared counter\n");
    
    int status;
    wait(&status);
    
    printf(1, "Final counter value: %d\n", shared_counter);
    free(stack);
  }
}

int
main(int argc, char *argv[])
{
  printf(1, "Kernel Thread Tests\n");
  printf(1, "===================\n");
  
  if(argc < 2) {
    printf(1, "Usage: threadtest [1|2|3|all]\n");
    printf(1, "  1   - Basic thread test\n");
    printf(1, "  2   - Multiple threads test\n");
    printf(1, "  3   - Shared memory test\n");
    printf(1, "  all - Run all tests\n");
    exit();
  }
  
  if(strcmp(argv[1], "1") == 0 || strcmp(argv[1], "all") == 0) {
    test_basic_threads();
  }
  
  if(strcmp(argv[1], "2") == 0 || strcmp(argv[1], "all") == 0) {
    test_multiple_threads();
  }
  
  if(strcmp(argv[1], "3") == 0 || strcmp(argv[1], "all") == 0) {
    test_shared_memory();
  }
  
  printf(1, "\nAll tests completed!\n");
  exit();
}
