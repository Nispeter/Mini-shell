#include <iostream>
#include <unistd.h>
#include <string>
#include <sstream>
#include <ios>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>

using namespace std;

class MiniShell{
	private:
		char fun[100];			
		char* inputW[25];
		char* args[20];			
		int argn;			
		string token;
		// Cosas de perf
		string perf;
		string log_filename;
		bool uso;
		struct rusage start, end;			 

		void printPromt();
		void signalManager();
		bool getCommand();
		void execCommand();
		void listen();
		void pipeExec();
		void appendPerf();

	public:
		MiniShell();
		void printLine();

};
