
#include <kernel.h>


#define CARRIGE_RETURN "\015"
#define TRAIN "20"

//**************************
//run the train application
//**************************




void append_cmd(char* target, char* tail)
{
	
	int i = 0;
	int target_length = k_strlen(target);
	
	for(i=0; *tail != '\0'; i++)
		target[target_length+i] = *tail++;
		
	// determine the length of the target 
	
	
	
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
    
    sleep(100);
    
    wprintf(&train_window, "\nfull command: %s", train_cmd);
    
    send(com_port, &msg);
	
	
}

void change_speed(char* speed)
{
	char action[10] = "L";
	char dummy; 
	append_cmd(action, TRAIN);
	append_cmd(action, "S");
	append_cmd(action, speed);

	wprintf(&train_window, "\naction: %s", action);
	
	send_command(action, 0, &dummy);
	
}

void change_direction()
{
	char action[10] = "L";
	char dummy; 
	append_cmd(action, TRAIN);
	append_cmd(action, "D");

	wprintf(&train_window, "\naction: %s", action);
	
	send_command(action, 0, &dummy);
	
}


void stop_train()
{
	change_speed("0");
}

void set_switch(char *switch_id, char *color)
{
	// M4R
	char action[10] = "M";
	char dummy; 
	append_cmd(action, switch_id);
	append_cmd(action, color);

	send_command(action, 0, &dummy);
	
}

int get_segment_status(int segment)
{
	char action[10] = "L";
	char dummy; 
	
	append_cmd(action, "D");
	
	
	// return 0 if not on the segment and 1 if the train is present on the given segment
	
}


void train_process(PROCESS self, PARAM param)
{	
	set_switch("5", "G");
	set_switch("9", "R");
	set_switch("1", "R");
	set_switch("2", "R");
	set_switch("7", "R");
	
	remove_ready_queue(active_proc);
	resign();
	
}


void init_train()
{
	
	create_process(train_process, 5, 0, "Train Process");
	//train_process();
	
}
