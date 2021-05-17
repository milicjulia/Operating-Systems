#include "KernelEv.h"
#include "IVTEntry.h"
#include"PCB.h"
#include "semaphor.h"
#include "IOSTREAM.H"

KernelEv::KernelEv(int rb){
	turn=1;
	myIVTEntry=IVTEntry::IVT[rb];
	IVTEntry::IVT[rb]->myKernelEv=this;
	myPCB=PCB::running;
	s=new Semaphore(0);
}

void KernelEv::wait(){
	if(PCB::running==myPCB){
		cout<<"Blokiran";
		s->wait(0); }
}

void KernelEv::signal(){

	//if(turn)
		s->signal();
}

KernelEv::~KernelEv(){
	delete s;
}


