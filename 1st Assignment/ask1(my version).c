#include <stdio.h> 
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define RED "\033[31;1m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define WHITE "\033[37m"

char TEXT[100000], Pid[30];

void ChildProcess(char *colour, int k, int i, int pid, int ppid, int n, int fd)
{
    sprintf(Pid, "message from %d\n", pid);
    printf("%s[Chlid%d] Started. PID=%d PPID=%d\n", colour, i, pid, ppid);
    for (k; k < n; k += 2)
    {
      sleep(1);
      printf("%s[Child%d] Heartbeat PID=%d Time=%ld x=%d\n", colour, i, pid, (long int)time(NULL), k);
      write(fd, Pid, strlen(Pid));
    }
    printf("%s[Child%d] Terminating!\n", colour, i);
    exit(-2);
}

int main(int argc, char* argv[])
{
  if (argc != 3)
  {
    printf("False number of Arguments!\n");
    exit(-1);
  }
  int fd = open(argv[1], O_CREAT | O_RDWR | O_TRUNC, 0600);  
  if (fd == -1)
  {
    perror("Error opening file!");
    exit(1);
  }

  int C1 = fork(), C2, n = atoi(argv[2]); 
  if (C1 > 0)
  {
    C2 = fork();
    if (C2 < 0)
    {
      perror("Child wasn't created!");
      exit(-1);
    }
    if (C2 > 0)
    {
      sprintf(Pid, "message from %d\n", getpid());
      for(int k = 0; k < n/2; k++)
      {
        sleep(1);
        printf(YELLOW "[Parent] Heartbeat PID=%d Time=%ld\n", getpid(), (long int)time(NULL));
        write(fd, Pid, strlen(Pid)); 
      }
      printf(YELLOW "[Parent] Waiting for Child\n");
      printf(YELLOW "[Parent] Child with PID=%d terminated\n", wait(NULL));
      printf(YELLOW "[Parent] Waiting for Child\n"); 
      printf(YELLOW "[Parent] Child with PID=%d terminated\n", wait(NULL));
      printf(YELLOW "[Parent] PID=%d Reading file:\n", getpid());
      lseek(fd, 0, SEEK_SET);
      int r = read(fd, TEXT, 500);
      if (r == -1)
      {
        perror("Error reading file!");
        exit(-1);
      }
      printf(WHITE "%s", TEXT);
      printf(YELLOW "[Parent] PID=%d Bye\n", getpid());
      close(fd);
    }
    if (C2 == 0) ChildProcess(GREEN, 1, 2, getpid(), getppid(), n, fd); //Child2
  }
  if (C1 == 0) ChildProcess(RED, 0, 1, getpid(), getppid(), n, fd); //Child1
  if (C1 < 0)
  {
    perror("Child wasn't created!");
    exit(-1);
  }
}