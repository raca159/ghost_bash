#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFFERSIZE 1024
#define MAXARGS 5
#define MAXARGSIZE 100
typedef struct session_info SessInfo;

struct session_info
{
  char *user;
};

int execute(char **command)
{
  pid_t pid, wpid;
  int status;

  pid = fork();
  if (pid == 0) {
    // Child process
    if (execvp(command[0], command) == -1) {
      perror("lsh");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    perror("lsh");
  } else {
    // Parent process
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

void execute_cmds(char ***comandos)
{
  int num_cmd = 0;
  char **cmd = comandos[num_cmd];
  while (cmd!=NULL)
  {
    execute(cmd);
    num_cmd++;
    cmd = comandos[num_cmd];
  }
  
}

char *read_line_shell(void)
{
  char *text = calloc(1,1), buffer[BUFFERSIZE];
  fgets(buffer, BUFFERSIZE , stdin);
  char *comando = strtok(buffer, "\n");
  if(comando == NULL){
    comando = "";
  }
  return comando;
}

char ***list_cmds(char *linha_de_comando)
{
  char ***comandos_e_argumentos;
  comandos_e_argumentos = malloc(sizeof(char**)*MAXARGS*MAXARGSIZE);
  int comando_counter = 0;
  for(char *comando = strtok(linha_de_comando, "#"); comando!=NULL; comando = strtok(NULL, "#"))
  {
    // printf("Comando -> |%s| -> ", comando);
    char **todos_comandos;
    todos_comandos = malloc(sizeof(char*)*MAXARGS);
    int argument_counter = 0;
    for(char *argument = strsep(&comando, " "); argument!=NULL; argument = strsep(&comando, " "))
    {
      if(strcmp(argument," ")!=0 && strcmp(argument,"")!=0)
      {
      // printf("|%s|\n", argument);
      todos_comandos[argument_counter] = argument;
      argument_counter++;
      }
    }
    comandos_e_argumentos[comando_counter] = todos_comandos;
    comando_counter++;
  }

  return comandos_e_argumentos;
}

void loop_shell(SessInfo* info)
{
  char *linha;
  char ***todos_comandos;
  int status = 1;
  char *username = info->user;

  do {
    printf("%s@gsh>", username);
    linha = read_line_shell();
    // printf("Recebeu: |%s|\n", linha);
    todos_comandos = list_cmds(linha);
    execute_cmds(todos_comandos);
  } while (status);
}

SessInfo* init_shell() 
{ 
    char* username = getenv("USER");
    SessInfo* sess = (SessInfo*)malloc(sizeof(SessInfo));
    sess->user = (char*)malloc((strlen(username)+1) * sizeof(char));
    strcpy(sess->user, username);
    

    return sess;

} 

int main(int argc, char **argv)
{
  
  SessInfo* sess = init_shell();

  // Run command loop.
  loop_shell(sess);

  // Perform any shutdown/cleanup.

  return EXIT_SUCCESS;
}
