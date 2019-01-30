#include "types.h"
#include "user.h"
#include "stat.h"
#include "fcntl.h"

#define MAXLEN 64
#define MAXBUF 2048
#define FILENAME_PASSWD "shadow"

int checkpasswd(char *filename, char *user, char *passwd) {
  int fd, i, n, c = 0, l = 0;
  char ipasswd[MAXLEN];
  char iuser[MAXLEN];
  char buf[MAXBUF];

  if((fd = open(filename, O_RDONLY)) < 0) {
    printf(1, "login: cannot open %s\n", filename);
    return 1;
  }

  while((n = read(fd, buf, sizeof(buf))) > 0) {
    for(i = 0; i < n;) {

      while(i < n && buf[i] != ':' ) iuser[c++] = buf[i++];
      if(i == n) break; 
      iuser[c] = '\0';
      i++;

      while(i < n && buf[i] != ':') ipasswd[l++] = buf[i++];
      if(i == n) break;
      ipasswd[l] = '\0';
      
      c = l = 0;

      if(!strcmp(user, iuser) && !strcmp(passwd, ipasswd)) {
        close(fd);
        return 1;
      }

      while(i < n && buf[i++] != '\n');
    }
  }
  close(fd);
  return 0; 
}

int main(void){
    char username[MAXLEN];
    char password[MAXLEN];

    printf(1, "\nxv6 login\n");
    printf(1, "Username: ");
    gets(username, MAXLEN);

    printf(1, "Password: ");
    gets(password, MAXLEN);

    // remove \n
    if(username[strlen(username) - 1]  == '\n'){
      username[strlen(username) - 1]  = 0;	
    }

    if(password[strlen(password) - 1]  == '\n'){
      password[strlen(password) - 1]  = 0;	
    }

    if(checkpasswd(FILENAME_PASSWD, username, password)){
      char home_dir[MAXLEN];
      strcpy(home_dir, "/home/");      
      mkdir(home_dir);      
      strcpy(home_dir + strlen(home_dir), username);
      mkdir(home_dir);

      printf(1,"\nWelcome %s!\n", username);

      char *args[] = {"sh", 0};
      exec(args[0], args);
      printf(1, "login: exec sh failed\n");
      exit(); 
    } 
      
    printf(1, "login: wrong username or password\n");
    exit();
}

