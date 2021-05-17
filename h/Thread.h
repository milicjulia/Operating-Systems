#ifndef _thread_h_
#define _thread_h_
#include <iostream.h>

#define lock asm cli   // zabranjuje prekide
#define unlock asm sti  // dozvoljava prekide

typedef unsigned long StackSize;
const StackSize defaultStackSize = 4096;
typedef unsigned int Time; // time, x 55ms
const Time defaultTimeSlice = 2; // default = 2*55ms
typedef void (*SignalHandler)();
typedef unsigned SignalId;
typedef int ID;
class PCB;
struct TimerRequest {
 TimerRequest* next;
 unsigned long int time;
 PCB* pcb;
 TimerRequest(){ next=0;}
 ~TimerRequest(){ pcb=0; next=0;}
};

struct ListInt {
  ListInt* next;
  int x;
  ListInt(int xx){ x=xx; next=0; }
 ~ListInt(){ next=0;}
};

struct ListFunc {
  ListFunc* next;
  SignalHandler func;
  ListFunc(SignalHandler f){ func=f; next=0; }
 ~ListFunc(){ func=0; delete next; }
};


class Thread {
friend void main();
friend class PCB;

private:
 PCB* myPCB;
 Thread *parent;
 ListInt *requests;
 int localSig[16];
 int postpSig[16];
 static unsigned CS,IP;
 ListFunc *functions[16];
 static int globalSig[16];

public:
 friend void inic();
 friend void nultiSig();
 friend  void interrupt timer(...);

 static TimerRequest* timerQueueHead;
 static TimerRequest* firsttimerQueueHead;

 ID getId();
 static ID getRunningId();
 static Thread * getThreadById(ID id);
 void start();
 void waitToComplete();
 virtual ~Thread();

 void signal(SignalId signal);
 void registerHandler(SignalId signal, SignalHandler handler);
 void unregisterAllHandlers(SignalId id);
 void blockSignal(SignalId signal);
 static void blockSignalGlobally(SignalId signal);
 void unblockSignal(SignalId signal);
 static void unblockSignalGlobally(SignalId signal);
 void swap(SignalId id, SignalHandler hand1, SignalHandler hand2);

 static void obradaSig();

protected:
 friend class PCB;
 friend class Semaphore;
 friend void interrupt timer1();

 Thread(StackSize stackSize = defaultStackSize, Time timeSlice =defaultTimeSlice);
 virtual void run() ;
 void sleep(int t);

 };

void dispatch();
extern inter_demand;
void interrupt timer1 () ;
extern void interrupt timer(...);
int postojiFun(SignalId id,SignalHandler h);

#endif
