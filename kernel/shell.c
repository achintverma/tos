#include <kernel.h>

WINDOW train_window =        {0, 0, 60, 11, 0, 0, ' '}; // window for train milestone messages

WINDOW train_status_head =   {60, 0, 20, 2, 0, 0, ' '}; // window for heading of status
WINDOW train_status =        {60, 2, 20, 9, 0, 0, ' '}; // shows live train commands

static WINDOW shell_border = {0, 11, 80, 2, 0, 0, 0, TRUE, "Shell Border"};

WINDOW shell_window =        {0, 12, 80, 10, 0, 0, 0xA6, TRUE, "Shell"};


// Array of commands in tos shell.
command commands_array[MAX_COMMANDS];

// Global variables so command and arguments can be accessed by any method
char arguments[20] = "";
char cmd_string[10] = ""; 



/*
 * lookup_command_array() function searches for the command entered by the user in the
 * command array and returns the match if found.
 * 
 * */
command* lookup_command(const command *commands, const char *user_input_command){
	
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

/*
 *   functon to clear entire screen and render the shell border window and TOS prompt
 */

void render_shell()
{
     // clear the window every time you open the shell.
	 clear_window(kernel_window);
	 
	 // render the shell top border 
	 wprintf(&shell_border, "============================== TRAIN OS TERMINAL ===============================");
	 
	 tos_prompt();	
}

/*
 *   functon to capture each key pressed on shell and call function when ENTER key is pressed 
 */

void start_kb(PROCESS self, PARAM param)
{
	
	 char ch;
	 Keyb_Message msg;
	 
	 char* input_string;                // holds entire string types in shell
	 int char_num = 0;                  // used to keep track of characters entered in shell
	 
	 command* command_var;
	 void (*caller)();
	 
	 // render the shell
	 render_shell();
	 
	 while (1) { 
		msg.key_buffer = &ch;
		send(keyb_port, &msg);
		wprintf(&shell_window, "%c", ch);
		
		// CHECK IF ENTER KEY WAS PRESSED
		if(ch == KB_ENTER){
					
			// remove all whitespaces (including extra spaces between command and argument) and 
			// prepare command and argument string
			clear_whites(input_string, char_num);
		
			// reset counter everytime ENTER is pressed 
			char_num 		= 0;
			
			command_var = lookup_command(commands_array, cmd_string);
				
				
				if (command_var->func == NULL)
				{
					wprintf(&shell_window, "\n Error: Bad Command. Please check syntax.");
				}
				else
				{	
					// run the command as per the user input.
					caller = command_var->func;
					caller();
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
		// valid character found, lets put it in command or argument
		
		if(is_first_block == TRUE)
			cmd_string[j] = ch;
		else
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


/* TOS SHELL COMMANDS START HERE */


// function to show command prompt after command is done

void tos_prompt(){

	wprintf(&shell_window, "\nTOS > ");

}

// function to clear screen
void tos_clr(){

	clear_window(&shell_window);
	
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
		wprintf(&shell_window, "  %2d     ", p->priority);  // priority 
		wprintf(&shell_window, state[p->state]);      		// state
		
		if (p == active_proc)          		                // active process?     
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
	
	wprintf(&shell_window, "%s", arguments);
	
}

// function to start train 
void tos_train(){
	
	wprintf(&shell_window,"\ntrain starting...");
	init_train();
	
}

// function to print all functions 
void tos_help()
{
	int i = 0;
	
	wprintf(&shell_window, "\n Available Commands ");  
	
	for(i=0; i < MAX_COMMANDS; i++)
	{
	   if(commands_array[i].func == NULL)
	     continue;
	    
	   wprintf(&shell_window, "\n - %s     %s", commands_array[i].display_name, commands_array[i].description);  
			
	}
	
}

/*
 *  function to initialize commands in the command struct
 */
 
void init_command(void (*func) ,char *name, char* disp_name, char *description, command *cmd) 
{
	cmd->name 			= name;
	cmd->display_name 	= disp_name;
	cmd->func 			= func;
	cmd->description 	= description;
}


/* 
 * function to initialize all functions in the array
 */
void init_shell()
{
	// counter to store the functions in the commands array.
	int counter = 0;
	
	// initialize commands 
	init_command(tos_ps,    "ps",    "ps   ", "Shows list of running processes", 	 &commands_array[counter++]);
	init_command(tos_clr,   "clr",   "clr  ", "Clear Window", 			             &commands_array[counter++]);
	init_command(tos_ports, "ports", "ports", "Show list of running ports",          &commands_array[counter++]);
	init_command(tos_echo,  "echo",  "echo ", "Print the string back on screen",     &commands_array[counter++]);
	init_command(tos_train, "train", "train", "Start train process",                 &commands_array[counter++]);
	init_command(tos_help,  "help",  "help ", "Show list of available commands",     &commands_array[counter++]);
	
	
	// assign the NULL to each of the remaining commands in the array.
	while(counter < MAX_COMMANDS)
	{
		init_command(NULL, "", "", "Command not found", &commands_array[counter++]);
	}
	
	// create shell process
	create_process(start_kb, 5, 0, "Terminal Process");
	resign();
	
}


	
	
