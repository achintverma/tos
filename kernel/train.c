
#include <kernel.h>


//**************************
//run the train application
//**************************

/*
void append_cmd(char* target, char* tail)
{
	
	int i = 0;
	int target_length = k_strlen(target);
	
	for(i=0; *tail != '\0'; i++)
		*(target+target_length+i) = *tail++;
		
	// determine the length of the target 
	
	
	
}
*/


void print_train(){
	wprintf(&train_window, "\nPavitra App Started");
}

void train_process(PROCESS self, PARAM param)
{	
	
	
	
	//char *first = "Pavitra";
	//char *last  = "Kansara";
	
	//append_cmd(first, last);
	
	print_train();
	
	remove_ready_queue(active_proc);
	resign();
	
}


void init_train()
{
	
	create_process(train_process, 5, 0, "Train Process");
	//train_process();
	
}
