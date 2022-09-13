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
	while(getline(c,aux, ' ')){	//esto esta re cursed OPTIMIZAR convercion de string a array of pointers of chars
		argn++;
		const char *tempConv = aux.c_str();
		inputW[i] = (char*)malloc(strlen(tempConv)+1);	//no se donde poner el free, kinda sus
		strcpy(inputW[i],tempConv);
		i++;
	}

	// MOVER ESTO A EXEC COMAND

	strcpy(fun,inputW[0]);

	if (strcmp(fun, "exit") == 0) {
		exit(0);
	} else if (strcmp(fun, "cd") == 0) {
		// OPCIONAL: Agregar la funcionalidad de cd.
		// (Preferiblemente en una funcion aparte.)
		chdir(inputW[1]);

		return false;
	} 

	return true;
}

/*void MiniShell::execCommand(){
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
}*/

void MiniShell::execCommand(){
	int i = 0, argCount=0;
	bool func = true, piped = false; 
	while(i != argn){
		if(func){
			strcpy(fun,inputW[i]);
			func = false;
		}
		else if(!strcmp(inputW[i],"|")){


		}
		else{
			args[argCount] = inputW[i];
			argCount++;
		}
		i++;
	}
	args[argCount] = NULL;
	if(piped){

	}
	else{
		pid_t pid = fork();
		if(pid < 0)
			cerr<<"failed to create a child"<<endl;
		else if(pid == 0){
			cout<<fun<<endl;
			for (int i = 0; i < argCount; ++i)
			{
				cout<<args[i]<<endl;
			}
			if(execvp(fun,args) < 0){
				if(errno == ENOENT)cerr<<"El comando no existe"<<endl;
				else cerr<<strerror(errno)<<endl;
			}
		}
		else
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


