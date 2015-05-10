
#include <kernel.h>

void append_cmd(char* target, char* tail)
{
	
	int i = 0;
	int target_length = k_strlen(target);
	
	for(i=0; *tail != '\0'; i++)
		target[target_length+i] = *tail++;

}

// function to send command to the train

void send_command(char *action, int len_buffer, char *input_buffer)
{
	char train_cmd[20] = "";
	append_cmd(train_cmd, action);
	append_cmd(train_cmd, CARRIGE_RETURN);
	
	COM_Message msg;
	msg.output_buffer = train_cmd;
    msg.len_input_buffer = len_buffer;
    msg.input_buffer = input_buffer;
    
    sleep(20); // sleep 20 ticks between each command
    
    wprintf(&train_status, "Command: %s", train_cmd);
    
    send(com_port, &msg);
	
}

/*
 *  function to change speed of the train
 *  
 *  @param char* speed 
 */

void change_speed(char* speed)
{
	char action[10] = "L";
	char dummy; 
	append_cmd(action, TRAIN);
	append_cmd(action, "S");
	append_cmd(action, speed);

	send_command(action, 0, &dummy);
	
}

/*
 *  function to change train direction
 */

void change_direction()
{
	char action[10] = "L";
	char dummy; 
	append_cmd(action, TRAIN);
	append_cmd(action, "D");

	send_command(action, 0, &dummy);
	
}

/*
 *  function to stop train
 */
 
void stop_train()
{
	change_speed("0");
}

/*
 *  function to set a switch 
 *  
 *  @param char* switch_id
 *  @param char* color (valid options are "G" and "R")
 * 
 */

void set_switch(char *switch_id, char *color)
{
	// M4R
	char action[10] = "M";
	char dummy; 
	append_cmd(action, switch_id);
	append_cmd(action, color);

	send_command(action, 0, &dummy);
	
}

/*
 *  function to check if vehicle is present on certain track(segment)
 *  
 *  @param char* segment_ID (input should be a char integer like "8")
 * 
 */


char get_segment_status(char* segmentID)
{
	// return 0 if not on the segment and 1 if the train is present on the given segment
	char action[5] = "C";
	char input[3];
	char dummy; 
	
	append_cmd(action, segmentID);
	
	// clear the s88 buffer for every 
	send_command("R", 0, &dummy);
	
	// send the commnand to COM1_PORT.
	send_command(action, 3, input);
	
	return input[1];
	
}

/*
 *  function to stop train if it reaches certain segment
 *  
 *  @param char* segment
 * 
 */
void stop_when_on(char* segment)
{
		
	while(1)
	{
	 	if(get_segment_status(segment) == '1')
	 	{
			stop_train();
			break;
		}
			
	}
	
}

// function to solve config 1 or 2 without zamboni 

void config12()
{	
	wprintf(&train_window, "\nStarting config 1 or 2 without Zamboni...\n");
	
	// set switches required for train path
	set_switch("4","R");
	set_switch("3","G");
	set_switch("5","R");
	set_switch("6","R");	
	
	change_speed("5");
	stop_when_on("1");
	change_direction();
	change_speed("5");
	
	stop_when_on("8");
	
}

// train configuration 3 without zamboni 
void config3()
{
	
	wprintf(&train_window, "Starting config 3 without Zamboni...\n");
	// set switches required for train path
	
	set_switch("5","R");
	set_switch("6","G");
	set_switch("7","R");
	set_switch("4","R");
	set_switch("3","R");
	set_switch("8","G");
	
	change_speed("4");
	
	stop_when_on("12");
	change_direction();
	change_speed("4");
	
	stop_when_on("13");
	change_direction();
	change_speed("5");
	
	stop_when_on("5");
}


// train configuration 4 without zamboni 
void config4()
{
	
	wprintf(&train_window, "Starting config 4 without Zamboni...\n");
	// set switches required for train path
	set_switch("5","G");
	set_switch("9","G");
	set_switch("8","G");
	set_switch("3","R");
	set_switch("4","R");
	
	change_speed("4");
	
	while(1)
	{
	 	if(get_segment_status("14") == '1')
	 	{ 
			sleep(1300);
			stop_train(); 
			break; 
		}
	}
	
	change_direction();
	change_speed("5");
	
	stop_when_on("5");
	
}


// configuration 1 with zamboni 

void config1z()
{
	
	wprintf(&train_window, "Starting config 1 with Zamboni...\n");

	// trap zamboni
	set_switch("1","R");
	set_switch("2","R");
	set_switch("7","R");	
	
	while(1)
	{
	 	if(get_segment_status("12") == '1')
	 	{  	
			
			set_switch("6","R");
			set_switch("5","R");
			set_switch("4","R");
			set_switch("3","G"); 
			break; 
		}
	}	
		
	change_speed("4");
	
	stop_when_on("1");
	change_direction();
	change_speed("5");
	
	stop_when_on("8");
	
}


// configuration 1 with zamboni 

void config2z()
{
	wprintf(&train_window, "Starting config 2 with Zamboni...\n");
	
	// trap zamboni
	set_switch("8","R");
	
	while(1)
	{
	 	if(get_segment_status("11") == '1')
	 	{  
			set_switch("5","R");
			set_switch("6","R");
			set_switch("4","R");
			set_switch("3","G");
			break; 
		}
	}	
	
	change_speed("4");
	
	// pick up wagon return
	stop_when_on("1");
	change_direction();
	change_speed("4");
	
	// stop on destination
	stop_when_on("8");
	
}

