/*******************************************************************************

  @file         lsh.c

  @author       Stephen Brennan, Nicola Bicocchi

*******************************************************************************/

#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#define LSH_RL_BUFSIZE 1024
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"

#define EXEC   100
#define PIPE   101
#define REDIN  102
#define REDOUT 103

/**
  @brief struct describing commands to be executed
*/
struct cmd {
  int type;
  char *left[LSH_TOK_BUFSIZE];
  char *right[LSH_TOK_BUFSIZE];
};

/**
  @brief Builtin functions
*/
int lsh_cd(struct cmd *cmd);
int lsh_help(struct cmd *cmd);
int lsh_exit(struct cmd *cmd);

char *builtin_str[] = {
  "cd",
  "help",
  "exit"
};

int (*builtin_func[]) (struct cmd *) = {
  &lsh_cd,
  &lsh_help,
  &lsh_exit
};

int lsh_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

int lsh_cd(struct cmd *cmd) {
  if (strlen(cmd->left[1]) == 0) {
    printf(2, "lsh: expected argument to \"cd\"\n");
  } else {
    if (chdir(cmd->left[1]) != 0) {
      printf(2, "chdir()!");
    }
  }
  return 1;
}

int lsh_help(struct cmd *cmd) {
  printf(1, "xv6 LSH\n");
  printf(1, "Type program names and arguments, and hit enter.\n");
  printf(1, "The following are built in:\n");

  for (int i = 0; i < lsh_num_builtins(); i++) {
    printf(1, "  %s\n", builtin_str[i]);
  }
  return 1;
}

int lsh_exit(struct cmd *cmd) {
  exit();
}

/**
  @brief Launch a program and wait for it to terminate.
  @param args Null terminated list of arguments (including program).
  @return Always returns 1
 */
int lsh_execute(struct cmd *cmd) {
  int fd, p[2];

  // check for builtins
  for (int i = 0; i < lsh_num_builtins(); i++) {
    if (strcmp(cmd->left[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(cmd);
    }
  }

  switch (cmd->type) {
    case EXEC:
      if (fork() == 0) {
        exec(cmd->left[0], cmd->left);
        printf(2, "lsh: exec failed\n");
        exit();
      }
      wait();
      break;

    case REDIN:
      if ((fd = open(cmd->right[0], O_RDONLY)) < 0) {
        printf(2, "lsh: redirection error\n");
        return 1;
      }
      if (fork() == 0) {
        close(0);
        dup(fd);
        exec(cmd->left[0], cmd->left);
        printf(2, "lsh: exec failed\n");
        exit();
      }
      wait();
      close(fd);
      break;

    case REDOUT:
      if ((fd = open(cmd->right[0], O_WRONLY|O_CREATE)) < 0) {
        printf(2, "lsh: redirection error\n");
        return 1;
      }
      if (fork() == 0) {
        close(1);
        dup(fd);
        exec(cmd->left[0], cmd->left);
        printf(2, "lsh: exec failed\n");
        exit();
      }
      wait();
      close(fd);
      break;

    case PIPE:
      pipe(p);   
      // first child   
      if (fork() == 0) {
        close(1);
        dup(p[1]);
        close(p[0]);
        close(p[1]);
        exec(cmd->left[0], cmd->left);
        printf(2, "lsh: exec failed\n");
        exit();
      }
      // second child
      if(fork() == 0){
        close(0);
        dup(p[0]);
        close(p[0]);
        close(p[1]);
        exec(cmd->right[0], cmd->right);
        printf(2, "lsh: exec failed\n");
        exit();
      }
      close(p[0]);
      close(p[1]);
      wait();
      wait();
      break;
  }
  return 1;
}

/* 
  @brief Count how many of the leading characters there are in "string"
  	 before there's one that's also in "chars".
  @return The return value is the index of the first character in "string"
          that is also in "chars".  If there is no such character, then
          the return value is the length of "string".
*/
int strcspn(char *string, char *chars) {
    register char c, *p, *s;

    for (s = string, c = *s; c != 0; s++, c = *s) {
	for (p = chars; *p != 0; p++) {
	    if (c == *p) {
		return s-string;
	    }
	}
    }
    return s-string;
}

/**
   @brief Split a string up into tokens
   @return If the first argument is non-NULL then a pointer to the
           first token in the string is returned.  Otherwise the
           next token of the previous string is returned.  If there
           are no more tokens, NULL is returned.
*/
char *strtok(char *s, char *delim) {
    static char *lasts;
    register int ch;

    if (s == 0)
	s = lasts;
    do {
	if ((ch = *s++) == '\0')
	    return 0;
    } while (strchr(delim, ch));
    --s;
    lasts = s + strcspn(s, delim);
    if (*lasts != 0)
	*lasts++ = 0;
    return s;
}

/**
   @brief Main entry point.
   @param argc Argument count.
   @param argv Argument vector.
   @return status code
 */
int main(int argc, char **argv) {
  int left_i, right_i;
  char *line;  
  struct cmd *cmd;
  
  line = (char *)malloc(LSH_RL_BUFSIZE * sizeof(char));
  cmd = (struct cmd *)malloc(sizeof(struct cmd));

  do {
    printf(1, "lsh> ");

    // clear buffers and get a new line
    memset(line, 0, LSH_RL_BUFSIZE * sizeof(char));
    memset(cmd, 0, sizeof(struct cmd));
    gets(line, LSH_RL_BUFSIZE);

    // fill struct cmd and splits args between left and right (if symbols >,<,| are detected)
    left_i = right_i = 0;
    cmd->type = EXEC;
    for (char *token = strtok(line, LSH_TOK_DELIM); token != 0; token = strtok(0, LSH_TOK_DELIM)) {
      if (strcmp(token, "<") == 0) {
        cmd->type = REDIN;
        continue;
      }
      if (strcmp(token, ">") == 0) {
        cmd->type = REDOUT;
        continue;
      }
      if (strcmp(token, "|") == 0) {
        cmd->type = PIPE;
        continue;
      }

      if (cmd->type == EXEC) cmd->left[left_i++] = token;
      if (cmd->type != EXEC) cmd->right[right_i++] = token;
    }

    // check for faulty commands
    if (strlen(cmd->left[0]) == 0) {
      printf(2, "lsh: parse error\n");
      continue;
    }
    if ((cmd->type != EXEC) && (strlen(cmd->right[0]) == 0)) {
      printf(2, "lsh: parse error\n");
      continue;
    }

    // execute
    lsh_execute(cmd);    
  } while (1);
}


