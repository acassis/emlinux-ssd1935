/*======================================================================================

* piulib.c - PIU library
* Version 1.1
* Author:	Shao Wei
* Date:		15 Oct 2007
*
========================================================================================*/
/* We need F_SETSIG */
#define _GNU_SOURCE	1

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>

#ifdef __USE_GNU
#warning "define __USE_GNU"
#endif

//#define PIU_SIGINFO			// if defined, use sig_handler, otherwise, use sigaction

#define dbg printf

#include "piu_types.h"
#include "piulib.h"

struct piu_ctx
{
	piu_callback_t	func;
	void		*ctx;
//	pid_t		pid;
};

pthread_t piu_sig_thread_t;

static struct piu_ctx	_piu_ctx[PIU_MAX_QUEUES];
static int	_hpiu = 0;
static uint32_t	c_num = 0;
volatile uint8_t piu_status_flag = 0; 
volatile uint8_t piu_pending_msg_flag = 0;
volatile int 	piu_signal_thread_id = 0;
			
#ifndef PIU_SIGINFO
static void piulib_sig(int sig);
#else
static void piulib_sig(int sig, siginfo_t *info, void *ctx);
#endif
int piu_signal_thread();

int piulib_init()
{
	sigset_t mask;
	sigset_t oldmask;
	//struct sigaction act;
	//int	rt;
	//long	f;	
	uint32_t rtsigno = SIGRTMIN;

	c_num = 0;
	_hpiu = 0;
	memset(_piu_ctx, 0, PIU_MAX_QUEUES * sizeof(struct piu_ctx));

	/* signal mask */
	sigemptyset(&mask);
	sigaddset(&mask, rtsigno);
	
	/* send signals to handler() and keep all signals blocked 
	   that handler() has been configured to catch to avoid
	   races in manipulating the global variables */
#ifndef PIU_SIGINFO
	//act.sa_handler = piulib_sig;
	//act.sa_flags = 0;// SA_RESTART;
#else
	//act.sa_sigaction = piulib_sig;
	//act.sa_flags = SA_RESTART;//SA_SIGINFO;
#endif
	//act.sa_mask = mask;

	/* signals to be handled */
	//sigaction(rtsigno, &act, NULL);
	//sigaction(SIGRTMIN+1, &act, NULL);
#if 1
	//block the signal
	if(pthread_sigmask(SIG_BLOCK, &mask,&oldmask) != 0)
	{
		dbg("pthread_sigmask fails \n");
		return -1;
	}
#endif

	dbg("PIU sig thread parent PID %d\n", getpid());

	//create signal thread
	pthread_create(&piu_sig_thread_t,NULL,(void*(*)())piu_signal_thread,NULL);

	return 0;
}

void piulib_exit(void)
{
	c_num = 0;

	if(piu_signal_thread_id)
		dbg("PIULIB: Warning-PIU signal thread %d not exit yet\n", piu_signal_thread_id);
  //  pthread_exit(piu_signal_thread);
	if(!_hpiu)
		dbg("PIULIB ERROR: PIU fd is Zero\n");
	else
		close(_hpiu);
	piu_status_flag = 0;
	_hpiu = 0;
}

void piulib_enable(void)
{
	int	rt;
	uint32_t status = 0x3F;

	if(_hpiu == 0)
	{
		dbg("PIULIB: piulib_enable error, PIU device is closed already\n");
		return;
	}	
	// clear any pending status before re-enable the IRQs
	rt = ioctl(_hpiu, PIU_IOCTL_CLR_STATUS, &status);
	
	if (rt != 0)
	{
		perror("PIULIB: PIU_IOCTL_CLR_STATUS\n");		
	}

	rt = ioctl(_hpiu, PIU_IOCTL_ENA);
	
	if (rt != 0)
	{
		perror("PIULIB: PIU_IOCTL_ENA\n");		
	}
	rt = ioctl(_hpiu, PIU_IOCTL_NONBLOCK_ENA);
	
	if (rt != 0)
	{
		perror("PIULIB: PIU_IOCTL_NONBLOCK_ENA\n");		
	}

	//set piu_status_flag after PIU IRQ is re-enabled
	piu_status_flag = 1;
	dbg(".......PIU_IRQ enabled.......\n");
	return;
}

// disable PIU interrupts and clear the pending status bit
void piulib_disable(void)
{
	int	rt;
	uint32_t status = 0x3F;

 	if(_hpiu == 0)
        {
                dbg("PIULIB: piulib_disable error, PIU device is closed already\n");
                return;
        }

	rt = ioctl(_hpiu, PIU_IOCTL_DIS);
	
	if (rt != 0)
	{
		perror("PIULIB: PIU_IOCTL_DIS\n");		
	}
	rt = ioctl(_hpiu, PIU_IOCTL_NONBLOCK_DIS);
	
	if (rt != 0)
	{
		perror("PIULIB: PIU_IOCTL_NONBLOCK_DIS\n");		
	}
	
	// clear pending IRQs after disable IRQs
	rt = ioctl(_hpiu, PIU_IOCTL_CLR_STATUS, &status);
	
	if (rt != 0)
	{
		perror("PIULIB: PIU_IOCTL_CLR_STATUS\n");		
	}

       //reset piu_status_flag after PIU IRQ is disabled
	piu_status_flag = 0;
	dbg("............PIU_IRQ Disabled.............\n");
	return;
}

