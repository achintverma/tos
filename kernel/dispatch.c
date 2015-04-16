
#include <kernel.h>

#include "disptable.c"


PROCESS active_proc;


/*
 * Ready queues for all eight priorities.
 */
PCB *ready_queue [MAX_READY_QUEUES];

/*
 * The bits in ready_procs tell which ready queue is empty.
 * The MSB of ready_procs corresponds to ready_queue[7].
 */
unsigned ready_procs;



/*
 * add_ready_queue
 *----------------------------------------------------------------------------
 * The process pointed to by p is put the ready queue.
 * The appropiate ready queue is determined by p->priority.
 */

void add_ready_queue (PROCESS proc)
{
    int          prio;
    assert (proc->magic == MAGIC_PCB);
    prio = proc->priority;
    if (ready_queue [prio] == NULL) {
	/* The only process on this priority level */
	ready_queue [prio] 	= proc;
	proc->next         	= proc;
	proc->prev 	   		= proc;
	
	ready_procs = ready_procs | (1 << prio); // ? 
	
    } else {
	/* Some other processes on this priority level */
	
	
	// connect NEXT of this new process to first process in the doubly linked list 
	proc->next  = ready_queue [prio];
	
	// connect PREV of this new process to last process in the doubly linked list
	proc->prev  = ready_queue [prio]->prev;
	
	// connect the NEXT of the last process to new process
	ready_queue [prio]->prev->next = proc;
	
	// connect PREV of first process to point to new process to make it last in the queue
	ready_queue [prio]->prev       = proc;
    }
    
    // make the state of this new process READY so it can be executed now
    proc->state = STATE_READY;
}



/*
 * remove_ready_queue
 *----------------------------------------------------------------------------
 * The process pointed to by p is dequeued from the ready
 * queue.
 */

void remove_ready_queue (PROCESS proc)
{
    int prio;
    assert (proc->magic == MAGIC_PCB);
    
    prio = proc->priority;
    
    if (proc->next == proc) {
	/* No further processes on this priority level */
	ready_queue [prio] = NULL;
	
	// set the bit flag associated to the ready_queue[prio] for ready_proc to zero
	ready_procs &= ~(1 << prio);
    
    } else {
		
	ready_queue [prio] = proc->next;
	proc->next->prev   = proc->prev;
	proc->prev->next   = proc->next;
	
    }
}



/*
 * dispatcher
 *----------------------------------------------------------------------------
 * Determines a new process to be dispatched. The process
 * with the highest priority is taken. Within one priority
 * level round robin is used.
 */

PROCESS dispatcher()
{
    
   int i;
    
   for(i=7; i >= 0; i--)
   {
		if(ready_queue[i] != NULL)
		{
			if(i > active_proc->priority)
				return ready_queue[i];
			else
				return active_proc->next;
		}
		
   }
   
   return (PROCESS) NULL;
   
}



/*
 * resign
 *----------------------------------------------------------------------------
 * The current process gives up the CPU voluntarily. The
 * next running process is determined via dispatcher().
 * The stack of the calling process is setup such that it
 * looks like an interrupt.
 */
void resign()
{
}



/*
 * init_dispatcher
 *----------------------------------------------------------------------------
 * Initializes the necessary data structures.
 */

void init_dispatcher()
{
    int i;

    for (i = 0; i < MAX_READY_QUEUES; i++)
	ready_queue [i] = NULL;

    ready_procs = 0;
    
    /* Setup first process */
    add_ready_queue (active_proc);
}
