/*
* Nome: Rafael Costa de Almeida
* Trabalho: Ghost Shell

* Estrutura:
  -> Utilizei uma TAD com sentinela para guardar todos os grupos de comandos executados
  -> Cada linnha esta em um grupo separado. Para poder criar um novo grupo para cada linha, caso
  o usuario de mais de um comando, quem executa cada comando e um fork(), porem, o processo que lança
  os forks em si, é um fork(). Desta forma, se cria um grupo novo para cada linha
  -> A idéia geral é: na main, inicialmente rodamos uma funcao para iniciar dependencias da shell,
  no caso isso seria os tratadores de sinais para executar o comportamento nao default de Crtl Z e Ctrl C.
  Depois disso, entramos em uma funcao de loop onde esperamos a entrada do usuario, depois separamos essa entrada
  através dos tokens "#" e depois separamos cada substring por " " para separar comando de argumentos, para isso, utiliza-se
  uma variavel char*** para armazenar todo o conteudo da linha separada ja em comandos e argumentos.
  Depois disso, passamos para uma funcao com o objetivo de executar cada comando na variavel char***.
  -> Durante a execucao do comando, através de rand() temos 50% de chance de termos um ghost gerado, independente se
  background ou foreground.
  -> Apos o fork criar outros forks, um para cada comando, seu PID é adicionado ao TAD de linhas criado como variavel global e
  que foi iniciado na init_shell().
  -> Para os comandos especiais mywait, clean&die e os tratamentos de sinais, utiliza-se a TAD para ir de elemento em elemento
  acessando o Group PID ID dos forks de cada linha e lançando o apropriado sinal para cada grupo, efetivamente controlando
  todos os processos associados a session criada.

* OBS: Apenas para clarificar, me atrasei na resolucao pois acabei fazendo sozinho
* OBS1: Coloquei uma mensagem para aparecer quando um ghost e criado, apenas para checar se esta ok o processo
* OBS2: Fiz um repositorio no GitHub para fazer o trabalho, portanto todo o historico de desenvolvimento se esta la
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <signal.h>
#include "lines_struct.c"

#define BUFFERSIZE 1024
#define MAXCOMMANDS 5
#define MAXARGS 3
#define MAXARGSIZE 100
#define LINESNUM 100
pid_t session_ghost;
TListalinha* linhas;


/*
* Funcao para matar todas as linhas executadas 
*/
void send_signal_lines(int signal)
{
  // itera por todas as linhas salvas
  for(TCelulalinha* line=linhas->ini; line!=NULL; line=line->prox)
  {
    // manda sinal para matar todo o grupo que por sua vez mata todos os comandos
    kill(-1*line->linha->process_group_id, signal);
  }
}

/*
* Funcao para matar tudo inclusive sessao
*/
void send_signal_session(int signal)
{
  send_signal_lines(signal);
  Liberalinhas(linhas);
  kill(-1*getpid(), signal);
  exit(EXIT_SUCCESS);
}


/*
  Handle Crtl + C
*/
void CHandler(int sig)
{
  if(Contalinha(linhas)>0) // alguma linha esta rodando
  {
    printf("I have a family pleeease, are you sure you want to exit?\n(y)es or (n)o?\n");
    char buffer[BUFFERSIZE];
    fgets (buffer, BUFFERSIZE, stdin);
    char *option = strtok(buffer, "\n");
    // printf("Option : |%s|\n", option);
    if(strcmp(option,"y")==0 || strcmp(option,"yes")==0)
    {
      printf("If i have to die, i'll take all my family with me!\n");
      send_signal_lines(SIGINT);
      kill(-1*getpid(), SIGKILL);
      exit(EXIT_SUCCESS);
    }
    else
    {
      printf("God bless you!\n");
    }
  }
  else
  {
    printf("I have no family so i have no reason to live anyway!\n");
    kill(-1*getpid(), SIGKILL);
    exit(EXIT_SUCCESS);
  }
}

/*
  Handle Crtl + Z
*/
void ZHandler(int sig)
{
  if(Contalinha(linhas)>0) // alguma linha esta rodando
  {
    printf("I'll not be stopped be the likes of you, you can have my children!!\n");
    send_signal_lines(SIGTSTP);
  }
  else
  {
    printf("If i had children i would tell them to do that buuuuut i prefer dogs\n");
  }
  
  
}

/*
  Faz com que fork ou session leader rode o comando passado
*/
void execute(char **command)
{

  pid_t ghost;
  if (rand() & 1) // creating ghosts
  {
    printf("Wild Ghost Appeared!\n"); // pokemon reference
    ghost = fork();
  }
  // printf("[Child] |%d| |%d| |%d|\n", getpgrp(), getpid(), getppid());
  
  /*
  Se um ghost for criado, colocar ele em outro grupo antes de rodar exec()
  para que ele se separe dos filhos comuns nao ghost
  */
  if(ghost==0)
  {
    // setpgid(0,0);
  }
  if (execvp(command[0], command) == -1)
  {
    perror("Invalid command");
  }
}

