#include <kernel.h>

#define KB_SPACE '\040'
#define KB_TAB '\011'
#define KB_ENTER '\015'
#define KB_BACK '\010'
// #define const char* COMMANDS[] = {"clr","go","stop"};


void tos_clr();
int  tos_string_compare(char*, char*);
void print_string(char*);
void talk_to_com(char*);
void start_kb();
void tos_ps();
void tos_prompt();
void tos_ports();
void clear_whites(char*, char*, char*, int);

char* cmd_string = "a"; 
char* arguments = "";

// init_shell() method will initialize the Shell process.
void init_shell()
{
	create_process(start_kb, 5, 0, "TOS Terminal"); 
}


void start_kb(PROCESS self, PARAM param)
{
	 	
	 // clear the window every time you open the shell.
	 clear_window(kernel_window);
	  
	 kprintf("=========== WELCOME TO TRAIN OS ============\n"); 	
	 tos_prompt();
	 
	 show_cursor(kernel_window); 	
		
	 char ch;
	 Keyb_Message msg;
	 
	 char* input_string;
	 
	 
	 int char_num = 0;
	 // char valid_cmd_chars[] = "abcdefghijklmnopqrstuvwxyz0123456789";
	 char clr[] 	= "clr";
	 char ps[] 		= "ps";
	 char ports[] 	= "ports";
	 char train[] = "run";
	 char check[] = "check";
	 char train_stop[] = "stop";
	 
	 while (1) { 
		msg.key_buffer = &ch;
		send(keyb_port, &msg);
		kprintf("%c", ch);
		
		// CHECK IF ENTER KEY WAS PRESSED
		if(ch == KB_ENTER){
					
			clear_whites(input_string, cmd_string, arguments, char_num);
			
			kprintf("\nCommand: %s ", cmd_string);
			//print_string(cmd_string);
			
			kprintf("\nArguments: %s ", arguments);
			//print_string(arguments);
			
			
			// 2. trim whitespaces 
			
			
			
			// 3. reset all counters everytime ENTER is pressed 
			char_num 		= 0;
			
			// check if the comparison is made properly or not
			if(tos_string_compare(cmd_string, ports))
				tos_ports();
			else if(tos_string_compare(cmd_string, ps))
				tos_ps();
			else if(tos_string_compare(cmd_string, clr))
				tos_clr();	
			
	//		else if(tos_string_compare(cmd_string, train_stop))
		//		talk_to_com4("L20S0\015");
			else
			{
				kprintf("\n Error: Bad Command. Please check Syntax. \n ");
			}
			
			
			*arguments = '\0';
			tos_prompt();
			
			//pass the Command String to COM1 port. 
			//talk_to_com(cmd_string);
		}
		else if(ch == KB_BACK) // there was a backspace hit, decrement char counter
		{
			char_num--;
		}
		else
		{
			//concatenate characters into command
			*(input_string+char_num) = ch;
			 char_num++;
		}
	}
	//ENABLE_INTR(flag);
} 

void clear_whites(char *input_ch, char *cmd, char *arg, int char_num){
	
	int i = 0;
	int j = 0;
	int in_command = FALSE;
	int is_first_block = TRUE;
	
	
	for(i = 0; i < char_num; i++)
	{	
				char ch = *(input_ch+i);
				
				
				
				if(ch == KB_SPACE || ch == KB_TAB)
				{
					
					if(in_command == TRUE)
					{
						
						
						if(is_first_block == FALSE)
						{
							
							*(arg+j) = ' ';
							j++;
						}
						else
						{
							
							is_first_block = FALSE;
							*(cmd+j) = '\0';
							j=0;
							
							print_string(cmd);
						
						}
						
						in_command = FALSE;
					}
					else
						continue;
					
				}
				else
				{
					
					if(is_first_block == TRUE)
					{
						*(cmd+j) = ch;
						
					}
					else
					{
						*(arg+j) = ch;
						
					}	
					// kprintf("%c", tmp_char);
					j++;
					in_command = TRUE;
				}
	}	
	
	// if last character is a space
	
	if(is_first_block == TRUE)
	*(cmd+j) = '\0';
	
	if(in_command == FALSE)
	j--;
	
	*(arg+j) = '\0';
		
}


int tos_string_compare(char* str1, char* str2)
{		
	while(*str1 == *str2)
	{	
		if(*str1 == '\0' || *str2 == '\0'){
			break;
		}
		str1++;
		str2++;
	}	
	if(*str1 == '\0' && *str2 == '\0'){
		return 1;
	}
	else{
		return 0;
	}
}


void print_string(char* str)
{
	while(*str != '\0')
	kprintf("%c",*str++);
}


void talk_to_com(char* cmd_string){	
	char buffer [12]; /* 12 == strlen ("Hello World!") */
	volatile int flag;
	//DISABLE_INTR(flag);
	COM_Message msg;
	//int i;
	msg.output_buffer = cmd_string;
	msg.input_buffer = buffer;
	msg.len_input_buffer = 0;
	
	send (com_port, &msg);
	
	tos_prompt();
	//for (i = 0; i < 12; i++)
	//kprintf ("%c", buffer[i]);
	
	
} 


void talk_to_com1(char* cmd_string){	
	char buffer [12]; /* 12 == strlen ("Hello World!") */
	volatile int flag;
	DISABLE_INTR(flag);
	COM_Message msg;
	//int i;
	msg.output_buffer = cmd_string;
	msg.input_buffer = buffer;
	msg.len_input_buffer = 3;
	//kprintf(input_buffer[1]);
	
	send (com_port, &msg);
	ENABLE_INTR(flag);
	tos_prompt();
	//for (i = 0; i < 12; i++)
	//kprintf ("%c", buffer[i]);
} 

/* TOS SHELL COMMANDS START HERE */


// function to show command prompt

void tos_prompt(){

	kprintf("\nTOS > ");

}

// function to clear screen
void tos_clr(){

	
	clear_window(kernel_window);
	print_string(arguments);
	
}

// function to print list of processes 

void tos_ps(){
	
	int i;
    PCB* p = pcb;
    
    kprintf("PCB Priority State           Active Name\n");
    for (i = 1; i < MAX_PROCS; i++, p++) {
		
		// if pcb is not used, skip
		if (!p->used)
	    continue;
		
		static const char *state[] = 
		{ "  READY          ",
		  "  SEND_BLOCKED   ",
		  "  REPLY_BLOCKED  ",
		  "  RECEIVE_BLOCKED",
		  "  MESSAGE_BLOCKED",
		  "  INTR_BLOCKED   "
		};
		
		
		kprintf("%d  ", i);					// pcb entry
		kprintf("  %2d     ", p->priority); // priority 
		kprintf(state[p->state]);      		// state
		
		if (p == active_proc)          		// active process?     
			kprintf(" *      ");
		else
			kprintf("        ");
		
		//kprintf(" %s\n", p->name);
		
		kprintf("%s\n", p->name);	   		// process name 
		
		
    }
    
}

// function to print list of ports 

void tos_ports(){
	
	int i;
	PORT_DEF *pr = port;
	PROCESS ps; 
	
	kprintf("Port Process\n");
	
	for(i=0; i<MAX_PORTS -1; i++, pr++){
		
		if(pr->used == 0)
		continue;
	
		ps = pr->owner;
		kprintf("\n %d    %s", i, ps->name);
			
	}
	
}





	
	