// q_id: start from 1
void piu_register(piu_callback_t call, piu_queue_e id, void *ctx)
{
	uint32_t	q_id = (uint32_t)id - 1;

/* //clean 071219 move piu init to device open
	if (c_num)
	{
		ioctl(_hpiu, PIU_IOCTL_DIS);
#ifdef PIU_NONBLOCK
		ioctl(_hpiu, PIU_IOCTL_NONBLOCK_DIS);
#endif
	}
*/
	if (!_piu_ctx[q_id].func && call)
	{
		c_num++;
	}
	if (_piu_ctx[q_id].func && !call)
	{
		c_num--;
	}
	_piu_ctx[q_id].func = call;
	_piu_ctx[q_id].ctx = ctx;
/* 
	// if want to differentiate between processes
	if (call)
	{
		_piu_ctx[q_id].pid = getpid();
	}
	else
	{
		_piu_ctx[q_id].pid = 0;
	}
*/

/* //clean 071219 move piu init to device open
	if (c_num)
	{
		ioctl(_hpiu, PIU_IOCTL_ENA);
#ifdef PIU_NONBLOCK
		ioctl(_hpiu, PIU_IOCTL_NONBLOCK_ENA);
#endif
	}
*/
}

int piu_tx(uint32_t msg, piu_msg_p body)
{
	struct piu_protl	m;

	memset(&m, 0, sizeof(struct piu_protl));
	m.msg = msg;
	memcpy(&(m.msg_body), body, sizeof(piu_msg_t));

	if(_hpiu == 0)
	{
		dbg("PIULIB: piul_tx error, PIU device is closed already\n");
		return -1;
	}

	// if PIU status is disabled, just discard this message
	if(piu_status_flag == 0)
	{
		//dbg("piu msg %d is discarded due to piu_status_flag\n", msg);
		return -1;
	}

	if (ioctl(_hpiu, PIU_IOCTL_TX, (void *)&m))
	{
		dbg("ioctl err - ioctl, PIU_IOCTL_TX\n");
		goto l_tx_err;
	}
	return 0;

l_tx_err:
	return -1;
}

