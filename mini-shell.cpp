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

void MiniShell::pipeExec(){
		
}


void MiniShell::execCommand(){
	int fd[2][2];
	int i = 0, argCount=0,activePipe = 0;
	bool piped = false; 
	while(i != argn){
		if(!strcmp(inputW[i],"|")){//PIPE
			if(piped){
				if(pipe(fd[(activePipe+1)%2]) == -1)cout<<"ERROR EN SEGUND PIPE"<<endl;
				int pidPipe = fork();
				if(pidPipe < 0)cout<<"ERROR EN FORK"<<endl;
				if(pidPipe == 0){//child
					dup2(fd[activePipe][0], STDIN_FILENO);
					close(fd[activePipe][0]);
					close(fd[activePipe][1]);
					dup2(fd[(activePipe+1)%2][1], STDOUT_FILENO);
					close(fd[(activePipe+1)%2][0]);
					close(fd[(activePipe+1)%2][1]);
					args[argCount] = NULL;
					cout<<"\0";
					if(execvp(args[0],args) < 0){
						if(errno == ENOENT)cerr<<"El comando no existe"<<endl;
						else cerr<<strerror(errno)<<endl;
					}
				}
				close(fd[activePipe][1]);
				close(fd[activePipe][0]);
				activePipe = (activePipe+1)%2;
				wait(NULL);
				argCount = 0;
			}
			else {
				if(pipe(fd[activePipe]) == -1)return;
				int pidPipe = fork();
				if(pidPipe < 0)return;
				if(pidPipe == 0){//child
					dup2(fd[activePipe][1], STDOUT_FILENO);
					close(fd[activePipe][0]);
					close(fd[activePipe][1]);
					args[argCount] = NULL;
					if(execvp(args[0],args) < 0){
						if(errno == ENOENT)cerr<<"El comando no existe"<<endl;
						else cerr<<strerror(errno)<<endl;
					}
				}
				wait(NULL);
				argCount = 0;
				piped = true;

			}

		}
		else{//SE AGREG ARGUMENTO
			args[argCount] = inputW[i];
			argCount++;
		}
		i++;
	}
	//ULTIMO COMANDO A EJECUTAR
	args[argCount] = NULL;
	if(piped){
		pid_t pid = fork();
		if(pid < 0)
			cerr<<"failed to create a child"<<endl;
		else if(pid == 0){
			dup2(fd[activePipe][0], STDIN_FILENO);
			close(fd[activePipe][0]);
			close(fd[activePipe][1]);
			if(execvp(args[0],args) < 0){
				if(errno == ENOENT)cerr<<"El comando no existe"<<endl;
				else cerr<<strerror(errno)<<endl;
			}
		}
		else{
			close(fd[activePipe][1]);
			close(fd[activePipe][0]);
			wait(NULL);
		}
	}
	else{
		pid_t pid = fork();
		if(pid < 0)
			cerr<<"failed to create a child"<<endl;
		else if(pid == 0){
			if(execvp(args[0],args) < 0){
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


