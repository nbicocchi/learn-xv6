## Operating Systems Course - DIEF UNIMORE ##

IPC - shared memory

In questa tesina viene implementata nel sistema xv6 la possibilità di far comunicare due processi tramite la condivisione di memoria. In particolare è stata realizzata una system call così definita:

void* shmgetat(int key, int num_pages){ ... }

La funzione accetta come parametri una key e un numero di pagine, e ritornerà al processo chiamante un puntatore ad un'area di memoria di dimensione pari al numero di pagine specificato. In particolare, la funzione alloca il numero di pagine fisiche indicato, associandole alla chiave, e le mappa nella memoria virtuale del processo, a partire dall'ultima pagina dell'heap.

void* shmgetat(int key, int num_pages){
...
    for(i = 0; i < num_pages; i++) {
      mem = kalloc();  // Physical memory
      if (mem == 0) {
        cprintf("allocuvm out of memory\n");
        return 0;
      }
      memset(mem, 0, PGSIZE);

      // Store the physical page for future use
      key_pageaddr[key][i] = mem;

      // Change address of next available VP
      proc->top -= PGSIZE;

      // Set up mapping for this process
      // VA.
      addr = (void*)(proc->top);

      // Map vp to pp
      if (mappages(proc->pgdir, addr, PGSIZE, V2P(mem), PTE_P|PTE_W|PTE_U) < 0) {
        return 0;
      }
    //key now used by the process
    proc->keys[key] = 1;
  }
...
}

Nel caso qualunque altro processo provasse ad utilizzare la stessa chiave per chiamare la funzione, si vedrà mappate nella proprio memoria virtuale le stesse pagine fisiche precedentemente allocate, sempre partendo dall'ultima pagina dell'heap.

...
else { 
    if (proc->keys[key] == 0) { 
      for(i = 0; i < num_pages; i++) {
      
        proc->top -= PGSIZE;
        addr = (void*)(proc->top);

        
        if (mappages(proc->pgdir, addr, PGSIZE,
             V2P(key_pageaddr[key][i]), PTE_P|PTE_W|PTE_U) < 0) {
          return 0;
        }
      }
    } 
    //key now used by the process
    proc->keys[key] = 1;
  }
...

Viene quindi ritornato dalla funzione l'indirizzo virtuale dell'ultima pagina mappata/allocata. In questo modo i due processi potranno leggere e scrivere dati in una stessa area di memoria, potendo quindi comunicare anche quando non appartenenti a una stessa gerarchia. Per arrivare a realizzare la system call presentata, sono necessarie alcune strutture dati e delle funzioni aggiuntive per gestirle

#Strutture dati - GLOBALI
int key_refcount[NKEYS];
int key_numpages[NKEYS];
void* key_pageaddr[NKEYS][NKEYPAGES];

Le strutture vengono definite in vm.c. Si tratta di due array uno-dimensionali e un array bidimensionale, le cui dimensioni sono stabilite per mezzo delle variabili NKEYS ed NKEYPAGES, definite in defs.h, e i cui valori di default sono rispettivamente 8 e 4.
Gli array devono essere inizializzati nelle prime fasi del sistema. Si sfrutterà una funzione così definita:

void shmeminit(){ .. }

L'unico scopo della funzione sarà quello di rimempire tutti gli array di 0, e verrà chiamata una volta sola in main.c

..
userinit();      // first user process
shmeminit();	 // setting up shared memory
mpmain();        // finish this processor's setup
..

Vediamo ora per cosa vengono utilizzati i tre array:
- key_numpages mantiene il numero di pagine allocate associate alla chiave
- key_pageaddr mantiene l'indirizzo (fisico) di ogni pagina allocata associata alla chiave
- key_refcount mantiene il conteggio del numero di processi che stanno "usando" la chiave

I primi due array vengono utilizzati solo internamente a vm.c, nella system call shmgetat() e nella funzione dec_ref_count() che verrà presentata a breve. Il terzo array invece deve poter essere acceduto e modificato non solo in shmgetat() ma anche dall'esterno (ad esempio quando un processo che utilizzava una chiave termina). Vengono quindi definite altre due funzioni:

int shm_refcount(int key){ ... }
void dec_ref_count(struct proc *p){ ... }

