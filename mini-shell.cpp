#include "mini-shell.h"

using namespace std;

#define clear_c() printf("\033[H\033[J") 


void childSignalManager(int sigID){
	exit(1);
}

//PRIVATE METHODS
void MiniShell::printPromt(){

	char wd[200];
	getcwd(wd,sizeof(wd));								//pwd a lo challa 
	string aux(wd);

	string promtA = "\033[1;33mmini-shell:\033[0m"; 	//string orange en negrita
	string promtB = "\033[1;36m~" + aux + "\033[0m$ ";	//string cyan en negrita
	cout<<promtA<<promtB;
}

bool MiniShell::getCommand(){
	//signalManager();
	cin.clear();
	fflush(stdin);
	getline(cin,token);
	fflush(stdin);
	cin.clear();

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
		fflush(stdin);
		cin.clear();
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
	} else if (strcmp(fun, "sigur1") == 0){
		int pid = fork();
			
		if(pid == 0){
		pid_t parent = getpid();
		write(STDOUT_FILENO, "Sending custom signal 1\n",25);
		kill(parent, SIGUSR1);
		sleep(2);
		exit(0);
		}
		else {
			wait(NULL);
			return false;
		}
	} else if (strcmp(fun, "sigur2") == 0){
		int pid = fork();
		if(pid == 0){
			pid_t parent = getpid();
			write(STDOUT_FILENO, "Sending custom signal 2\n",25);
			kill(parent, SIGUSR2);
			sleep(2);
			exit(0);
		}
		else {
			wait(NULL);
			return false;
		}
		
	} else if (strcmp(fun, "usorecursos") == 0) {
		if (strcmp(inputW[1], "start") == 0) {
			uso = true;

			string aux(inputW[2]);
			log_filename = aux; 

			ofstream log(log_filename, ios_base::app | ios_base::out);

			log << "comando" << "\t" << "tuser" << "\t" << "tsys" << "\t" << "maxrss" << endl;
			log.close();
		} else if (strcmp(inputW[1], "stop") == 0) {
			uso = false;
		}

		return false;
	}

	return true;
}

void MiniShell::execCommand(){
	int execPid = fork();
	if(execPid < 0)
		write(STDOUT_FILENO,"failed to create a child\n", 26 );
	else if(execPid == 0){
		struct sigaction sa;
		sa.sa_handler = childSignalManager;
		//int temp = sigemptyset(&sa.sa_mask);
		sa.sa_flags = 0;
		sigaction(SIGINT, &sa, NULL);
		int fd[2][2];
		int i = 0, argCount=0,activePipe = 0;
		bool piped = false; 
		while(i != argn){
			if(!strcmp(inputW[i],"|")){//PIPE
				if(piped){
					if(pipe(fd[(activePipe+1)%2]) == -1)cout<<"ERROR EN SEGUND PIPE"<<endl;
					int pidPipe = fork();
					if(pidPipe < 0)
						write(STDOUT_FILENO,"failed to create a child\n", 26 );
					else if(pidPipe == 0){//child
						dup2(fd[activePipe][0], STDIN_FILENO);
						close(fd[activePipe][0]);
						close(fd[activePipe][1]);
						dup2(fd[(activePipe+1)%2][1], STDOUT_FILENO);
						close(fd[(activePipe+1)%2][0]);
						close(fd[(activePipe+1)%2][1]);
						args[argCount] = NULL;
						cout<<"\0";
						if(execvp(args[0],args) < 0){
							if(errno == ENOENT)write(STDOUT_FILENO, "El comando no existe\n",22);
							else cerr<<strerror(errno)<<endl;
							exit(1);
						}
					}
					close(fd[activePipe][1]);
					close(fd[activePipe][0]);
					activePipe = (activePipe+1)%2;
					wait(NULL);
					argCount = 0;
				}
				else {
					if(pipe(fd[activePipe]) == -1)write(STDOUT_FILENO,"Error en el pipe\n", 17);
					int pidPipe = fork();
					if(pidPipe < 0)
						write(STDOUT_FILENO,"failed to create a child\n", 26 );
					if(pidPipe == 0){//child
						dup2(fd[activePipe][1], STDOUT_FILENO);
						close(fd[activePipe][0]);
						close(fd[activePipe][1]);
						args[argCount] = NULL;
						if(execvp(args[0],args) < 0){
							if(errno == ENOENT)write(STDOUT_FILENO, "El comando no existe\n",22);
							else cerr<<strerror(errno)<<endl;
							exit(1);
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
					exit(1);
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
					exit(1);
				}
			}
			else
				wait(NULL);
		}
		exit(1);
	}
	else 
		wait(NULL);
}

void MiniShell::freeMem(){
	for (int i = 0; i < argn; ++i)free(inputW[i]);
}


void MiniShell::listen(){
	clear_c();					//limpiar consola
	while(1){
		printPromt();			//imprimir el prompt duh
		if(getCommand()){
			//continue;		//getline y parse
			if (uso) getrusage(RUSAGE_CHILDREN, &start); 
			execCommand();		//fork y exec
			freeMem();
			if (uso) {
				getrusage(RUSAGE_CHILDREN, &end);
				appendPerf();
			}
		}
	}
}

void MiniShell::appendPerf() {
	ofstream log(log_filename, ios_base::app | ios_base::out);
	string name(fun);
	long tuser = ((end.ru_utime.tv_sec - start.ru_utime.tv_sec) * 1000) + ((end.ru_utime.tv_usec - start.ru_utime.tv_usec) / 1000); 
	long tsys = ((end.ru_stime.tv_sec - start.ru_stime.tv_sec) * 1000) + ((end.ru_stime.tv_usec - start.ru_stime.tv_usec) / 1000); 
	log << name << "\t" << tuser << "ms\t" << tsys << "ms\t" << start.ru_maxrss << endl;
	log.close();
}

void signalManager(int sigID){
	string c;
	switch(sigID){
		case SIGUSR1: 
			write(STDOUT_FILENO, "Custom signal 1 activated\n",27);
			break;
        case SIGUSR2:
            write(STDOUT_FILENO, "Custom signal 2 activated\n",27);
            break;
        case SIGINT:
        	cin.clear();
        	fflush(stdin);
        	write(STDOUT_FILENO, "\nAre you sure you want to close mini-shell? (y/n): ",52);
        	getline(cin,c);
        	cin.clear();
        	fflush(stdin);
        	if(c != "y"){
        		break;
        	}
        	signal(SIGINT, SIG_DFL);
        	write(STDOUT_FILENO, "closing program...\n",20);
            exit(1);
            break;
        default:
            break;
	}
	
}


//PUBLIC METHODS
MiniShell::MiniShell(){
	struct sigaction sa;
	sa.sa_handler = signalManager;
	//int temp = sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
	//solo funciona cuando se sale del mini-shell una vez antes!!!???
	sigaction(SIGUSR1, &sa, NULL);
	sigaction(SIGUSR2, &sa, NULL);


	uso = false;
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


