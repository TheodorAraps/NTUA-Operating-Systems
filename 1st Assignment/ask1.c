#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>                          
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

#define CYAN "\033[36m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define WHITE "\033[37m"

char buffer[5000], message[30];

void ChildTask(char* colour, int ChildNumber, int repeats, int initiate, int file_des)
{		
  sprintf(message, "message from %d\n", getpid()); //message to be written in the file
	printf("%s[Child%d] Started. PID = %d PPID = %d\n", colour, ChildNumber, getpid(), getppid());
	for (int i = initiate; i <= repeats; i += 2)
	{
		printf("%s[Child%d] Heartbeat PID = %d Time = %ld x = %d\n", colour, ChildNumber, getpid(), (long int) time(NULL), i);
		if (write(file_des, message, strlen(message)) == -1)
		{
			printf(WHITE "%s[Child%d] Failed to write to the file!\n", colour, ChildNumber);
			exit(0);
		}
		sleep(1);
	}
	printf("%s[Child%d] Terminating!\n", colour, ChildNumber);	
	exit(0);
}

void file_manipulation(int file_des, char* buf) 
{
	int n_read, n_write;	
	do
	{
		n_read = read(file_des, buf, sizeof(buf));
		if (n_read == -1)
		{
			printf(WHITE "Error reading the file!\n");
		}

		n_write = write(1, buf, n_read); //1 refers to STD_OUT
		if (n_write == -1)
		{
			printf(WHITE "Error Printing the contents of file!\n");
		}
	} while (n_read > 0); //if n_read == 0 --> EOF
}

int main(int argc, char* argv[])
{
  int N = atoi(argv[2]); //atoi() turns a string to an integer   
  int fd = open(argv[1], O_CREAT|O_TRUNC|O_RDWR, 0660); //File Opening-Creation (if non-existent) and cleared
  if (fd == -1)
  {
  	printf("Failed to open the file!\n");
   	exit(0);
  } 
     		
  pid_t C1 = fork(); //Process C1 creation
    
  if (C1 == -1) //C1 error case
  {
    perror("Child1 failed to be created!");
    exit(0);
  }
  else if (C1 != 0) //Parent Process                           
  {   
  	pid_t C2 = fork(); //Process C2 creation
        
    if (C2 == -1) //C2 error case                       
    {
      perror("Child2 failed to be created!");
      exit(0);
    }  
    else if (C2 != 0)
    {
      sprintf(message, "message from %d\n", getpid()); //Message to be wriiten in the file
      for (int k = 0; k < N/2; k++)
      {
        printf(YELLOW "[Parent] Heartbeat PID = %d Time = %ld\n", getpid(), (long int) time(NULL));
        if (write(fd, message, strlen(message)) == -1) 
        {
          printf(WHITE "[Parent] Failed to write to the file!\n");
          exit(0);
        }
        sleep(1);
      }
      printf(YELLOW "[Parent] Waiting for Children...\n");
      printf(YELLOW "[Parent] Child with PID = %d terminated\n", wait(NULL));
      printf(YELLOW "[Parent] Waiting for Child\n"); 
      printf(YELLOW "[Parent] Child with PID = %d terminated\n", wait(NULL));
      printf(YELLOW "[Parent] PID = %d Reading file:" WHITE "\n", getpid());
		  if (lseek(fd, 0, SEEK_SET) == -1) //Initialising the offset & Error case of resetting offset
		  {
			  printf(WHITE "Offset set failed");
		  }
      file_manipulation(fd, buffer);
		  close(fd);
      printf(YELLOW "[Parent] PID = %d Bye\n", getpid());
      return 0;
    }
    else ChildTask(CYAN, 2, N, 1, fd); //C2 Process                        
  }
  else ChildTask(GREEN, 1, N, 0, fd); //C1 Process
}