#include <stdio.h> 
#include <stdlib.h> 
# include <string.h>

#define LSH_RL_BUFSIZE 1024
// #define EXIT_FAILURE -1
// #define EXIT_SUCCESS 0

char *read_line_shell(void)
{
  int bufsize = LSH_RL_BUFSIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;

  while (1) {
    // Read a character
    c = getchar();

    // If we hit EOF, replace it with a null character and return.
    if (c == EOF || c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = c;
    }
    position++;
  }
}

void loop_shell(void)
{
  char *line;
  char **args;
  int status = 1;
  char* username = getenv("USER");

  do {
    printf("%s@gsh> ", username);
    line = read_line_shell();
    printf("%s", line);
    // args = lsh_split_line(line);
    // status = lsh_execute(args);

    free(line);
    free(args);
  } while (status);
}

void init_shell() 
{ 
    // asd
} 

int main(int argc, char **argv)
{
  // Load config files, if any.

  // Run command loop.
  loop_shell();

  // Perform any shutdown/cleanup.

  return EXIT_SUCCESS;
}
