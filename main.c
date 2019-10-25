#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>

#define BUFFERSIZE 1024
#define MAXARGS 5
#define MAXARGSIZE 100
typedef struct session_info SessInfo;
typedef struct commands_pack CommandsGroup;

struct commands_pack
{
  char ***commands;      // lista de comandos
  pid_t **real_pids;     // pid dos comandos da linha
  pid_t **ethereal_pids; // pid dos fantasmas da linha
};

struct session_info
{
  char *user;
  char *computer;
  CommandsGroup *lines_received;
};

SessInfo *init_shell()
{
  char *username = getenv("USER");
  char *computer = getenv("NAME");
  SessInfo *sess = (SessInfo *)malloc(sizeof(SessInfo));
  sess->user = (char *)malloc((strlen(username) + 1) * sizeof(char));
  sess->computer = (char *)malloc((strlen(computer) + 1) * sizeof(char));
  strcpy(sess->user, username);
  strcpy(sess->computer, computer);
  sess->lines_received = NULL;

  return sess;
}

CommandsGroup *init_group()
{
  CommandsGroup *line = (CommandsGroup *)malloc(sizeof(CommandsGroup));
  line->commands = NULL;
  line->ethereal_pids = NULL;
  line->real_pids = NULL;

  return line;
}

int execute(char **command)
{

  pid_t pid, duplicated, wpid;
  int status;

  pid = fork();
  if (pid == 0)
  {
    if (rand() & 1)
    { // creating ghosts
      printf("Duplicate\n");
      duplicated = fork();
    }
    // Child process
    if (execvp(command[0], command) == -1)
    {
      perror("Invalid command");
      exit(EXIT_FAILURE);
    }
  }
  else if (pid < 0)
  {
    // Error forking
    perror("Something wrong with the children");
  }
  else
  {
    // Parent process
    do
    {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }
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

void loop_shell(SessInfo *info)
{
  char *linha;
  char ***todos_comandos;
  char *username = info->user;
  char *computer = info->computer;
  int status = EXIT_SUCCESS;

  do
  {
    printf("%s@%s:gsh>", username, computer);
    linha = read_line_shell();
    // printf("Recebeu: |%s|\n", linha);
    todos_comandos = list_cmds(linha);
    execute_cmds(todos_comandos);
  } while (status == EXIT_SUCCESS);
}

int main(int argc, char **argv)
{

  SessInfo *sess = init_shell();

  // Run command loop.
  loop_shell(sess);

  // Perform any shutdown/cleanup.

  return EXIT_SUCCESS;
}
