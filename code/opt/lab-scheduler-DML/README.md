# Scheduler DML - Dynamic Multi Level queue scheduling
#### *Progetto realizzato da Luca Casarin*
----

### Introduzione
Questa patch modifica il laboratorio *06K - Scheduler (Policies)* aggiungendo uno scheduling dinamico per xv6 ed avremo quindi a disposizione le seguenti politiche:

| Policy | Politica di scheduling |
| ------ | ------ |
| DEFAULT | Round-Robin |
| PRIORITY | Priority based |
| FCFS | First Come First Served |
| CFS | Completely Fair Scheduling |
| SML | Static Multi Level queue |
| DML | Dynamic Multi Level queue |

Le proprietà dei primi cinque algoritmi sono già state descritte, per quanto riguarda il *Dynamic Multi Level Queue* si fa riferimento al paragrafo estratto dal libro "Operating Systems" di Stuart E. Madnick [1] qui riportato:

![Stuart E. Madnick, "Operating Systems", McGraw-Hill College](img/book.img)

### Selezione di uno scheduler
La selezione della politica di scheduling, da parte dell'utente, avviene sempre in fase di compilazione, utilizzando il flag *SCHEDPOLICY* e inserendo il nome della politica desiderata.

Il comando da utilizzare da riga di comando sarà il seguente:
```sh
$ generate.sh -l <LAB> -f SCHEDPOLICY=<POLICY>
```
dove al posto di *<LAB>* andrà inserito il nome della cartella in cui si trova la patch ed al posto di *<POLICY>* il nome abbreviato di una delle politiche di scheduling sopra elencati. Se il flag non viene specificato, viene selezionata la politica predefinita di xv6, ovvero Round-Robin.

Per eseguire la nuova patch il comando sarà dunque:
```sh
$ generate.sh -l lab-scheduler-DML -f SCHEDPOLICY=DML
```

### Descrizione scheduling DML
Per prima cosa è stato necessario cambiare il codice per permettere la preemption di un processo ogni quanto di tempo, misurato in ticks del clock, invece che ogni singolo tick. Per fare ciò è stato modificato il file ​*param.h*​, definendo una nuova variabile:
```sh
# define QUANTA <Number>
```
DML (Dynamic Multi Level queue) rappresenta una politica di scheduling preemptive che include 3 code di priorità. Il processo viene inizializzato alla priorità appartenente alla coda intermedia e lo scheduler seleziona un processo da una coda a priorità inferiore solo quando non sono presenti processi *RUNNABLE* nelle code superiori. La differenza principale rispetto alla politica SML (Static Multi Level queue) consiste nella variazione delle priorità è automatica e non più esclusivamente manuale tramite la system call *setpriority*.

### Verifica dello scheduler
Nella shell di xv6 è disponibile il tool *test_scheduler* fornito dal professor Bicocchi, ma per testare la variazione dinamica delle priorità dei processi [2] durante la esecuzione in CPU, è necessario simulare dei comportamenti diversi tra i vari processi e per fare ciò è stato realizzato un apposito test (*test_dml.c*) che segue delle regole [3] così assegnate:
- Eseguire l'intero quanto di tempo (*QUANTA*) decrementa la priorità
- Ritornare dallo stato *SLEEPING* (I/O) incrementa la priorità
- Rilasciare la CPU manualmente (*yield*) mantiene la priorità
- Richiamare la system call *EXEC* resetta la priorità di default

Dato che nel DML ci sono 3 code di priorità in cui vengono divisi i processi, l'incremento ed il decremento della priorità è stato implementato in modo tale da spostare il processo da una coda all'altra, ovviamente nel limite delle possibilità:
```sh
int incpriority(int pid){
  struct proc *p;
  acquire(&ptable.lock);
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->pid == pid){
        if(p->priority <= 7){
	        p->priority = p->priority == 1 ? 1 : p->priority - 1;
    	} else if(p->priority > 7 && p->priority <= 14){
      	        p->priority = 7;
    	} else {
      	        p->priority = 14;
    	}
        break;
    }
  }
  release(&ptable.lock);
  return pid;
}

int decpriority(int pid){
  struct proc *p;
  acquire(&ptable.lock);
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->pid == pid){
        if(p->priority <= 7){
      		p->priority = 8;
    	} else if(p->priority > 7 && p->priority <= 14){
      		p->priority = 15;
    	} else {
		p->priority = p->priority == 20 ? 20 : p->priority + 1;
    	}
        break;
    }
  }
  release(&ptable.lock);
  return pid;
}
```

### Bibliografia
1. Stuart E. Madnick, "Operating Systems", McGraw-Hill College
2. [V. Chahar and S. Raheja, "Fuzzy based multilevel queue scheduling algorithm," 2013 International Conference on Advances in Computing](https://doi.org/10.1109/ICACCI.2013.6637156)
3. [Parallel & Distributed Operating Systems Group](https://pdos.csail.mit.edu)

