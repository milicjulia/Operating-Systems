#include "KlSem.h"
#include "PCB.h"
#include "SCHEDULE.H"
#include<IOSTREAM.H>
extern void dispatch();

KernelSem::KernelSem(int v){
	val=v;
	first=0;
}

int KernelSem::signal(){
int ret=0;
if(val++<0) {
	unblock();
	ret=1;
}
return ret;
}

int KernelSem::wait(){
int ret=1;
if(--val<0) ret=block();
return ret;
}

int KernelSem::block(){
Blokirani *node=new Blokirani(PCB::running);

node->data->state=blocked;
node->data->myKernelSem=this;
if(first==0) first=node;
else{
	Blokirani *tmp=first;
	while(tmp){
		if(tmp->next==0){tmp->next=node; break;}
		else tmp=tmp->next;
	}
}
dispatch();
return PCB::running->blk;
}

void KernelSem::unblock(){
Blokirani *node=first;
first=first->next;
node->data->state=ready;
node->data->blk=1;

Scheduler::put(node->data);
}


void KernelSem::deletePCB(PCB* pcb){
	Blokirani *tmp=first;
	Blokirani *old=0;
	while(tmp!=0){
		if(tmp->data->ID==pcb->ID){
			if(old==0) first=first->next;
			else {
				old->next=tmp->next;

				delete tmp;

			}
val++;
			return;
		}
		old=tmp;
	tmp=tmp->next;

	}
}

KernelSem::~KernelSem(){
	while(first){
		Blokirani *blk=first;
		first=first->next;
		blk->data=0;
		blk->next=0;
		blk=0;
	}
}