La prima è una system call che ritorna il valore contenuto nell'array per quella chiave. Può essere utilizzato dal processo chiamante per controllare se vi sono altri processi con cui comunicare, o per altre funzioni di controllo. La seconda invece è una funzione chiamata da altre funzioni in kernel space per decrementare il valore di ref_count per le chiavi utilizzate da un processo P. Risulta necessario ricorrere a questa funzione nei seguenti casi:
- quando viene chiamata una exec()
- quando viene chiamata una exit()

Nel caso in cui il decremento porti il valore di ref_count a 0 per quella chiave, le pagine fisiche vengono deallocate e gli array riportati a valori nulli:

void
dec_ref_count(struct proc *p)
{
  ...
      if(key_refcount[i] == 0) {
	//Freeing memory associated to that key since no one is using that
        for(j=0;j<key_numpages[i];j++) {
		kfree(key_pageaddr[i][j]);
		key_pageaddr[i][j]=0;		
	}
	key_numpages[i]=0;
      }
  ...
}

Rimane da definire sia come si determini quali chiavi sono in uso in un processo, sia come gestire la mappatura in memoria delle pagine condivise quando vengono invocate più shmgetat() con diverse chiavi, per evitare sovrapposizioni. Sono necessari quindi alcuni dati da inserire nei descrittori di un processo

#Strutture dati - PROCESSI
int keys[NKEYS];	       // Keys used by the process for shared memory
int top;		       // VA top shmem

L'array binario memorizza le chiavi utilizzate dal processo, mentre l'intero top è un puntatore che rappresenta il limite superiore dell'heap di quel processo (inizialmente settato a KERNBASE, viene decrementato ogni volta che l'ultima pagina dell'heap viene utilizzata per mappare una pagina condivisa). Vengono entrambi inseriti nella definizione della struct proc del file proc.h, ed inizializzati nella funzione allocproc()

...
  p->state = EMBRYO;
  p->pid = nextpid++;
  for(int i = 0; i<NKEYS; i++) p->keys[i] = 0;
  p->top = KERNBASE;

  release(&ptable.lock);
...

Sono inoltre entrabi aggiornati solo ed esclusivamente nella system call shmgetat(). Per mantenere i parametri consistenti sarà necessario prestare attenzione ai seguenti casi:
* nella fork() e nella exec(), dove i parametri devono essere inizializzati nuovamente
* nella allouvm(), per evitare di superare top e sovrascrivere le pagine condivise mappate (se sono state allocate pagine top è minore di KERNBASE)
..
int
allocuvm(pde_t *pgdir, uint oldsz, uint newsz)
{
  char *mem;
  uint a;

  if(newsz > myproc()->top)
    return 0;
..
}
* nella freevm(), dove si rischia di deallocare più volte le stesse pagine(poichè le pagine fisiche della memoria condivisa sono mappate in più processi). Il compito di deallocare le pagine fisiche è, come già visto, demandato a dec_ref_count(), e sarà necessario ridefinire la funzione freevm() come di seguito;
void freevm(pde_t *pgdir, int top){
  uint i;

  if(pgdir == 0)
    panic("freevm: no pgdir");
  //If shared memory has not been used, top will still be KERNBASE.
  //If not, we must avoid deallocating memory in the space dedicated to shmem (it will be
  //deallocated in the dec_ref_count function whenever the ref count comes to 0)
  deallocuvm(pgdir, top, 0); 
...
  
#Testing - PRODUTTORE/CONSUMATORE
Il tutto può essere testato utilizzando il comando $test, che genera un processo PRODUTTORE e un processo CONSUMATORE (i cui codici sono contenuti rispettivamente in test.c, producer.c, consumer.c). Il processo test chiama il metodo shmgetat(), allocando le pagine e assicurando che il valore di ref_count non scenda mai a zero fino all'uscita di entrambi i figli. Sarà poi il processo produttore a chiamare nuovamente shmgetat() con la stessa chiave e a scrivere sulle pagine. Il processo consumatore si sospende per qualche secondo, per poi chiamare shmgetat() con la stessa chiave utilizzata dal produttore e leggere quindi dalla stessa area i dati inseriti (in questo caso degli interi).
