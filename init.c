// init: The initial user-level program

#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#define MAX_INPUT 24
#define MAXLENGTH 3
#define MAXATTEMPTS 3

char *argv[] = { "sh", 0 };

//hardcoded users and passwords
char* users[MAXLENGTH] = {"root", "user", "guest"};
char* pass[MAXLENGTH] = {"password", "pass", "mypassword"};

int login(char *u, char *pass); // prototype

int
main(void)
{
  int pid, wpid;

  if(open("console", O_RDWR) < 0){
    mknod("console", 1, 1);
    open("console", O_RDWR);
  }
  dup(0);  // stdout
  dup(0);  // stderr

  for(;;){
    printf(1, "init: starting sh\n");
    pid = fork();
    if(pid < 0){
      printf(1, "init: fork failed\n");
      exit();
    }
    if(pid == 0){
      char username[MAX_INPUT];
      char password[MAX_INPUT];
      int attempts = 0;
      int is_loggedIn = 0;
      
      while(attempts < MAXATTEMPTS && !is_loggedIn){
      printf(1, "Username: ");
      gets(username, MAX_INPUT);
      
      // Remove trailing newline (from pressing Enter)
      if(username[strlen(username) - 1] == '\n'){
         username[strlen(username) - 1] = '\0';
      }
      printf(1, "Password: ");
      gets(password, MAX_INPUT);
      
      // Remove trailing newline again
      if(password[strlen(password) - 1] == '\n'){
         password[strlen(password) - 1] = '\0';
      }
      
      is_loggedIn = login(username, password);
      
      if(!is_loggedIn){
         printf(1, "Login failed, Try again.\n");
         attempts++;
      }
    }
      
      if(is_loggedIn){
         printf(1, "Welcome %s!\n", username);
         exec("sh", argv);
         printf(1, "init: exec sh failed\n");
         exit();
      }
      else{
         printf(1, "Too many failed attempts. Please reboot the system,\n");
         while(1); //stall the process to prevent shell launch cycle 
                   //from reoccurring via the infinite for loop
      }
      exit();
    }
    while((wpid=wait()) >= 0 && wpid != pid)
      printf(1, "zombie!\n");
  }
}

//checks if given user input is valid
int login(char *u, char *p){
  for(int i = 0; i < MAXLENGTH; i++){
    if(strcmp(u, users[i]) == 0 && strcmp(p, pass[i]) == 0){
      //correct credentials, we can login now
      return 1;
    }
  }
  return 0; //failure
}