// function to wait till vehicle reaches a track segment 

void probe_trigger(char *segment)
{
   	while(1)
	  if(get_segment_status(segment) == '1')
	 	break;	
}

// configuration 3z

void config3z()
{
	wprintf(&train_window, "Starting config 3 with Zamboni...\n");
	// set switches required for train path
	set_switch("5","G");
	set_switch("9","R");
	set_switch("1","G");
	
	probe_trigger("6");  
		change_speed("4");
	
	probe_trigger("10");
		set_switch("5","R");
		set_switch("6","G");
			
	
	probe_trigger("9");
		set_switch("5","G");
		
	probe_trigger("12");
		stop_train(); 	
		set_switch("7","R");	
		 
	change_direction();
	
	probe_trigger("14");
		change_speed("4");
			
	probe_trigger("13");
		stop_train(); 
		set_switch("8","R");
			
	change_direction();
	change_speed("4");
	
	probe_trigger("12");
		stop_train(); 
		set_switch("7","G"); 	
	
	change_direction();
	
	probe_trigger("13");
		
			change_speed("4");
			
			set_switch("9","R");
			set_switch("1","R");
			set_switch("2","R");
			set_switch("4","R");
			set_switch("3","R");
			
		
	probe_trigger("9");	
		set_switch("7","R");
			
	stop_when_on("5");

}

void config4z()
{
	
	wprintf(&train_window, "Starting config 4 with Zamboni...\n");
	
	change_speed("4");
	
	set_switch("8","R");
	set_switch("5","R");
	set_switch("6","G");
	
	stop_when_on("9");
	
	probe_trigger("12");
		change_speed("4"); 
	
	set_switch("8","G");
	set_switch("4","G");
	set_switch("9","G");
	
	sleep(1500);
	
	stop_when_on("14");
	
	change_direction();
	change_speed("4");
	sleep(1000);
	stop_train();
	change_direction();
	
	
	probe_trigger("14");
	 change_speed("4");
	 
	probe_trigger("4");
		set_switch("4","R");
		set_switch("3","R"); 
		
	probe_trigger("3");
		set_switch("8","R");
		
	stop_when_on("5");
		
}


// initialize the tracks for default configuration.
void initialize_tracks()
{
	wprintf(&train_status_head, "COMMANDS STATUS \n");
	wprintf(&train_window, "\nClosing outer loop...");
	
	// if zamboni travels clockwise 
	set_switch("5","G");
	set_switch("9","R");
	set_switch("1","G");
	
	// if zamboni travels anti-clockwise
	set_switch("8","G");
	set_switch("4","G");
	
	wprintf(&train_window, "\nClosed outer loop.");
	
}

/* 
 * function to probe the presence of Zamboni and its direction.
 * returns int = 0, 2, 3  
 * 0 means zamboni not found 
 * 2 means zamboni direction is clockwise
 * 3 means zamboni direction is anti-clockwise
 */
 
int find_direction()
{
	// initialize the tracks once the config started
	initialize_tracks();
	
	wprintf(&train_window, "\nLooking For Zamboni.");
	
	int flag = 0;
	int counter=0;
	
	// loop until the the counter detects the track segment 6.
	for(counter = 0; counter < 35; counter++)
	{
		wprintf(&train_window, ".");
	 	if(get_segment_status("6") == '1')
		{ 
			flag = 1; 
			wprintf(&train_window, "\nZamboni Found");
			break; 	
		}
	}
	
	// track detected lets find the direction 
	if(flag)
	{
		wprintf(&train_window, "\nFinding Direction...");
		for(counter = 0; counter < 5; counter++)
		{
			wprintf(&train_window, ".");
				
			if(get_segment_status("7") == '1')
			{ flag = 2; break; }
			
			if(get_segment_status("4") == '1')
			{ flag = 3; break; }
		}
	}
	
	if(flag == 2) 
		wprintf(&train_window, "\nDirection Found: clockwise\n");
	if(flag == 3)
		wprintf(&train_window, "\nDirection Found: anti-clockwise\n");
	
	return flag;
}

// function to find the configuration 
int find_config()
{
	int conf = 0;
	
	// detect vehicle positions 
	if(get_segment_status("5") == '1')
	{
		// config 3 or 4 
		if(get_segment_status("11") == '1')
			return conf = 3;
		
		if(get_segment_status("16") == '1')
			return conf = 4;	
	}
	else 
		if(get_segment_status("8") == '1')
			return conf = 12;
}

// find configuration and execute 
void init_train_configuration()
{	
	
	int direction = find_direction();
	int conf = find_config();
	
	if(direction == 0) // zamboni is not present 
	{
		switch(conf)
		{
		   case 3: 
				config3();
				break; 
		   case 4: 
				config4();
				break;
		   case 12: 
				config12();
				break;
		   default: 
				break;
		   }	
	}
	else              // zamboni is present 
	{
		switch(conf)
		{
		   case 3: 
				config3z();
				break; 
		   case 4: 
				config4z();
				break;
		   case 12: 
				if(direction == 2)       // 2 means clockwise 
					config1z();
				else if(direction == 3)  // 3 means anti-clockwise
					config2z();
				break;
		   default: 
				break;
		}		
	}
	
	wprintf(&train_window, "\nTask Done!");
} 

// main train process
void train_process(PROCESS self, PARAM param)
{	
	init_train_configuration();
	
	remove_ready_queue(active_proc);
	resign();
	
}

void init_train()
{
	
	create_process(train_process, 5, 0, "Train Process");
	
}
