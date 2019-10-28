#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <signal.h>

#define BUFFERSIZE 1024
#define MAXARGS 5
#define MAXARGSIZE 100
typedef struct session_info SessInfo;
typedef struct commands_pack CommandsGroup;
static volatile int keepRunning = 1;

void init_shell()
{

  printf("Welcome\n");

}

int execute(char **command)
{
  printf("\n#####\n");

  pid_t pid, duplicated, wpid;
  int status;
  pid_t father = getpid();

  if (father == getpid())
  {
    pid = fork();
  }
  if (father == getpid())
  {
    if (rand() & 1) // creating ghosts
    {
      printf("Wild Ghost Appeared!\n"); // pokemon reference
      duplicated = fork();
    }
  }
  if (pid == getpid() || duplicated == getpid()) // Child process
  {
    printf("Filho/Ghost |%d| |%d| \n", pid, duplicated);
    if (execvp(command[0], command) == -1)
    {
      perror("Invalid command");
      exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
  }
  else // Parent process
  {
    printf("Pai |%d|\n", father);
    do
    {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return EXIT_SUCCESS;
}

int execute_cmds(char ***comandos)
{
  int num_cmd = 0;
  char **cmd = comandos[num_cmd];
  if (comandos[1] == NULL) // foreground
  {
    if (cmd == NULL || cmd[0] == NULL) // empty line
    {
      return EXIT_SUCCESS;
    }
    else
    {
      if (execvp(cmd[0], cmd) == -1)
      {
        perror("Invalid command");
        exit(EXIT_FAILURE);
      }
    }
  }
  else // background
  {
    while (cmd != NULL)
    {
      execute(cmd);
      num_cmd++;
      cmd = comandos[num_cmd];
    }
  }
  return EXIT_SUCCESS;
}

char *read_line_shell(void)
{
  char *text = calloc(1, 1), buffer[BUFFERSIZE];
  char *comando;
  fgets(buffer, BUFFERSIZE, stdin);
  if (!strcmp(buffer, "\n") || !strcmp(buffer, "\r\n"))
  {
    comando = "";
  }
  else
  {
    comando = strtok(buffer, "\n");
  }
  if (comando == NULL)
  {
    comando = "";
  }
  return comando;
}

char ***list_cmds(char *linha_de_comando)
{
  char ***comandos_e_argumentos;
  comandos_e_argumentos = malloc(sizeof(char **) * MAXARGS * MAXARGSIZE);
  int comando_counter = 0;
  for (char *comando = strtok(linha_de_comando, "#"); comando != NULL; comando = strtok(NULL, "#"))
  {
    // printf("Comando -> |%s| -> ", comando);
    char **todos_comandos;
    todos_comandos = malloc(sizeof(char *) * MAXARGS);
    int argument_counter = 0;
    for (char *argument = strsep(&comando, " "); argument != NULL; argument = strsep(&comando, " "))
    {
      if (strcmp(argument, " ") != 0 && strcmp(argument, "") != 0)
      {
        // printf("|%s|", argument);
        todos_comandos[argument_counter] = argument;
        argument_counter++;
      }
    }
    // printf("\n");
    comandos_e_argumentos[comando_counter] = todos_comandos;
    comando_counter++;
  }

  return comandos_e_argumentos;
}

void loop_shell()
{
  char *linha;
  char ***todos_comandos;
  int status = EXIT_SUCCESS;

  int counter = 0;
  do
  {
    printf("gsh>");
    linha = read_line_shell();
    // printf("Recebeu: |%s|\n", linha);
    todos_comandos = list_cmds(linha);
    execute_cmds(todos_comandos);
    counter++;
  } while (status == EXIT_SUCCESS && counter <10);
}


void CHandler(int sig) {
  printf("Do you want to exit?\n(y)es or (n)o?\n");
  char option[10];
  scanf("%s", option);
  if(strcmp(option,"y")==0 || strcmp(option,"yes")==0){
    keepRunning = 0;
    exit(SIGTSTP);
  }
}


int main(int argc, char **argv)
{
  init_shell();
  signal(SIGTSTP, SIG_IGN); // get Crtl Z
  signal(SIGINT, CHandler); // get Crtl C

  // Run command loop.
  loop_shell();

  // Perform any shutdown/cleanup.

  return EXIT_SUCCESS;
}
