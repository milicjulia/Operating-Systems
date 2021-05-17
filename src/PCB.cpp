#include "PCB.h"
#include<dos.h>
#include"Thread.h"
#include"SCHEDULE.H"

int PCB::IDs=0;
PCB* PCB::running=0;
PCB* PCB::allPCBs[100]={0};

PCB::PCB(unsigned long stackSize, int timeSlice, Thread* t){
	ID=IDs++;
	myThread=t;
	allPCBs[ID]=this;
	waitingList=0;
	state=running1;
    time=timeSlice;
	StackSize vel=stackSize/sizeof(unsigned long);
	Time kvant=timeSlice;
	stek=new unsigned [vel];
//#ifndef BCC_BLOCK_IGNORE
	sp=FP_OFF(stek+vel-16);
	ss=FP_SEG(stek+vel-16);
	bp=sp;
	stek[vel-1]=FP_SEG(this);
	stek[vel-2]=FP_OFF(this);
	stek[vel-5]=0x200;
	stek[vel-6]=FP_SEG(wraper);
	stek[vel-7]=FP_OFF(wraper);
	stek[vel-8]=0;
	stek[vel-9]=0;
	stek[vel-10]=0;
	stek[vel-11]=0;
	stek[vel-12]=0;
	stek[vel-13]=0;
	stek[vel-14]=0;
	stek[vel-15]=0;
	stek[vel-16]=bp;
//#endif

}

PCB::~PCB(){
	 allPCBs[ID]=0;
	 delete[] stek;
 }

void PCB::oslobodiNiti(){
	while(waitingList!=0){
		waitingList->data->state=ready;
		Scheduler::put(waitingList->data);
		Node*old=waitingList;
		waitingList=waitingList->next;
		delete old;
	}
}

 void PCB::wraper(){
	PCB::running->myThread->run();
	if(PCB::running->myThread->parent!=0) PCB::running->myThread->parent->signal(1);
	PCB::running->myThread->signal(2);
	dispatch();
	lock;
	PCB::running->oslobodiNiti();
	PCB::running->state=finished;
	dispatch();
	unlock;
}

