#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define EXIT_SUCCESS 0

#define RL_BUFSIZE 1024
#define TOK_SIZE 64
#define TOK_DELI " \t\r\n\a"

int sh_cd(char **args);
int sh_exit(char **args);
int sh_help(char **args);

char *sh_str[] = {"cd", "exit", "help"};

int (*sh_fn[])(char **) = {&sh_cd, &sh_exit, &sh_help};

int num_shc() { return (sizeof(sh_str) / sizeof(char *)); }

char *read_line(void) {
  int bufs = RL_BUFSIZE;
  int pos = 0;
  char *buf = malloc(sizeof(char) * bufs);
  int ch;

  if (!buf) {
    fprintf(stderr, "allocation error \n");
    exit(EXIT_FAILURE);
  }
  while (1) {
    ch = getchar();

    if (ch == EOF || ch == '\n') {
      buf[pos] = '\0';
      return buf;
    } else
      buf[pos] = ch;
    pos++;

    if (pos >= bufs) {
      bufs += RL_BUFSIZE;
      buf = realloc(buf, bufs);
      if (!buf) {
        fprintf(stderr, "realloc error \n");
        exit(EXIT_FAILURE);
      }
    }
  }
}

char **tokenize(char *line) {

  int bufs = TOK_SIZE;
  int pos = 0;
  char **tokp = malloc(bufs * sizeof(char *));
  char *token;
  if (!tokp) {
    fprintf(stderr, "tokp allocation error");
    exit(EXIT_FAILURE);
  }
  token = strtok(line, TOK_DELI);
  while (token != NULL) {
    tokp[pos] = token;
    pos++;
    /*     printf("%p", tokp[pos]);
        printf("%s", token); */

    if (pos >= bufs) {
      bufs += TOK_SIZE;
      tokp = realloc(tokp, bufs * sizeof(char *));
      if (!tokp) {
        fprintf(stderr, "tokp realloc error");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, TOK_DELI);
    /*   printf("%p", tokp[pos - 2]);
      printf("%s", token); */
  }
  tokp[pos] = NULL;
  return tokp;
}

int launch(char **args) {
  pid_t pid, wpid;
  int status;

  pid = fork();
  if (pid == 0) {
    if (execvp(args[0], args) == -1) {
      perror("ash");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // fork error
    perror("ash");
  } else {
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }
  return 1;
}

int sh_cd(char **args) {
  if (args[1] == NULL)
    fprintf(stderr, "ash: cd missing args");
  else {
    if (chdir(args[1]) != 0)
      perror("ash");
  }
  return 1;
}

int sh_exit(char **args) { return 0; }

int sh_help(char **args) {
  int i;
  printf("ash aka aarnav shell [use at ur own risk] \n");
  printf("inbuilt functions include \n");
  for (i = 0; i < num_shc(); i++)
    printf("%s \n", sh_str[i]);
  return 1;
}

int execute(char **args) {
  int i = 0;

  if (args[0] == NULL)
    return 1;

  for (i = 0; i < num_shc(); i++) {
    if (strcmp(args[0], sh_str[i]) == 0)
      return (*sh_fn[i])(args);
  }
  return launch(args);
}

void loop(void) {
  char *line;
  char **args;
  int status;

  do {
    printf("> ");
    line = read_line();
    args = tokenize(line);
    status = execute(args);

    free(line);
    free(args);
  } while (status);
}

int main(int argc, char **argv) {
  /*   char *l = "hellooo does this stuff work??";
    tokenize(l); */
  loop();
  return EXIT_SUCCESS;
}
