/*****************************************
* test.c - Illustrate signal queuing and ordering
* 1) If "#if 1", must use send.c to send signal, this program is used to receive signals
* 2) If "#if 0", the program send itself signals
*/
#define _GNU_SOURCE	1

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define dbg printf

/* globals for building a list of caught signals */
volatile int nextSig = 0;
volatile int sigOrder[10];

/* catch a signal and record that it was handleed */
void handler(int signo)
{
//	int i= 900000;
	sigOrder[nextSig++] = signo;
//	while (i--);
	printf("%d), sig=%d\n", nextSig, signo);
}

int main()
{
	sigset_t mask;
	sigset_t oldmask;
	struct sigaction act;
	int	i;
	
	/* signal mask */
	sigemptyset(&mask);
	sigaddset(&mask, SIGRTMIN);
	sigaddset(&mask, SIGRTMIN+1);
	sigaddset(&mask, SIGUSR1);
	
	/* send signals to handler() and keep all signals blocked 
	   that handler() has been configured to catch to avoid
	   races in manipulating the global variables */
	act.sa_handler = handler;
	act.sa_mask = mask;
	act.sa_flags = 0;
	
	/* signals to be handled */
	sigaction(SIGRTMIN, &act, NULL);
	sigaction(SIGRTMIN+1, &act, NULL);
	sigaction(SIGUSR1, &act, NULL);
	
#if 1	// act as signal receiver only
	while (nextSig<6);
#else	// act as signal sender and receiver
	/* block the signals we are working with so we can see the 
	   queuing and ordering behavior */
	sigprocmask(SIG_BLOCK, &mask, &oldmask);
	
	/* generating signals */

	/* raise - send signal to process itself */
	raise(SIGRTMIN + 1);
	raise(SIGRTMIN);
	raise(SIGRTMIN);
	raise(SIGRTMIN + 1);
	raise(SIGRTMIN);
	raise(SIGUSR1);
	raise(SIGUSR1);
	/* enable delivery of the signals. They'll all be delivered 
	   right before this call returns (on Linux; this is NOT 
	   protable behavior)*/
	sigprocmask(SIG_SETMASK, &oldmask, NULL);
#endif

	
	/* Display the ordered list of signals we caught */
	printf("signals received:\n");
	for (i=0; i<nextSig; i++)
	{
		if (sigOrder[i] < SIGRTMIN)
		{
			printf("\t%s\n", strsignal(sigOrder[i]));
		}
		else
		{
			printf("\tSIGRTMIN + %d\n", sigOrder[i] - SIGRTMIN);
		}
	}
	return 0;
}


