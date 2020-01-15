#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#define NULL 0

char* read_file(char *name, uint *num) ; 
void disasm(unsigned char *buffer, uint num) ;
char *rm8_r8(char *buffer, int *j, int *err) ;
char *rm(char *buffer, int *j, char type, int *error) ;
char *rm16_r16(char *buffer, int *j, int *err) ;
char *r8_rm8(char *buffer, int *j, int *err) ;
char *r16_rm16(char *buffer, int *j, int *err) ;
char *imm8(char *buffer, int *j, int *err) ;
char *imm16(char *buffer, int *j, int *err) ;
char *rel8(char *buufer, int *j, int *err) ;
char *rm8_imm8(char *buffer, int *j, int *err) ;
char *rm16_imm16(char *buffer, int *j, int *err) ;
char *rm16_imm8(char *buffer, int *j, int *err) ;
char *sreg_rm16(char *buffer, int *j, int *error) ;
char *rm16_sreg(char *buffer, int *j, int *error) ;
char *m16(char *buffer, int *j, int *err) ;
char *call_inter(char *buffer, int *j, int *err) ;
char *moffs16(char *buffer, int *j, int *err) ;
char *rm8(char *buffer, int *j, int *err) ;
char *rm16(char *buffer, int *j, int *err) ;
char *rel16(char *buffer, int *j, int *err) ;
int parse(char *s, char*(*func)(char*, int*, int*),unsigned char *buffer, int *j) ;
int parse_noop(char *s, unsigned char *buffer, int *j) ;
int get_bytes(int k, int j) ;
