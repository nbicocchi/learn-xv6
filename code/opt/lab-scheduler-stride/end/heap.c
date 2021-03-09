#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"
#include "heap.h"


int insert(struct proc *p, int last, struct proc* heap[]){
  heap[last] = p;
  return checkHeapParent(last, heap);
}

//target is the process to be removed
struct proc *extract(int target, struct proc* heap[], int last){
  struct proc *q;
  q = heap[target];
  heap[last]->position = heap[target]->position;
  heap[target] = heap[last];
  checkHeapChild(target, heap, last);
  return q;
}


void checkHeapChild(int i, struct proc* heap[], int last){ 
  int smallest = i;
  struct proc* tmp;
  if(valid(child_left(i), last) && heap[child_left(i)]->pass < heap[smallest]->pass) smallest = child_left(i);
  if(valid(child_right(i), last) && heap[child_right(i)]->pass < heap[smallest]->pass) smallest = child_right(i);

  if(smallest != i){
    tmp = heap[smallest];
    heap[smallest] = heap[i];
    heap[i] = tmp;
    tmp->position = heap[smallest]->position;
    heap[smallest]->position = heap[i]->position;
    heap[i]->position = tmp->position;
    checkHeapChild(smallest, heap, last);
  }
  return;
}

int checkHeapParent(int i, struct proc* heap[]){ 
  struct proc* tmp;

  if(i == 1) 
    return i; 
  if(heap[parent(i)]->pass > heap[i]->pass){
    tmp = heap[parent(i)];
    heap[parent(i)] = heap[i];
    heap[i] = tmp;
    tmp->position = heap[parent(i)]->position;
    heap[parent(i)]->position = heap[i]->position;
    heap[i]->position = tmp->position;
  }
  checkHeapParent(parent(i), heap);
  return i;
}
