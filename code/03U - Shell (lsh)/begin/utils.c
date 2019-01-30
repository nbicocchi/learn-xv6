#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 
  @brief Count how many of the leading characters there are in "string"
  	 before there's one that's also in "chars".
  @return The return value is the index of the first character in "string"
          that is also in "chars".  If there is no such character, then
          the return value is the length of "string".
 */

int my_strcspn(char *string, char *chars)
{
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
char *my_strtok(char *s, char *delim)
{
    static char *lasts;
    register int ch;

    if (s == 0)
	s = lasts;
    do {
	if ((ch = *s++) == '\0')
	    return 0;
    } while (strchr(delim, ch));
    --s;
    lasts = s + my_strcspn(s, delim);
    if (*lasts != 0)
	*lasts++ = 0;
    return s;
}


int main(int argc, char **argv)
{
  char line[] = "Hello, hello world!";

  for (char *p = my_strtok(line, " "); p != 0; p = my_strtok(0, " ")) {
    printf("%s\n", p);
  }

  exit(0);
}