/*
  Executa um comando, usa fork(), rand() e exec() para gerar filhos e fantasmas e
  rodar comandos

  if comandos[1]==NULL -> tem apenas um comando na lista de comandos
  else -> tem mais de um, portante passamos por um while que gera forks()
*/
void execute_cmds(char ***comandos)
{

  if(comandos==NULL || comandos[0]==NULL){ // tratando linha vazia
    return;
  }

  int num_cmd = 0;
  char **cmd = comandos[num_cmd];
  // se o proximo ponteiro no array for NULL quer dizer que temos um comando apenas
  if(comandos[1]==NULL) //foreground
  {
    // implementando funcoes da gsh
    if(strcmp(cmd[0], "mywait")==0 || strcmp(cmd[0], "clean&die")==0)
    {
      // liberando todos os processos filhos da bash antes de matar ela
      printf("Killing all descendents...");
      send_signal_lines(SIGKILL);
      printf("Done.\n");
      if (strcmp(cmd[0], "clean&die")==0)
      {
        // caso o comando seja clean&die, matar a shell depois de liberar todos
        printf("I may rest in peace now.\n");
        exit(EXIT_SUCCESS);
      }
    }
    else
    {
     execute(cmd); 
    }
  }
  else //background
  {
    // cria fork para rodar os processos da linha para que seja criado um novo process group associado aos comandos
    pid_t executer = fork();
    int executer_status;
    setpgid(0,0);
    if( executer < 0)
    {
      /* falha do fork */
      exit(EXIT_FAILURE);
    }
    else if ( executer == 0)
    {
      /* codigo do executer*/
      while (cmd != NULL) 
      {

        int status;
        // printf("[Father] |%d| |%d|\n", group_code, getpid());
        pid_t pid = fork ();
        // setting static volatile to process group id so it can kill this line
        if( pid < 0)
        {
          /* falha do fork */
          exit(EXIT_FAILURE);
        }
        else if ( pid == 0)
        {
          /* codigo do  filho*/
          execute(cmd);
        }
        else // pid > 0
        {
          /* codigo do pai */
          waitpid(pid, &status, WNOHANG); // bash lança forks executar e volta para o usuario 
        }
        // printf("%d",num_cmd);
        num_cmd++;
        cmd = comandos[num_cmd];
      }

      // retorna para process leader com o codigo do grupo da linha executada
      exit(getpgrp());
    }
    else // executer > 0
    {
      /* codigo do process leader of session */
      // espera fork executor rodar comandos da linha
      waitpid(executer, &executer_status, 0);
    }
  
  // cria referencia ao grupo da linha
  Tlinha* linha = InicializaTlinha(executer);
  // insete linha ao conjunto de linhas executadas no background
  Inserelinha(linha, linhas);

  }
  return;
}


/*
  Reading functions:
  Recebe linha do user -> corrige espaco em branco e '\n'
*/
char *read_line_shell(void)
{
  char *comando;
  char buffer[BUFFERSIZE];
  fgets (buffer, BUFFERSIZE, stdin);
  if (!strcmp(buffer, "\n") || !strcmp(buffer, "\r\n"))
  {
    comando = "";
  }
  else
  {
    comando = strtok(buffer, "\n");
  }

  // caso nao tenha comando recebido, retorne espaco vazio inves de NULL
  if (comando == NULL)
  {
    comando = "";
  }
  return comando;
}

/*
  Reading functions:
  Divide em tokens -> divide argumentos -> cria char** com todos comandos e argumentos
*/
char ***list_cmds(char *linha_de_comando)
{

  // inicialmente se cria char*** para guardar os ponteiros que representam cada comando e seus argumentos
  char ***comandos_e_argumentos;
  comandos_e_argumentos = malloc(sizeof(char **) * MAXCOMMANDS * MAXARGSIZE);
  int comando_counter = 0;

  // quebra linha entre tokens '#' que representam comandos
  for (char *comando = strtok(linha_de_comando, "#"); comando != NULL; comando = strtok(NULL, "#"))
  {
    // aloca um char** como buffer para manter comando e argumentos
    char **todos_comandos;
    todos_comandos = malloc(sizeof(char *) * MAXCOMMANDS);
    int argument_counter = 0;

    // quebra argumentos em " "
    for (char *argument = strsep(&comando, " "); argument != NULL; argument = strsep(&comando, " "))
    {
      // se o argumento quebrado for realmente um argumento, adiciona ele no char**
      if (strcmp(argument, " ") != 0 && strcmp(argument, "") != 0 && argument_counter<MAXARGS)
      {
        // printf("|%s|", argument);
        todos_comandos[argument_counter] = argument;
        argument_counter++;
      }
    }
    // printf("\n");
    //se existem comandos/argumentos, adiciona-os a lista de comandos
    if(argument_counter!=0 && comando_counter<MAXCOMMANDS)
    {
      // printf("Comando -> |%s| -> ", comando);
      comandos_e_argumentos[comando_counter] = todos_comandos;
      comando_counter++;
    }
  }

  return comandos_e_argumentos;
}


/*
  Loop function:
  Executa loop : printa 'gsh>' -> le input -> parse input ->  executa comandos
*/
void loop_shell()
{
  char *linha;
  char ***todos_comandos;

  while (1)
  {

    printf("gsh>");
    linha = read_line_shell(); // le linha de comandos recebida
    // printf("Recebeu: |%s|\n", linha);
    todos_comandos = list_cmds(linha); // separa cada comando e seus argumentos
    execute_cmds(todos_comandos); // executa comandos
  }
}

/*
  Init shell function:
  Inicia dependencias da shell como tratadores de sinal para Crtl+C e Crtl+Z
*/
void init_shell()
{

  // printf("Welcome\n");

  // Ja setando handlers
  signal(SIGTSTP, ZHandler); // get Crtl Z
  signal(SIGINT, CHandler); // get Crtl C

  // criando sessao para certificar todos process estejam sobre a mesma sessao e definir Group Leader
  session_ghost = setsid();
  linhas = InicializaLista((int)session_ghost);

}

/*
  Main function:
  Roda tudo
*/
int main(int argc, char **argv)
{
  // Inicia dependencias da shell como tratadores de sinal
  init_shell();

  // Roda Loop de execucao
  loop_shell();

  return EXIT_SUCCESS;
}
