/*****************************************
* test.c - Illustrate signal queuing and ordering
*
*/
#define _GNU_SOURCE	1

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define dbg printf

void * my_thread()
{
 for(;;);
}
		

int main(int argc, char *argv[])
{
	pid_t	pid;
	pthread_t tid;
	pid = (pid_t)atoi(argv[1]);
	pthread_create(&tid, NULL, my_thread, NULL);
	
	for(;;)
	{
		kill(pid, SIGRTMIN);
		sleep(1);
		printf("sent sig %d to pid %d\n",SIGRTMIN, pid);
	}
	/* raise - send signal to process itself */
	//kill(pid, SIGRTMIN + 1);
	//kill(pid, SIGRTMIN);
	//kill(pid, SIGRTMIN);
//	kill(pid, SIGRTMIN + 1);
	//kill(pid, SIGRTMIN);
	//kill(pid, SIGUSR1);
	//kill(pid, SIGUSR1);
	return 0;
}


