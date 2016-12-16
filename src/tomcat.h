/*

Thomas Daley
11/13/2016
tomcat.h

*/

#ifndef TOMCAT
#define TOMCAT

#include <windows.h>
#include <stdio.h>
#include "AsyncStreamReader.h"
#include "pipes.h"

class TomCat {
	
	private:
		AsyncStreamReader *input;
		FILE *output;
		struct PipeHandles pipes;
		int launched;
		char *filename;

		int Launch();
		

	public:
		TomCat(char *filename);
		~TomCat();
		int Process(SOCKET ClientSocket);
		
};

#endif
