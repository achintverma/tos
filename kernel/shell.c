#include <kernel.h>

#define KB_SPACE '\040'
#define KB_TAB '\011'
#define KB_ENTER '\015'
#define KB_BACK '\010'
// #define const char* COMMANDS[] = {"clr","go","stop"};
WINDOW train_window =        {0, 0, 60, 11, 0, 0, ' '};

WINDOW train_status_head =   {60, 0, 20, 2, 0, 0, ' '};
WINDOW train_status =        {60, 2, 20, 9, 0, 0, ' '};

static WINDOW shell_border = {0, 11, 80, 2, 0, 0, 0, TRUE, "Shell Border"};

WINDOW shell_window =        {0, 12, 80, 10, 0, 0, 0xA6, TRUE, "Shell"};


// Array of commands in tos shell.
command commands_array[MAX_COMMANDS + 1];


char arguments[20] = "";
char cmd_string[10] = ""; 

/*
 * dispatch_command() method dispatches the function call to the appropriate command
 * entered by the user on Shell prompt.
 * This function is used for the definition of the dipatch_command()
 * */
void dispatch_command(void (*func) ,char *name, char *description, command *cmd) 
{
	cmd->name = name;
	cmd->func = func;
	cmd->description = description;
}

/*
 * lookup_command_array() function searches for the command entered by the user in the
 * command array and returns the match if found.
 * 
 * */
command* lookup_command_array(const command *commands, const char *user_input_command){
	
	// loop until the match is found.
	for(; commands->func != NULL; commands++)
	{
		if (k_strcmp(commands->name, user_input_command))
		{
			break;
		}
	}
	return (command *)commands;
}


void start_kb(PROCESS self, PARAM param)
{
	 	
	 // clear the window every time you open the shell.
	 clear_window(kernel_window);
	 
	 // render the border 
	 wprintf(&shell_border, "============================== TRAIN OS TERMINAL ===============================");
	 
	 tos_prompt();
	 
	 show_cursor(kernel_window); 	
		
	 char ch;
	 Keyb_Message msg;
	 
	 char* input_string;
	 
	 int char_num = 0;
	 
	 command* command_var;
	 void (*caller_function)();
	 
	 while (1) { 
		msg.key_buffer = &ch;
		send(keyb_port, &msg);
		wprintf(&shell_window, "%c", ch);
		
		// CHECK IF ENTER KEY WAS PRESSED
		if(ch == KB_ENTER){
					
			clear_whites(input_string, char_num);
		
			// reset all counters everytime ENTER is pressed 
			char_num 		= 0;
			
			command_var = lookup_command_array(commands_array, cmd_string);
				
				
				if (command_var->func == NULL)
				{
					wprintf(&shell_window, "\n Error: Bad Command. Please check syntax.");
				}
				else
				{	
					// run the command as per the user input.
					caller_function = command_var->func;
					caller_function();
				}

			
			*arguments = "\0";
			//*cmd_string = "default";
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
	//resign();
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


void clear_whites(char *input_ch, int char_num){
	
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
				//*(arguments+j) = ' ';
				
				arguments[j] = ' ';
				j++;
			 }
			else
			{
				// command string found, add a terminator and prepare to capture arguments 
				// *(cmd_string+j) = '\0';
				
				cmd_string[j] = '\0';
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
			//*(cmd_string+j) = ch;
			cmd_string[j] = ch;
		else
			//*(arguments+j) = ch;
			arguments[j] = ch;
		
		j++;
		in_command = TRUE;
	}
  }	
	
	
	
	if(is_first_block == TRUE)			// if only one word is entered, command string need to be terminated
	cmd_string[j] = '\0';
	//*(cmd_string+j) = '\0';
	
	if(in_command == FALSE)				// if last character is a space move pointer one space backwards
	j--;
	
	*(arguments+j) = '\0';					// terminate the argument string
		
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

// function to start train 
void tos_train(){
	
	wprintf(&shell_window,"\ntrain starting...");
	init_train();
	
}





/* 
 * init_shell() method will initialize the Shell process functions which associated with each commands
 * entered on the prompt.
 * 
 * */
void init_shell()
{
	// counter to store the functions in the commands array.
	int counter = 0;
	
	// initialize all the functions for commands
	dispatch_command(tos_ps, "ps", "Displays a list of all running processes", &commands_array[counter++]);
	dispatch_command(tos_clr, "clr", "Use to clear the shell window", &commands_array[counter++]);
	dispatch_command(tos_ports, "ports", "Displays a list of all used ports", &commands_array[counter++]);
	dispatch_command(tos_echo, "echo", "Echoes the function arguments", &commands_array[counter++]);
	dispatch_command(tos_train, "train", "Initialize and start Train process", &commands_array[counter++]);
	
	
	// assign the NULL to each of the remaining commands in the array.
	while(counter < MAX_COMMANDS){
		dispatch_command(NULL, "NCF", "No command found", &commands_array[counter++]);
	}
	commands_array[MAX_COMMANDS].name 			= "NULL";
	commands_array[MAX_COMMANDS].func 			=  NULL;
	commands_array[MAX_COMMANDS].description 	= "NULL";
	
	
	// once all functions are initialized, create the shell process.
	create_process(start_kb, 5, 0, "Terminal Process");
	resign();
	
}


	
	
