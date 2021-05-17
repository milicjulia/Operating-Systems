#ifndef IVTENTRY_H_
#define IVTENTRY_H_

typedef void interrupt (*pInterrupt)(...);

class PCB;
class KernelEv;
class Semaphore;


class IVTEntry{
int redni_broj;
pInterrupt old;

public:
static IVTEntry* IVT[256];
KernelEv *myKernelEv;
IVTEntry(unsigned char ulaz,pInterrupt inter);
~IVTEntry();
void signal();
void proslaRutina();

};



#endif
