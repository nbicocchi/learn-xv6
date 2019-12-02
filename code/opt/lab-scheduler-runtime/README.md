# Scheduler (Runtime Policies)  
## Introduzione
Questa patch per xv6 è una modifica del laboratorio "06K - Scheduler (Policies)".

La patch mette a disposizione le seguenti politiche di scheduling (precaricate e mappate nel sistema con il proprio identificativo sid):

- **(0) DEFAULT** - Round-Robin algorithm (stock xv6)
- **(1) PRIORITY** - Priority based algorithm
- **(2) FCFS** - First Come First Served algorithm
- **(3) CFS** - Completely Fair Scheduling algorithm
- **(4) SML** - Static Multi Level Queue algorithm

Le proprietà delle singole politiche di scheduling sono identiche a quelle del laboratorio 06K.

La modifica della politica di scheduling, da parte dell'utente finale, avviene in due modalità:
- In fase di compilazione, utilizzando il flag **SCHEDPOLICY=POLICY** e inserendo il nome della politica in maiuscolo. Il flag imposta una politica di base da utilizzare ad ogni avvio del sistema. 
Il comando finale da utilizzare è: `$ generate.sh -l labn -f SCHEDPOLICY=POLICY`.
Se il flag non viene specificato, viene applicato la politica di scheduling: **DEFAULT**.
- In runtime utilizzando il tool **scheduler**, accessibile dalla linea di comando. Il tool richiama due system call che modificano il comportamento del sistema.

## System calls
Sono state aggiunte al laboratorio 06K le seguenti system call, presenti nel file `proc.c`:

```c++
int getscheduler(void);
```
Recupera il sid dell'attuale politica di scheduling in esecuzione

```c++
int setscheduler(int sid);
```
Modifica la politica di scheduling, utilizzando il sid. Restituisce l'attuale politica, altrimenti `-1`.

## Tool scheduler
Dalla linea di comando di xv6 è disponibile il comando `scheduler`:

- `scheduler get`: stampa il nome della politica di scheduling e il relativo identificativo sid in esecuzione sul sistema.

- `scheduler default`: stampa il nome della politica di scheduling e il relativo identificativo sid selezionata alla compilazione.

- `scheduler list`: stampa una tabella con i sid, stato e nome di tutte le politiche di scheduling pre-installate nel sistema.

- `scheduler set <sid>`: imposta una nuova politica di scheduling inserendo un sid tra le politiche preinstallate in runtime.
(ricorda che il sid si trova con il comando `scheduler list`). Stampa la politica di scheduling applicata.

- `scheduler set`: stesso effetto di `scheduler set <sid>` ma imposta la politica di scheduling di base selezionata alla compilazione (reperibile con il comando `scheduler default`).

## Installazione di una nuova politica di scheduling
Lo scheduler nel file `proc.c`, prima del cambio di contesto, richiama la funzione 
```c++
static struct proc *(*ready_process)();
```
che rappresenta un puntatore a funzione, configurata dalla system call 
```c++
int setscheduler(int sid);
```
e all'atto della compilazione.
Il puntatore a funzione richiama la funzione principale della politica di scheduling che dovrà restituire il processo successivo da eseguire.
Il cuore del nuovo algoritmo deve risiedere in `proc.c`, comprese la inizializzazione e la rimozione in runtime nelle funzioni `pinit()` e in `int setscheduler(int sid);`

Il mapping avviene nel file `sdh.h`. Il sid (scheduler id) è unico per ogni algoritmo. Nel array 

```c++
static char *schedulerName[] = {
  [0] "DEFAULT",
  [1] "PRIORITY",
  [2] "FCFS",
  [3] "CFS",
  [4] "SML"
};
```
sono presenti i nomi delle politiche associati ai sid. 

Successivamente sono presenti tutte le definizioni delle funzioni principali di ogni politica di scheduling 

```c++
struct proc *defaultScheduler(void);
struct proc *priorityScheduler(void);
struct proc *fcfsScheduler(void);
struct proc *cfsScheduler(void);
struct proc *smlScheduler(void);
```

(utilizzate dal puntatore a funzione ready_process) e il relativo mapping con i sid 

```c++
static struct proc *(*schedulerFunction[]) () = { 
  [0] defaultScheduler,
  [1] priorityScheduler,
  [2] fcfsScheduler,
  [3] cfsScheduler,
  [4] smlScheduler
};
```

Infine, rimangono da gestire le associazioni delle politiche di scheduling di base (la politica che verrà utilizzata ad ogni avvio) provenienti dal compilatore

```c++
#ifdef DEFAULT
  static struct proc *(*ready_process)() = defaultScheduler;
  static int schedSelected = 0;
#elif PRIORITY
  static struct proc *(*ready_process)() = priorityScheduler;
  static int schedSelected = 1;
#elif FCFS
  static struct proc *(*ready_process)() = fcfsScheduler;
  static int schedSelected = 2;
#elif CFS
  static struct proc *(*ready_process)() = cfsScheduler;
  static int schedSelected = 3;
#elif SML
  static struct proc *(*ready_process)() = smlScheduler;
  static int schedSelected = 4;
#else
  static struct proc *(*ready_process)() = defaultScheduler;
  static int schedSelected = 0;
#endif
```

Ora il sistema può essere compilato.

> Perché creare un nuovo file `sdh.h` (scheduler headers) e non modificare `proc.h`? Per semplicità di lettura e di importazione nel codice. Inoltre, ha un comportamento diverso a seconda di chi lo importa nel codice. Se importato in `proc.c`, `ready_process()` e `schedSelected` rappresentano le variabili static che gestisco l'attuale politica di scheduling in esecuzione. Importato altrove `schedSelected` rappresenta lo scheduler di base scelto alla compilazione con la possibilità di utilizzare le strutture dati definite nel file. Il file `sdh.h` è utilizzato in `init.c`, `scheduler.c` e `proc.c`.
