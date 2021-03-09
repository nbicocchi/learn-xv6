# Stride Scheduling

Lo Stride Scheduling è un algoritmo deterministico della famiglia degli scheduler Fair-share. Ogni processo riceve inizialmente una quantità di ticket che determinano la priorità di un processo rispetto agli altri. Dal numero di tickets si calcola quello che viene chiamato **stride** (la lunghezza di passo) dividendo una costante per il numero di biglietti ricevuti. Lo scheduler utilizza quindi una variabile contatore, il passo, per determinare quale processo deve essere eseguito. In particolare viene scelto, per ogni momento, il processo che ha il valore di passo minore. Una volta che il processo va in esecuzione, incrementa il passo di una quantità uguale allo stride.

**Costante = 10.000**

|   Processo    |    Tickets    |     Stride    |
| ------------- |:-------------:|:-------------:|
|      A        |      100      |      100      |
|      B        |      50       |      200      |
|      C        |      250      |      40       |


**Schedulazione**

|    Passo(A)   |    Passo(B)   |   Passo (C)   | Esecuzione |
| ------------- |:-------------:|:-------------:|:----------:|
|      0        |       0       |      0        |     A      |
|      100      |       0       |      0        |     B      |
|      100      |      200      |      0        |     C      |
|      100      |      200      |      40       |     C      |
|      100      |      200      |      80       |     C      | 
|      100      |      200      |      120      |     A      |
|      200      |      200      |      120      |     C      |
|      200      |      200      |      160      |     C      |
|      200      |      200      |      200      |    ...     |


## Implementazione
Affinché lo scheduler rimuova il processo con il passo più breve è stata implementata una struttura di tipo heap binario (illustrata in figura) che permette, in fase di estrazione, di accedere all'elemento con il passo più breve in un tempo O(1). 
![Binary Heap.](/images/HeapVector.png)
L'heap binario è stato implementato utilizzando un vettore di riferimenti ai processi, in modo da mantenere sempre come root l'elemento con il gradino più basso.
Quindi sono stati aggiunti alla struttura di ogni processo gli attributi *tickets*, *stride*, *pass* e la costante, necessari per l'implementazione dell'algoritmo. 
Per assegnare a ciascun processo un ticket, è stata modificata la funzione fork() in modo da poter passare come parametro il numero di ticket che riceverà il processo durante la sua creazione. 
Il passo successivo è stato quello di inizializzare i processi in fase di creazione con i nuovi attributi, e quindi aggiungerli correttamente nell'heap.
Infine è stato modificato il funzionamento dello scheduler: se l'heap non è vuoto, esso preleva il processo con il passo più breve, calcola il nuovo passo ed esegue il processo estratto.

## Funzionamento
Per testare il nuovo scheduler è stato inserito un nuovo comando all'interno di xv6
```
$ stride nprocess exectime
```
Il comando crea un numero *n* di processi figlio, ognuno di essi con un numero diverso di ticket. Questi processi sostanzialmente attraversano un ciclo enorme senza far nulla, con la sola intenzione di trascorrere del tempo alternando fasi di run con fasi in cui attendono di essere schedulati. 
La durata del programma viene espressa dall'utente con il secondo parametro *exectime* (ogni unità corrisponde circa ad un secondo).

Facendo partire ad esempio il programma con 3 processi
```
$ stride 3 100
```
```
Xv6 Stride Scheduling

Press Ctrl+P to see details about processes
Child 5 with 50 tickets!
Child 6 with 100 tickets!
Child 7 with 150 tickets!
```
Premendo la sequenza di tasti *ctrl + p* è possibile visualizzare le proprietà dei processi: *pid, name, state, tickets, stride, pass* ed un ultimo parametro, *scheduled*, che indica quante volte è stato schedulato un processo. 
```
Process with PID: 1 state: sleep  name: init Tickets: 50 Stride: 200 Pass: 4800 Scheduled: 24
Process with PID: 2 state: sleep  name: sh Tickets: 50 Stride: 200 Pass: 27600 Scheduled: 138
Process with PID: 53 state: sleep  name: stride Tickets: 50 Stride: 200 Pass: 600 Scheduled: 81
Process with PID: 54 state: run    name: stride Tickets: 50 Stride: 200 Pass: 7800 Scheduled: 3021
Process with PID: 55 state: runble name: stride Tickets: 100 Stride: 100 Pass: 7700 Scheduled: 3296
Process with PID: 56 state: runble name: stride Tickets: 150 Stride: 66 Pass: 7656 Scheduled: 3440

Heap: PID 56 Pass 7656, PID 55 Pass 7700

```
Osservando i valori di schedulazione dei processi creati dal programma, è possibile vedere che i processi con il maggior numero di ticket, e di conseguenza con lo stride minore, sono stati schedulati proporzionalmente più volte rispetto agli altri. 
Inoltre, è possibile vedere come i processi runnable che sono in attesa di essere schedulati sono inseriti correttamente all'interno della coda heap.

## Papers
http://web.eecs.umich.edu/~mosharaf/Readings/Stride.pdf
