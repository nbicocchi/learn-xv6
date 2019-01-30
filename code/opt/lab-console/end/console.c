// Console input and output.
// Input is from the keyboard or serial port.
// Output is written to the screen and serial port.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "traps.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"

static void consputc(int);

static int panicked = 0;

static struct {
  struct spinlock lock;
  int locking;
} cons;

static void
printint(int xx, int base, int sign)
{
  static char digits[] = "0123456789abcdef";
  char buf[16];
  int i;
  uint x;

  if(sign && (sign = xx < 0))
    x = -xx;
  else
    x = xx;

  i = 0;
  do{
    buf[i++] = digits[x % base];
  }while((x /= base) != 0);

  if(sign)
    buf[i++] = '-';

  while(--i >= 0)
    consputc(buf[i]);
}
//PAGEBREAK: 50

// Print to the console. only understands %d, %x, %p, %s.
void
cprintf(char *fmt, ...)
{
  int i, c, locking;
  uint *argp;
  char *s;

  locking = cons.locking;
  if(locking)
    acquire(&cons.lock);

  if (fmt == 0)
    panic("null fmt");

  argp = (uint*)(void*)(&fmt + 1);
  for(i = 0; (c = fmt[i] & 0xff) != 0; i++){
    if(c != '%'){
      consputc(c);
      continue;
    }
    c = fmt[++i] & 0xff;
    if(c == 0)
      break;
    switch(c){
    case 'd':
      printint(*argp++, 10, 1);
      break;
    case 'x':
    case 'p':
      printint(*argp++, 16, 0);
      break;
    case 's':
      if((s = (char*)*argp++) == 0)
        s = "(null)";
      for(; *s; s++)
        consputc(*s);
      break;
    case '%':
      consputc('%');
      break;
    default:
      // Print unknown % sequence to draw attention.
      consputc('%');
      consputc(c);
      break;
    }
  }

  if(locking)
    release(&cons.lock);
}

void
panic(char *s)
{
  int i;
  uint pcs[10];

  cli();
  cons.locking = 0;
  // use lapiccpunum so that we can call panic from mycpu()
  cprintf("lapicid %d: panic: ", lapicid());
  cprintf(s);
  cprintf("\n");
  getcallerpcs(&s, pcs);
  for(i=0; i<10; i++)
    cprintf(" %p", pcs[i]);
  panicked = 1; // freeze other CPU
  for(;;)
    ;
}

//PAGEBREAK: 50
#define BACKSPACE 0x100
#define CRTPORT 0x3d4
#define TAB_LENGHT 4
static ushort *crt = (ushort*)P2V(0xb8000);  // CGA memory

static void
cgaputc(int c)
{
  int pos;

  // Cursor position: col + 80*row.
  outb(CRTPORT, 14);
  pos = inb(CRTPORT+1) << 8;
  outb(CRTPORT, 15);
  pos |= inb(CRTPORT+1);

  if(c == '\n')
    pos += 80 - pos%80;
  else if(c == BACKSPACE){
    if(pos > 0) --pos;
  } else
    crt[pos++] = (c&0xff) | 0x0700;  // black on white

  if(pos < 0 || pos > 25*80)
    panic("pos under/overflow");

  if((pos/80) >= 24){  // Scroll up.
    memmove(crt, crt+80, sizeof(crt[0])*23*80);
    pos -= 80;
    memset(crt+pos, 0, sizeof(crt[0])*(24*80 - pos));
  }

  outb(CRTPORT, 14);
  outb(CRTPORT+1, pos>>8);
  outb(CRTPORT, 15);
  outb(CRTPORT+1, pos);
  crt[pos] = ' ' | 0x0700;
}

void
consputc(int c)
{
  if(panicked){
    cli();
    for(;;)
      ;
  }

  if(c == BACKSPACE){
    uartputc('\b'); uartputc(' '); uartputc('\b');
  } else
    uartputc(c);
  cgaputc(c);
}

#define INPUT_BUF 128
struct {
  char buf[INPUT_BUF];
  uint r;  // Read index
  uint w;  // Write index
  uint e;  // Edit index
} input;

#define C(x)  ((x)-'@')  // Control-x

void
tab(void) {
  int i;
  for(i = 0; i < TAB_LENGHT; i++) {
    input.buf[input.e++ % INPUT_BUF] = ' ';
    consputc(' ');
  }
}

