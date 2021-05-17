#ifndef _PCB_h_
#define _PCB_h_
//#include <IOSTREAM.H>

enum State{ready,running1,waiting,delayed,blocked,finished };

class PCB;
struct Node{
	Node* next;
	PCB*  data;
	Node(PCB* d){ data=d; next=0; }
	~Node(){ data=0; next=0;}
};

class Thread;
class KernelSem;

class PCB{
friend class Thread;
friend class KernelSem;
friend class Semaphore;
friend void inic();
friend void nultiSig();
friend void interrupt timer1 ();
friend  void interrupt timer(...);

int blk; //0 sem 1 timer
int time;
State state;
static int IDs;
static PCB* allPCBs[100];
Thread *myThread;
Node *waitingList;
KernelSem *myKernelSem;
unsigned ss,sp,bp,*stek;

void oslobodiNiti();

public:
int ID;
static PCB* running;

PCB(unsigned long stackSize, int timeSlice, Thread* t);
~PCB();
static void wraper();

};

#endif
