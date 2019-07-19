### Leonardo Cavazzoni Pederzini - Matricola 137198

# Added Intra Process Communication (IPC to xv6)

include 'user.h' to use the new functionality

added 2 new system calls!

FIFO message queue without timed wait implemented
sender has sync/async functionality
in sync case sender blocks until the receiver receives the message
in async case sender never blocks

3 tests available:
testmsg, proc1 sends a message to proc2, proc2 replies
testmsgqueue is the same as testmsg, but the message is split to confirm FIFO queue functionality
testsync proc1 waits for proc2 to receive the message. Proc 2 wastes some time before receiving the message

    
###sendmsg(int md, void *buffer,int sync);
This new system call makes possible to send a message to a mailbox
'int md' indicates which mailbox we are sending the message to. There is a limited number
of mailboxes, defined inside param.h
'void *buffer' is the message we store inside the mailbox. Max char size is defined inside param.h
'int sync' defines if the send is synchronous or asynchronous. To send a synchronous message int sync must be 1,
to send an asynchronous message int sync must be 0

sendmsg copies the message inside the buffer into the mailbox, making possible for other processes
to retrieve that message from the same mailbox.
In case sendmsg is synchronous the sender will wait for an answer from the receiver (sleep & release lock)

returns -1 if it fails, 0 on success

###recvmsg(int md, void *buffer);
This new system call makes possible to read a message from a mailbox and if the mailbox is empty
the process will wait for a message (sleep & release lock) until a new message is stored inside the mailbox.
Once the message is retrieved from the mailbox we can wake the sender process (if the send was synchronous)

returns -1 if it fails, 0 on success


##Inside the files! (changes to files)
###'usys.S', 'user.h', 'sysproc.c', 'syscall.h', 'syscall.c', 'defs.h'
in these files there are all the informations needed to make the new system calls available
Those informations consist of function declarations and a way to share variables to the OS

###'param.h'
inside param.h there are 3 new defines, used by the functions inside proc.c
-MSG_SIZE is the maximum size of a message
-MAILBOX_SIZE defines the number of available mailboxes
-MAILBOX_MSG_QUEUE defines the maximum number of messages inside a single mailbox

###'proc.c'
here we can find the 2 new system calls, an initialization function for the mailbox and the mailbox struct
The mailbox struct has a spinlock for every mailbox, to prevent data inconsistency
and 5 different set of datas:
-index for both send and receive
-size of the mailbox, to know how many messages already are inside the mailbox
-queue of messages
-flag for defining if a message is synchronous or not

inside userinit() there is the mailbox_init call
mailbox_init sets all the indexes and sizes to 0

sendmsg:
if the mailbox queue is full and the specified mailbox doesn't exist returns -1
makes a copy of the sent message and stores it inside the mailbox
after doing so increments the size of the mailbox adnd sets the index for the next send
in case the message is synchronous sleep until the receiver receives the message
at the end return 0 if no error occured

recvmsg:
if the mailbox doesn't exists return -1
if the mailbox is empty release the lock and sleep
otherwise copy message from the mailbox queue to the buffer
decrements the index
at the end return 0 if no error occured



#### Small Extra - Clear
added just to clear the terminal after tests :)