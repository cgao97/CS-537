#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "x86.h"
#define PGSIZE 4096
char*
strcpy(char *s, char *t)
{
  char *os;

  os = s;
  while((*s++ = *t++) != 0)
    ;
  return os;
}

int
strcmp(const char *p, const char *q)
{
  while(*p && *p == *q)
    p++, q++;
  return (uchar)*p - (uchar)*q;
}

uint
strlen(char *s)
{
  int n;

  for(n = 0; s[n]; n++)
    ;
  return n;
}

void*
memset(void *dst, int c, uint n)
{
  stosb(dst, c, n);
  return dst;
}

char*
strchr(const char *s, char c)
{
  for(; *s; s++)
    if(*s == c)
      return (char*)s;
  return 0;
}

char*
gets(char *buf, int max)
{
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
    cc = read(0, &c, 1);
    if(cc < 1)
      break;
    buf[i++] = c;
    if(c == '\n' || c == '\r')
      break;
  }
  buf[i] = '\0';
  return buf;
}

int
stat(char *n, struct stat *st)
{
  int fd;
  int r;

  fd = open(n, O_RDONLY);
  if(fd < 0)
    return -1;
  r = fstat(fd, st);
  close(fd);
  return r;
}

int
atoi(const char *s)
{
  int n;

  n = 0;
  while('0' <= *s && *s <= '9')
    n = n*10 + *s++ - '0';
  return n;
}

void*
memmove(void *vdst, void *vsrc, int n)
{
  char *dst, *src;
  
  dst = vdst;
  src = vsrc;
  while(n-- > 0)
    *dst++ = *src++;
  return vdst;
}
int
thread_create(void (*start_routine)(void *, void *), void *arg1, void *arg2)
{
  void *stack, *stack_free;
  stack_free = malloc(2 * PGSIZE);
  if (stack_free == 0) {
	  printf(2, "malloc failed\n");
	  return -1;
  }
  uint offset = (uint)stack_free % PGSIZE;
  stack = stack_free + (PGSIZE - offset);
  *((uint *)stack - 1) = (uint)stack_free;
  return clone(start_routine, arg1, arg2, stack);
}
int thread_join()
{
  void *stack, *stack_free;
  int tid = join(&stack);
  stack_free = (void *)(*((uint *)stack - 1));
  free(stack_free);
  return tid;
}
void
lock_init(lock_t *lk)
{
  lk->ticketnumber = 0;
  lk->turn = 0;
}
 int fa(int* variable, int value)
  {
      __asm__ volatile("lock; xaddl %0, %1"
        : "+r" (value), "+m" (*variable) // input+output
        : // No input-only
        : "memory"
      );
      return value;
  }

void
lock_acquire(lock_t *lk)
{
  int myturn = fa(&lk->ticketnumber,1);
  while(lk->turn != myturn){
	  ;
  }
}

void
lock_release(lock_t *lk)
{
   lk->turn = lk->turn +1;
}

