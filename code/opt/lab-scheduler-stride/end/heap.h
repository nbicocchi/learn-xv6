#define child_left(x) (2 * x) 
#define child_right(x) (2 * x + 1) 
#define parent(x) (x % 2 ? (x-1)/2 : x/2) 
#define valid(x, last) (x <= last)

int insert(struct proc *p, int last, struct proc *heap[]);
struct proc *extract(int target, struct proc* heap[], int last);
void checkHeapChild(int i, struct proc* heap[], int last);
int checkHeapParent(int i, struct proc* heap[]);
