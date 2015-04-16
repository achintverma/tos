
#include <kernel.h>


void poke_b (MEM_ADDR addr, BYTE value)
{
	// create a byte pointer 
	BYTE *b = (BYTE*)addr;
	
	// put the value to the address 
	*b = value;
}

void poke_w (MEM_ADDR addr, WORD value)
{
	// create a byte pointer 
	WORD *w = (WORD*)addr;
	
	// put the value to the address 
	*w = value;
	
}

void poke_l (MEM_ADDR addr, LONG value)
{
	
	// create a byte pointer 
	LONG *l = (LONG*)addr;
	
	// put the value to the address 
	*l = value;
}

BYTE peek_b (MEM_ADDR addr)
{
	// create the byte pointer and typecast the adddress
	BYTE *b = (BYTE*)addr;
	return *b;
}

WORD peek_w (MEM_ADDR addr)
{
	// create the word pointer and typecast the address
	WORD *w = (WORD*)addr;
	return *w;
}

LONG peek_l (MEM_ADDR addr)
{
	// create the word pointer and typecast the address
	LONG *l = (LONG*)addr;
	return *l;
}


