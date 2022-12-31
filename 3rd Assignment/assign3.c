#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

//Checks for System-Call Errors
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
	if (argc != 3) 
	{
		printf("Error: Invalid number of arguments! Please try again!\n");
		return 1;
	}
	int N = atoi(argv[1]), K = atoi(argv[2]);
	if ((N <= 1) | (K < 0)) 
	{
		printf("Please insert valid arguments! (N >= 2, K >= 0)\n");
		return 1;
	}
	unsigned long long int message[2] = {1, 1} /*{result, multiplier}*/, pids[N], extraloop = K%N;
	int fd[N][2];
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
			for (int j = 0; j < N; j++)
			{
				if (((j != i-1) & (i > 0)) | ((j != N-1) & (i == 0))) //Children 2, 3, ..., N | Child 1
				{
					syscallError(close(fd[j][0]), "Error closing reading end of pipe!\n");
				}
				if (j != i) 
				{
					syscallError(close(fd[j][1]), "Error closing writing end of pipe!\n");
				}
			}
			while (1)
			{
				if (i == 0) 
					syscallError(read(fd[N-1][0], message, 2*sizeof(unsigned long long int)), "Error reading end of pipe!\n");     //Child 1
				else 
					syscallError(read(fd[i-1][0], message, 2*sizeof(unsigned long long int)), "Error reading end of pipe!\n");     //Rest of children
				if (message[1] <= K) 
				{	
					message[0] *= message[1]; 																                       //Factorial multiplication
					message[1]++;																								   //increase multiplier
				} 																									               
				syscallError(write (fd[i][1], message, 2*sizeof(unsigned long long int)), "Error writing to pipe!");
				if (message[1] > K) break; 																			               //Exit while loop when multiplier is higher than K (K multiplications, though (if K%N != 0) last children will do one more calculation, however it won't get printed)
			}
			if (extraloop - i == 1) printf("%d! = %lld\n", K, message[0]); 													       //Cases (K < a*N-1 or K > a*N+1) and K > 0
			else if ((K == 0) & (i == 0)) printf("%d! = %lld\n", K, message[0]);                                                   //Case K = 0
			else if ((extraloop == 0) & (K != 0) & (i == N-1)) printf("%d! = %lld\n", K, message[0]);                              //Case N = a*K 						               
			if (i == 0) 
				syscallError(close(fd[N-1][0]), "Error closing reading end of pipe!\n");					                       //Child 1: close pipes used
			else 
				syscallError(close(fd[i-1][0]), "Error closing reading end of pipe!\n");							               //Rest of children: close pipes used
			syscallError(close(fd[i][1]), "Error closing writing end of pipe!\n");
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
	syscallError(write (fd[N-1][1], message, 2*sizeof(unsigned long long int)), "Error writing to pipe!");
	syscallError(close(fd[N-1][1]), "Error closing writing end of pipe!\n");
	for (int i = 0; i < N; i++)		//Wait for children
	{
		syscallError(waitpid(pids[i], NULL, 0), "Error waiting for children!\n");
	}

	return 0;	
}