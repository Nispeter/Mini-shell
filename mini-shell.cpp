#include "mini-shell.h"

using namespace std;

#define clear() printf("\033[H\033[J") 

//PRIVATE METHODS
void MiniShell::printPromt(){

	char wd[200];
	getcwd(wd,sizeof(wd));								//pwd a lo challa 
	string aux(wd);

	string promtA = "\033[1;33mmini-shell:\033[0m"; 	//string orange en negrita
	string promtB = "\033[1;36m~" + aux + "\033[0m$ ";	//string cyan en negrita
	cout<<promtA<<promtB;
}

void MiniShell::signalManager(){}

bool MiniShell::getCommand(){
	signalManager();
	getline(cin,token);

	if(token == "")
		return false;

	string aux;
	stringstream c(token);
	argn = 0;

	int i = 0;
	while(getline(c,aux, ' ')){
		if(i == 0)
			strcpy(fun,aux.c_str());
		else{							//esto esta re cursed OPTIMIZAR convercion de string a array of pointers of chars
			argn++;
			const char *tempConv = aux.c_str();
			args[i-1] = (char*)malloc(strlen(tempConv)+1);	//no se donde poner el free, kinda sus
			strcpy(args[i-1],tempConv);
		}
		i++;
	}
	args[i-1] = NULL;
	return true;
}

void MiniShell::execCommand(){
	pid_t pid = fork();
	int responsePid;

	if(pid < 0)
		cerr<<"failed to create child"<<endl;
	else if(pid == 0){
		responsePid = execvp(fun,args);
		if(responsePid < 0)
			cerr<<"failed to execute program"<<endl;
	}
	else {
		wait(NULL);
	}
}

void MiniShell::listen(){
	clear();					//limpiar consola
	while(1){
		printPromt();			//imprimir el prompt duh
		if(getCommand()){		//getline y parse
			execCommand();		//fork y exec
			//printLine();		//print for debugging
			//sleep(2);			// ¯\_('-')_/¯
		}
	}
}

//PUBLIC METHODS
MiniShell::MiniShell(){
	listen();
}

void MiniShell::printLine(){
	cerr<<"FUNC: "<<fun<<endl;
	cerr<<"ARGS: ";
	for (int j = 0; j < argn; ++j)
	{
		if(args[j] != "")
			cerr<<"-"<<args[j]<<" ";
	}cerr<<endl;
}