void
previous(void)
{
  int i, j = 1, h = 0, k;
  int a = 0, flag = 0;
  char tmp[INPUT_BUF + 2];

  //if no command have been submitted yet
  for(i = 0; i < input.e; i++) {
    if(input.buf[i] == '\n') {
      j = 0;
    }
  }

  //exit from the function without doing anything
  if(j == 1)
    return;

  i = input.e % INPUT_BUF - 1;
  
  //copy the content of the current line in tmp
  tmp[INPUT_BUF + 1] = 0;
  for(j = INPUT_BUF; input.buf[i % INPUT_BUF] != '\n'; i--, j--) {
    tmp[j] = input.buf[i % INPUT_BUF];
  }
  tmp[j] = 0;
  
  //skip all \n
  while(input.buf[i % INPUT_BUF] == '\n') {
      i--;
      a++;
  }

  k = i + 1;

  //check if the current line is equal to the last command
  for(h = INPUT_BUF; tmp[h] != 0 && input.buf[i % INPUT_BUF] != '\n'; i--, h--) { 
    if(input.buf[i % INPUT_BUF] == tmp[h]) {
      a++;
    }
  }

  //check if it is really equal
  if(input.buf[k] != '\n') {
    for(h = INPUT_BUF, i = k - 1; tmp[h] != 0; i--, h--) { 
      if(input.buf[i % INPUT_BUF] == tmp[h]) {
        a--;
      }
    }
  }

  //skip all /n
  while(input.buf[i % INPUT_BUF] == '\n') {
     a++;
     i--;
  }

  //delete current line
  while(input.e != input.w) {
    input.e--;
    consputc(BACKSPACE);
  }

  //check for the previous command
  for(i = 0; i % INPUT_BUF < (input.e - a) % INPUT_BUF; i++) {
    if(input.buf[i % INPUT_BUF] == '\n') {
        j = i + 1;
        flag = 1;
      }
  }
  if(flag == 0) {
    if(input.buf[input.e - a] == '\n' || input.e - a <= 0) {
      j = 0;
    }
    else {
      for(i = 0; i < INPUT_BUF; i++) {
        if(input.buf[i] == '\n') {
          j = i + 1;
          flag = 1;
        }
      }
    }
  }

  //print the command on the console
  
  for(i = j; input.buf[i % INPUT_BUF] != '\n'; i++) {
    input.buf[input.e++ % INPUT_BUF] = input.buf[i % INPUT_BUF];
    consputc(input.buf[i % INPUT_BUF]);
  }
}

void
consoleintr(int (*getc)(void))
{
  int c, doprocdump = 0;

  acquire(&cons.lock);
  while((c = getc()) >= 0){
    switch(c){
    case C('P'):  // Process listing.
      // procdump() locks cons.lock indirectly; invoke later
      doprocdump = 1;
      break;
    case C('U'):  // Kill line.
      while(input.e != input.w &&
            input.buf[(input.e-1) % INPUT_BUF] != '\n'){
        input.e--;
        consputc(BACKSPACE);
      }
      break;
    case C('H'): case '\x7f':  // Backspace
      if(input.e != input.w){
        input.e--;
        consputc(BACKSPACE);
      }
      break;
    case 228:  // Left arrow
      if(input.e != input.w){
        input.e--;
        consputc(BACKSPACE);
      }
      break;
    default:
    if(c == 9) {  //tab key
      tab();
      continue;
    }
    if(c == 226 || c == 227) { //up & down arrow
      previous();
      continue;
    }
    if(c == 229) { //right arrow
      if(input.buf[input.e % INPUT_BUF] != 0 && input.buf[input.e % INPUT_BUF] != '\n')
        consputc(input.buf[input.e++ % INPUT_BUF]);
      continue;
    }
      if(c != 0 && input.e-input.r < INPUT_BUF){
        c = (c == '\r') ? '\n' : c;
        input.buf[input.e++ % INPUT_BUF] = c;
        consputc(c);
        if(c == '\n' || c == C('D') || input.e == input.r+INPUT_BUF){
          input.w = input.e;
          wakeup(&input.r);
        }
      }
      break;
    }
  }
  release(&cons.lock);
  if(doprocdump) {
    procdump();  // now call procdump() wo. cons.lock held
  }
}

int
consoleread(struct inode *ip, char *dst, int n)
{
  uint target;
  int c;

  iunlock(ip);
  target = n;
  acquire(&cons.lock);
  while(n > 0){
    while(input.r == input.w){
      if(myproc()->killed){
        release(&cons.lock);
        ilock(ip);
        return -1;
      }
      sleep(&input.r, &cons.lock);
    }
    c = input.buf[input.r++ % INPUT_BUF];
    if(c == C('D')){  // EOF
      if(n < target){
        // Save ^D for next time, to make sure
        // caller gets a 0-byte result.
        input.r--;
      }
      break;
    }
    *dst++ = c;
    --n;
    if(c == '\n')
      break;
  }
  release(&cons.lock);
  ilock(ip);

  return target - n;
}

int
consolewrite(struct inode *ip, char *buf, int n)
{
  int i;

  iunlock(ip);
  acquire(&cons.lock);
  for(i = 0; i < n; i++)
    consputc(buf[i] & 0xff);
  release(&cons.lock);
  ilock(ip);

  return n;
}

void
consoleinit(void)
{
  initlock(&cons.lock, "console");

  devsw[CONSOLE].write = consolewrite;
  devsw[CONSOLE].read = consoleread;
  cons.locking = 1;

  ioapicenable(IRQ_KBD, 0);
}

