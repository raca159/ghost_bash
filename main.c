#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#define BUFFERSIZE 1024
#define ARGSIZE 5
typedef struct session_info SessInfo;

struct session_info
{
  char *user;
};

char *read_line_shell(void)
{
  char *text = calloc(1,1), buffer[BUFFERSIZE];
  fgets(buffer, BUFFERSIZE , stdin);
  char *comando = strtok(buffer, "\n");
  return comando;
}

// char **split_line_shell(char *linha_de_comando)
void split_line_shell(char *linha_de_comando)
{
  char *comando = strtok(linha_de_comando, " # ");
  char **todos_comandos;
  int i = 0;
  while(comando!=NULL)
  {
    printf("|%s|", comando);
    comando = strtok(NULL, " # ");
    i++;
  }
}

void loop_shell(SessInfo* info)
{
  char *line;
  char **args;
  int status = 1;
  char *username = info->user;

  do {
    printf("%s@gsh>", username);
    line = read_line_shell();
    printf("\nrecebeu : |%s|\n", line);
    split_line_shell(line);
    // status = lsh_execute(args);
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
