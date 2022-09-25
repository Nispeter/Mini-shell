# Mini-shell
Compilar y ejecutar 
  g++ main.cpp mini-shell.cpp -o m
  ./m
Llamadas a funciones específicas
  sigur1: invoca a funcion que crea un hijo que llama al signalHandler de SIGUSR1.
  sigur2: invoca a funcion que crea un hijo que llama al signalHandler de SIGUSR2.
  usorecursos start [nombre de archivo]: Inicia el registro de perf.
  usorecursos stop: Termina el registro de perf.
  ^C: Control C reconoce SIGINT 
  Se puede usar cualquier funcion de la shell con sus respectivos argumentos invocandola con el comando correspondiente.
  Se puede usar una cantidad indefinida de pipes usando | para redefinir 
