#ifndef _kevent_h_
#define _kevent_h_
#include "IVTEntry.h"
#include "semaphor.h"

class PCB;
class IVTEntry;

class KernelEv{
private:
int turn;
IVTEntry *myIVTEntry;
PCB* myPCB;
Semaphore *s;

public:
KernelEv(int redni_br);
void wait();
~KernelEv();
void signal();

};
#endif
