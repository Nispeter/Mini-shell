#include <iostream>
#include <unistd.h>
#include <string>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

class MiniShell{
	private:
		char fun[100];			
		char* args[20];			
		int argn;			
		string token; 			 

		void printPromt();
		void signalManager();
		void getCommand();
		void execCommand();
		void listen();

	public:
		MiniShell();
		void printLine();

};
