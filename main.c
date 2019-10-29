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
static volatile int process_counter = 0;

void CHandler(int sig) {
  if(process_counter==0)
  {
    printf("I have nothing to live for anyway, no child, no nothing.\n");
    exit(EXIT_SUCCESS);
  }
  else
  {
  printf("I have children pleeease, are you sure you want to exit?\n(y)es or (n)o?\n");
  char buffer[BUFFERSIZE];
  fgets (buffer, BUFFERSIZE, stdin);
  char *option = strtok(buffer, "\n");
  // printf("Option : |%s|\n", option);
  if(strcmp(option,"y")==0 || strcmp(option,"yes")==0){
    //exit(SIGTSTP);
    exit(EXIT_SUCCESS);
    }
  }

}


void ZHandler(int sig) {
  printf("Suspending everything.\n");
}


void execute(char **command)
{
  pid_t pid, wpid;
  int status;

  pid = fork () ;
  if( pid < 0)
  {
    /* falha do fork */
    exit(EXIT_FAILURE);
  }
  else if ( pid == 0)
  {
    /* codigo do  filho*/
    if (rand() & 1) // creating ghosts
    {
      printf("Wild Ghost Appeared!\n"); // pokemon reference
      fork();
    }
    process_counter++;
    if (execvp(command[0], command) == -1)
    {
      perror("Invalid command");
    }
  }
  else // pid > 0
  {
    /* codigo do pai */
    // do
    // {
    //   wpid = waitpid(pid, &status, WUNTRACED);
    // } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    // wait(NULL);
    process_counter++;
  }
  return;
}

void execute_cmds(char ***comandos)
{
  int num_cmd = 0;
  char **cmd = comandos[num_cmd];
  if (comandos[1] == NULL) // foreground
  {
    if (cmd == NULL || cmd[0] == NULL) // empty line
    {
      return;
    }
    else
    {
      if (execvp(cmd[0], cmd) == -1)
      {
        perror("Invalid command");
        return;
      }
    }
  }
  else // background
  {
    while (cmd != NULL)
    {
      execute(cmd);
      // printf("%d",num_cmd);
      num_cmd++;
      cmd = comandos[num_cmd];
    }
  }
}

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
  } while (counter <25);
}


void init_shell()
{

  // printf("Welcome\n");
  signal(SIGTSTP, ZHandler); // get Crtl Z
  signal(SIGINT, CHandler); // get Crtl C

}

int main(int argc, char **argv)
{
  // Initialize shell dependencies
  init_shell();

  // Run command loop.
  loop_shell();

  return EXIT_SUCCESS;
}
