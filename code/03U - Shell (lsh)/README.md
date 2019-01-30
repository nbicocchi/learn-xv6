## Operating Systems Course - DIEF UNIMORE ##

In this exercise, we will port a simple shell written for linux to xv6.
The folder begin/ contains the Linux version of lsh.c. On Ubuntu, you can compile and run it:

```
$ cd begin/
$ gcc lsh.c -o lsh
$ ./lsh
```

In the first part, we will make lsh.c compile and run on xv6. 
The most of the errors showed by the compiler are originated by missing definitions, 
and/or functions with different names. Please refer to (user.h) for system calls and 
library functions available on xv6. The most difficult part is on string manipulation 
because xv6 do not offer many library fuctions for that.
Please refer to begin/utils.c for the implementation of the missing strtok() function.

In the second part, we will extend lsh in order to understand simple redirection 
and piping (symbols < > |). Only the following cases (one symbol per line) have 
to be addressed:

* $ cat < README
* $ echo ciao > README
* $ cat README | wc

To deal with commands comprising two parts (left and right of >,<,|) we can use 
a structure like the one showed below.

```
struct cmd {
  int type;
  // supported types: EXEC, REDIN, REDOUT, PIPE
  char *left[LSH_TOK_BUFSIZE];
  char *right[LSH_TOK_BUFSIZE];
};
```

As a consequence the main function used to tokenize the input lines (showed below)
must correctly fill an instance of struct cmd.

```
for (char *token = strtok(line, LSH_TOK_DELIM); 
	token != 0; 
	token = strtok(0, LSH_TOK_DELIM)) {
	...
    }
```

Finally, function lsh_execute must be modified accordingly. To correcly handle the 
three new cases (REDIN, REDOUT, PIPE), please remind how to use fork(), exec() 
and dup() systems calls. They are all available on xv6!

```
int lsh_execute(struct cmd *cmd) {
  ...

  switch (cmd->type) {
    case EXEC:
      if (fork() == 0) {
        exec(cmd->left[0], cmd->left);
      }
      wait();
      break;

    case REDIN:
	...
      break;

    case REDOUT:
	...
      break;

    case PIPE:
	...
      break;
  }
  return 1;
}
```
It is also requested to lauch the lsh shell at boot (init.c).

Good work!
