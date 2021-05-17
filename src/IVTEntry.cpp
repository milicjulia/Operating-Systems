#include "IVTEntry.h"
#include<dos.h>
#include <IOSTREAM.H>
#include "PCB.h"
#include "semaphor.h"
#include "KernelEv.h"
//extern int theEnd;
IVTEntry* IVTEntry::IVT[256]={0};

IVTEntry::IVTEntry(unsigned char ulaz,pInterrupt inter){
	lock;
	redni_broj=ulaz;
	IVT[redni_broj]=this;
	old=getvect(ulaz);
	setvect(ulaz,inter);
	unlock;
}

IVTEntry::~IVTEntry(){
	IVT[redni_broj]=0;
	setvect((unsigned char)redni_broj,old);
}


void IVTEntry::signal(){
	myKernelEv->signal();
}

void IVTEntry::proslaRutina(){
	old();
}
