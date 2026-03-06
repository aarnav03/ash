#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define EXIT_SUCCESS 0

#define RL_BUFSIZE 1024
#define TOK_SIZE 64
#define TOK_DELI "\t\r\n\a"

char *read_line(void) {
  int bufs = RL_BUFSIZE;
  int pos = 0;
  char *buf = malloc(sizeof(char) * bufs);
  int ch;

  if (!buf)
    fprintf(stderr, "allocation error \n");
  exit(EXIT_FAILURE);

  while (1) {
    ch = getchar();

    if (ch == EOF || ch == '\n') {
      buf[pos] = '\0';
      return buf;
    } else
      buf[pos] = ch;
    pos++;
  }
  if (pos >= bufs) {
    bufs += RL_BUFSIZE;
    buf = realloc(buf, bufs);
    if (!buf) {
      fprintf(stderr, "realloc error \n");
      exit(EXIT_FAILURE);
    }
  }
}

char *tokenize(char *line) {

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

    if (pos > bufs) {
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
  return token;
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

int main() {
  /*   char *l = "hellooo does this stuff work??";
    tokenize(l); */
  return EXIT_SUCCESS;
}
