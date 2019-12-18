## Operating Systems Course - DIEF UNIMORE ##

In this exercise we will implement a lazy allocation of heap memory in xv6. User applications ask the kernel for heap memory using the sbrk() system call. The use it indirectly by calling the library function malloc() which uses sbrk() internally. Below, you can see the morecore() function which is called by malloc() when pre-allocated memory is finished. Morecore() allocates additional memory using sbrk().

```text
static Header*
morecore(uint nu)
{
  char *p;
  Header *hp;

  if(nu < 4096)
    nu = 4096;
  p = sbrk(nu * sizeof(Header));
  if(p == (char*)-1)
    return 0;
  hp = (Header*)p;
  hp->s.size = nu;
  free((void*)(hp + 1));
  return freep;
}
```

sbrk() system call allocates physical memory and maps it into the process's virtual address space. Some programs allocate memory but never use it, this result in a waste of space (e.g. allocation of sparse array). Modern kernels (e.g., Linux) don't allocate pages until applications ask for them. This mechanism is called **lazy allocation** and we have to implement it in xv6.

In the first part, we need to bypass the traditional allocation mechanism provided by the function sbrk(). This function relies on the sys_sbrk() system call in sysproc.c which grows the process's memory size by making use of growproc(). We should just increment the size of process memory by n (myproc()->sz + n) without actually allocating memory (removing the call to growproc()). 

It is worth remembering that the memory allocation chain, detailed below, works in this way:

* malloc() verifies pre-allocated memory. If not available, calls morecore().
* morecore() calculates the number of bytes (and thus pages) needed. Then calls sbrk().
* sbrk() updates the process state (struct proc) with the new amount of memory and calls growproc() for the actual allocation.
* growproc() calls allocuvm() or deallocuvm() for allocating or deallocating memory.
* allocuvm(), finally, calls kalloc() which actually (using the freelist linked list) allocates the needed memory pages.


```text
int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  // remove the growproc call so that the process can't allocate 
  // memory on demand leading to a page fault 
  // if(growproc(n) < 0) 
  // return -1;

  // increase the process space by n
  myproc()->sz = myproc()->sz + n; 

  return addr;
}
```

```
// Grow current process's memory by n bytes.
// Return 0 on success, -1 on failure.
int
growproc(int n)
{
  uint sz;
  struct proc *curproc = myproc();

  sz = curproc->sz;
  if(n > 0){
    if((sz = allocuvm(curproc->pgdir, sz, sz + n)) == 0)
      return -1;
  } else if(n < 0){
    if((sz = deallocuvm(curproc->pgdir, sz, sz + n)) == 0)
      return -1;
  }
  curproc->sz = sz;
  switchuvm(curproc);
  return 0;
}
```
```
// Allocate page tables and physical memory to grow process from oldsz to
// newsz, which need not be page aligned.  Returns new size or 0 on error.
int
allocuvm(pde_t *pgdir, uint oldsz, uint newsz)
{
  char *mem;
  uint a;

  if(newsz >= KERNBASE)
    return 0;
  if(newsz < oldsz)
    return oldsz;

  a = PGROUNDUP(oldsz);
  for(; a < newsz; a += PGSIZE){
    mem = kalloc();
    if(mem == 0){
      cprintf("allocuvm out of memory\n");
      deallocuvm(pgdir, newsz, oldsz);
      return 0;
    }
    memset(mem, 0, PGSIZE);
    if(mappages(pgdir, (char*)a, PGSIZE, V2P(mem), PTE_W|PTE_U) < 0){
      cprintf("allocuvm out of memory (2)\n");
      deallocuvm(pgdir, newsz, oldsz);
      kfree(mem);
      return 0;
    }
  }
  return newsz;
}
```

```
// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
char*
kalloc(void)
{
  struct run *r;

  if(kmem.use_lock)
    acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  if(kmem.use_lock)
    release(&kmem.lock);
  return (char*)r;
}
```

After compiling and booting xv6, if we enter the ls command we should see something like:

```text
init: starting sh
$ ls
pid 3 sh: trap 14 err 6 on cpu 0 eip 0x12f1 addr 0x4004--kill proc
$
```

The handler caught a page fault, a sort of exception called trap, (TRAP 14 or T_PGFLT) because the program tried to access to the memory address 0x4004 (outside the actual process's address space).

In the second part, we must handle this TRAP 14. Every time a page fault (TRAP 14) occurr we need to map a newly-allocated page of physical memory at the faulting address. Then, execution goes back to user space and let the process continue. To do that we handle the fault just before the cprintf() call that produced the error message in the trap.c file.

* We can check whether a fault is a page fault by checking if tf->trapno is equal to T_PGFLT in trap() 
* We need to allocate a new page to the process. Steal code from allocuvm() in vm.c, which is what sbrk() calls (via growproc())
* Find the virtual address that caused the page fault from the cprintf argument. We need the memory address to map the newly allocated page
* Round the faulting virtual address down to a multiple of page boundary so that they become aligned
* Map the new page into the process's pagetable we'll need to call mappages(). In order to do this you'll need to delete the static in the declaration of mappages() in vm.c, and you'll need to declare mappages() in trap.c. Add this declaration to trap.c before any call to mappages(): int mappages(pde_t *pgdir, void *va, uint size, uint pa, int perm);
* Break or return before the cprintf error in order to avoid myproc()->iskilled = 1

```text
  if(tf->trapno == T_PGFLT) {
    // In user space, assume process misbehaved.
    cprintf("pid %d %s: trap %d err %d on cpu %d "
            "eip 0x%x addr 0x%x--kill proc\n",
            myproc()->pid, myproc()->name, tf->trapno,
            tf->err, cpuid(), tf->eip, rcr2());

    // Allocate one 4096-byte page of physical memory.
    char *mem;
    mem = kalloc(); 
    
    if(mem == 0){
      cprintf("alloc lazy page, out of memory\n");
      return ;
    }
    
    // fills the first PGSIZE bytes of the memory area pointed 
    // to by mem with the constant byte 0. init allocated page
    memset(mem, 0, PGSIZE); 

    // Round the faulting virtual address down to a multiple of page boundary so that they become aligned
    uint a = PGROUNDDOWN(rcr2()); 
    
    // map the page into the process page table kernel. Kernel uses virtual 
    // address while the page table uses physical address
    mappages(myproc()->pgdir, (void*) a, PGSIZE, V2P(mem), PTE_W | PTE_U); 
    
    return ;
  } 
```

Good work!

