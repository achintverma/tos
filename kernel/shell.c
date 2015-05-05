#include <kernel.h>

#define KB_SPACE '\040'
#define KB_TAB '\011'
#define KB_ENTER '\015'
#define KB_BACK '\010'
// #define const char* COMMANDS[] = {"clr","go","stop"};
static WINDOW shell_window = {0, 11, 80, 15, 0, 0, 0xA6, TRUE, "Shell"};
static WINDOW shell_border = {0, 10, 80, 15, 0, 0, 0xA6, TRUE, "Shell Border"};
void tos_clr();
int  tos_string_compare(char*, char*);
void print_string(char*);
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
	 
	 // render the border 
	 wprintf(&shell_border, "________________________________________________________________________________");
	 
	 tos_prompt();
	 
	 show_cursor(kernel_window); 	
		
	 char ch;
	 Keyb_Message msg;
	 
	 char* input_string;
	 int char_num = 0;
	 char clr[] 		= "clr";
	 char ps[] 			= "ps";
	 char ports[] 		= "ports";
	 char echo[] 		= "echo";
	 char check[] 		= "check";
	 char train_stop[] 	= "stop";
	 
	 while (1) { 
		msg.key_buffer = &ch;
		send(keyb_port, &msg);
		wprintf(&shell_window, "%c", ch);
		
		// CHECK IF ENTER KEY WAS PRESSED
		if(ch == KB_ENTER){
					
			clear_whites(input_string, cmd_string, arguments, char_num);
		
			// reset all counters everytime ENTER is pressed 
			char_num 		= 0;
			
			// check if the comparison is made properly or not
			if(tos_string_compare(cmd_string, ports))
				tos_ports();
			else if(tos_string_compare(cmd_string, ps))
				tos_ps();
			else if(tos_string_compare(cmd_string, clr))
				tos_clr();	
			else if(tos_string_compare(cmd_string, echo))
				tos_echo();	
			else
			{
				wprintf(&shell_window, "\n Error: Bad Command. Please check syntax.");
			}
			
			
			*arguments = '\0';
			tos_prompt();
			
			
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
	
} 

/*
 * function to clear extra white spaces from start, end and between words. It also identifies command and argument strings.
 * 
 * @param string input_string
 * @param string command_string
 * @param string arguments
 * @param integer number_of_characters
 * 
 */


void clear_whites(char *input_ch, char *cmd, char *arg, int char_num){
	
	int i = 0;
	int j = 0;
	int in_command = FALSE;
	int is_first_block = TRUE;
	
  // process all characters typed on shell 	
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
				// command string found, add a terminator and prepare to capture arguments 
				*(cmd+j) = '\0';
				j=0; 						// reset J so it can be used again for arguments
				is_first_block = FALSE;
				
			}
					
			in_command = FALSE;
		}
		else
			continue; 						// if there are more than one spaces, skip
				
	}
	else
	{
		if(is_first_block == TRUE)
			*(cmd+j) = ch;
		else
			*(arg+j) = ch;
		
		j++;
		in_command = TRUE;
	}
  }	
	
	
	
	if(is_first_block == TRUE)			// if only one word is entered, command string need to be terminated
	*(cmd+j) = '\0';
	
	if(in_command == FALSE)				// if last character is a space move pointer one space backwards
	j--;
	
	*(arg+j) = '\0';					// terminate the argument string
		
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
	wprintf(&shell_window, "%c",*str++);
}



/* TOS SHELL COMMANDS START HERE */


// function to show command prompt

void tos_prompt(){

	wprintf(&shell_window, "\nTOS > ");

}

// function to clear screen
void tos_clr(){

	clear_window(&shell_window);
	//print_string(arguments);
}

// function to print list of processes 

void tos_ps(){
	
	int i;
    PCB* p = pcb;
    
    wprintf(&shell_window, "PCB Priority State           Active Name\n");
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
		
		
		wprintf(&shell_window, "%d  ", i);					// pcb entry
		wprintf(&shell_window, "  %2d     ", p->priority); // priority 
		wprintf(&shell_window, state[p->state]);      		// state
		
		if (p == active_proc)          		// active process?     
			wprintf(&shell_window, " *      ");
		else
			wprintf(&shell_window, "        ");
		
		//kprintf(" %s\n", p->name);
		
		wprintf(&shell_window, "%s\n", p->name);	   		// process name 
		
		
    }
    
}

// function to print list of ports 

void tos_ports(){
	
	int i;
	PORT_DEF *pr = port;
	PROCESS ps; 
	
	wprintf(&shell_window, "Port Process\n");
	
	for(i=0; i<MAX_PORTS -1; i++, pr++){
		
		if(pr->used == 0)
		continue;
	
		ps = pr->owner;
		wprintf(&shell_window, "\n %d    %s", i, ps->name);
			
	}
	
}


// function to echo a message 
void tos_echo(){
	
	wprintf(&shell_window, "\n %s", arguments);
	
}




	
	
