#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>

void syscallError(int ret, const char* msg) 
{
	if (ret < 0)
	{
		perror(msg);
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char* argv[])
{
	int N = atoi(argv[1]), K = atoi(argv[2]), message[2] = {1, 1} /*result, mult*/, pids[N], repeat = K/N, extraloop = K%N;
	int fd[N][2];
	if (N == 0) 
	{
		if (K == 0) printf("1\n");
		else printf("Please insert o number for children!\n");
		return 0;
	}
	for (int i = 0; i < N; i++) //Creation of pipes
	{
		syscallError(pipe(fd[i]), "Error creating pipe!\n");
	}

	for (int i = 0; i < N; i++)
	{
		pids[i] = fork();
		syscallError(pids[i], "Error creating children!\n");
		if (pids[i] == 0) //Children's code
		{
			if ((repeat == 0 | repeat == 1) & i > extraloop) 
			{
				repeat = 1;
			}
			else if (extraloop - i > 0) repeat++;
			printf("[%d] %d\n", i, repeat);
			for (int j = 0; j < N; j++)
			{
				if ((j != i-1 & i > 0) | (j != N-1 & i == 0)) 
				{
					syscallError(close(fd[j][0]), "Error closing reading end of pipe!\n");
				}
				if (j != i) 
				{
					syscallError(close(fd[j][1]), "Error closing writing end of pipe!\n");
				}
			}
			while (repeat)
			{
				//printf("%d\n", i);
				if (i == 0) syscallError(read(fd[N-1][0], message, 2*sizeof(int)), "Error reading end of pipe!\n");
				else syscallError(read(fd[i-1][0], message, 2*sizeof(int)), "Error reading end of pipe!\n");
				message[0] = message[0]*message[1];
				message[1]++;
				if (message[1] <= K) syscallError(write (fd[i][1], message, 2*sizeof(int)), "Error writing to pipe!");
				repeat--;
			}
			if (extraloop - i == 1) printf("%d\n", message[0]);
			if (i == 0) syscallError(close(fd[N-1][0]), "Error closing reading end of pipe!\n");
			else syscallError(close(fd[i-1][0]), "Error closing reading end of pipe!\n");
			syscallError(close(fd[i][1]), "Error closing reading end of pipe!\n");
			exit(0);
		}
	}
	for (int i = 0; i < N; i++)
	{
		syscallError(close(fd[i][0]), "Error closing reading end of pipe!\n");
		if (i != N-1) 
		{
			syscallError(close(fd[i][1]), "Error closing writing end of pipe!\n");
		}
	}
	syscallError(write (fd[N-1][1], message, 2*sizeof(int)), "Error writing to pipe!");
	for (int i = 0; i < N; i++)
	{
		syscallError(waitpid(pids[i], NULL, 0), "Error waiting for children!\n");
	}

	return 0;	
}