//static unsigned int sigcount =0;
/* internal funtions */
#ifndef PIU_SIGINFO
static void piulib_sig(int sig)
{
	uint32_t	msg;
	int 		ret;
	uint32_t	cmd[3];
//dbg("start of piulib_sig\n");
//dbg("sigcount = %u\n",sigcount++);
	if (sig == SIGRTMIN)
	{
		cmd[0] = PIU_IOCTL_RX_MSG;
		cmd[1] = PIU_IOCTL_RX;
		//cmd[2] = PIU_IOCTL_ENA;
	}
/*
// cannot use this, because we can only emit one real-time signal number from driver to this process
#ifdef PIU_NONBLOCK
	else if (sig == SIGRTMIN+1)
	{
		cmd[0] = PIU_IOCTL_NONBLOCK_RX_MSG;
		cmd[1] = PIU_IOCTL_NONBLOCK_RX;
		//cmd[2] = PIU_IOCTL_NONBLOCK_ENA;
	}
#endif
*/
#else
static void piulib_sig(int sig, siginfo_t *info, void *ctx)
{	
	uint32_t	msg;
	int 		ret;
	uint32_t	cmd[3];
//dbg("start of piulib_sig\n");	
	if (info->si_signo == SIGRTMIN)
	{
		cmd[0] = PIU_IOCTL_RX_MSG;
		cmd[1] = PIU_IOCTL_RX;
		//cmd[2] = PIU_IOCTL_ENA;
	}
/*
#ifdef PIU_NONBLOCK
	else if (info.si_signo == SIGRTMIN+1)
	{
		cmd[0] = PIU_IOCTL_NONBLOCK_RX_MSG;
		cmd[1] = PIU_IOCTL_NONBLOCK_RX;
		//cmd[2] = PIU_IOCTL_NONBLOCK_ENA;
	}
#endif
*/
#endif
	else
	{
		dbg("PIULIB: Wrong Signal ID %d received in piulib_sig\n", sig);
		return;
	}
	
	if(_hpiu == 0)
	{
		printf("PIULIB: piulib_sig error, PIU device is closed already\n");
		return;
	}

	//discard the message if PIU is disabled already
	if(piu_status_flag == 0)
	{
		//dbg("PIULIB: PIU signal is discarded due to piu_status_flag\n", msg);
		return;
	}
	//ret = ioctl(_hpiu, cmd[0], (void *)&msg);
	//if (!ret)
	//{
	//	msg--;
	//	if (msg < PIU_MAX_QUEUES)
		{
			//clean 080504 move the ioctl() out of "if(_piu_ctx[msg].func)"
			struct piu_protl m;

			memset(&m, 0, sizeof(struct piu_protl));	
			if (ioctl(_hpiu, cmd[1], (void *)&m))
			{
				dbg("PIULIB - IOCTL err\n");
				return;
			}
			m.msg--;
			if(m.msg > PIU_MAX_QUEUES)
           		{
				dbg("PIULIB: Queue id overflow %x\n", m.msg);
				return;
			}
			if (_piu_ctx[m.msg].func)
			{
				//m.msg--;
				_piu_ctx[m.msg].func(_piu_ctx[m.msg].ctx, &m.msg_body);
			}
			else
			{
				dbg("PIULIB: No handler installed for queue id %d\n", m.msg);
				return;
			}
		}
		//else
		//{
	//		dbg("PIULIB: Queue id overflow %x\n", msg);
	//	}
	//}
//dbg("end of piulib_sig\n");	
}

int piu_signal_thread()
{
	sigset_t mask;
	sigset_t oldmask;
	uint32_t rtsigno = SIGRTMIN;
	uint32_t sig_num;
	int	rt;
	long	f;

	/* signal mask */
	sigemptyset(&mask);
	sigaddset(&mask, rtsigno);
	// add SIGTERM to let application terminate piu signal thread
	sigaddset(&mask, SIGTERM);

	//block the signal
	if(pthread_sigmask(SIG_BLOCK, &mask,&oldmask) != 0)
	{ 
		dbg("pthread_sigmask fails \n");
		return -1;
	}

	_hpiu = open("/dev/piu", O_RDWR);	// O_SYNC | O_RDWR);
	if (_hpiu == -1)
	{
		perror("PIULIB: init err - open\n");
		return -1; 
	}
	dbg("PIU Signal Handler Thread PID %d\n", getpid());

	rt = fcntl(_hpiu, F_SETOWN, getpid());
	if (rt == -1)
	{
		perror("PIULIB: F_SETOWN\n");
		goto l_close;
	}
	f = fcntl(_hpiu, F_GETFL);
	if (rt == -1)
	{
		perror("PIULIB: F_GETFL\n");
		goto l_close;
	}
	rt = fcntl(_hpiu, F_SETFL, f | O_NONBLOCK | FASYNC); 	//f | FASYNC);
	if (rt == -1)
	{
		perror("PIULIB: F_SETFL\n");
		goto l_close;
	}
	
	if (-1 == fcntl(_hpiu, F_SETSIG, rtsigno))
	{
		perror("PIULIB: F_SETSIG err\n");
		goto l_close; 
	}

	if (ioctl(_hpiu, PIU_IOCTL_RTSIGNO, (void *)&rtsigno))
	{
		perror("PIULIB: IOCTL err\n");
		goto l_close;
	}
	//printf("PIULIB: SIGRTMIN=%d\n", SIGRTMIN);
	//c_num = 0;
	//memset(_piu_ctx, 0, PIU_MAX_QUEUES * sizeof(struct piu_ctx));

    //set PIU status flag to 1 after PIU is enabled
	piu_status_flag = 1;
	piu_pending_msg_flag = 0;
	piu_signal_thread_id = getpid();
	dbg("piu_signal_thread_created !\n");

	while(1)
	{
		//raise(rtsigno);
		if(!sigwait(&mask, &sig_num))
		{
			if(sig_num == SIGTERM) //If we receive a sigterm, exit the thread
			{
				dbg("PIULIB: SIGIO received, Exit PIU signal thread\n");
				dbg("PIULIB: signal parent to be terminated immediately\n");
				kill(getpid(), SIGKILL);
				break;
			}
			// else we handle the PIU signal
			piu_pending_msg_flag = 1;
			//dbg("sigwait receive sig %d\n", sig_num);
			piulib_sig(sig_num);
			piu_pending_msg_flag = 0;
		}
		else
			 dbg("ERROR:sigwait fails \n");	
	}
	// disable PIU before exit
	piulib_disable();
 	piu_pending_msg_flag = 0;
	piu_status_flag = 0;
	piu_signal_thread_id = 0;
	dbg("PIULIB: piu_signal_thread exits\n");
	return 0;
l_close:
	close(_hpiu);
	_hpiu = 0;
	return -1;
}

int8_t piulib_wait_piu_complete()
{
	uint32_t count = 0;
	while(piu_pending_msg_flag)
	{	
		count++;
		if(count > 1000) //timeout in 1second
			return -1;
		usleep(1);
	}
	return 0;
}
// retrieve piu signal thread id
int piulib_get_piu_thread_id()
{
	return piu_signal_thread_id;
}

