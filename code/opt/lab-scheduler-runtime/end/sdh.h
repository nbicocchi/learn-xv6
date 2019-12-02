/////////////////////////////////////////
/// Scheduler policies - Name mapping ///
/////////////////////////////////////////
static char *schedulerName[] = {
  [0] "DEFAULT",
  [1] "PRIORITY",
  [2] "FCFS",
  [3] "CFS",
  [4] "SML"
};

//////////////////////////////////////////
/// Scheduler policies - Main function ///
//////////////////////////////////////////
struct proc *defaultScheduler(void);
struct proc *priorityScheduler(void);
struct proc *fcfsScheduler(void);
struct proc *cfsScheduler(void);
struct proc *smlScheduler(void);

/////////////////////////////////////////////
/// Scheduler policies - Function mapping ///
/////////////////////////////////////////////
static struct proc *(*schedulerFunction[]) () = { 
  [0] defaultScheduler,
  [1] priorityScheduler,
  [2] fcfsScheduler,
  [3] cfsScheduler,
  [4] smlScheduler
};

//////////////////////////////////////////////
/// Default scheduler policy from compiler ///
/// initialization of the ready_process    ///
/// and schedSelected variables defined in ///
/// compilation                            ///
//////////////////////////////////////////////
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