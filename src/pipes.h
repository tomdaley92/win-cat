/*

Thomas Daley
10/26/2016
pipe.h

*/

#ifndef PIPES
#define PIPES

#include <stdio.h>

typedef struct PipeHandles {
	HANDLE Child_Std_IN_Rd = NULL;
	HANDLE Child_Std_IN_Wr = NULL;
	HANDLE Child_Std_OUT_Rd = NULL;
	HANDLE Child_Std_OUT_Wr = NULL;
	PROCESS_INFORMATION piProcInfo;
} PipeHandles, *pPipeHandles;

PipeHandles get_pipes(char *filename);
int close_pipes(struct PipeHandles pipes);

#endif
