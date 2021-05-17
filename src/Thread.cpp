#include"Thread.h"
#include"PCB.h"
#include "SCHEDULE.H"
#include "semaphor.h"
#include <dos.h>

unsigned Thread::CS=0;
unsigned Thread::IP=0;
TimerRequest* Thread::timerQueueHead=0;
TimerRequest* Thread::firsttimerQueueHead=0;
int Thread::globalSig[16]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

Thread::Thread(StackSize stackSize , Time timeSlice ){
	if(PCB::running!=0) parent=PCB::running->myThread; else parent=0;
	for(int i=0;i<16;i++){
		localSig[i]=1;
		postpSig[i]=0;
		functions[i]=0;
	}
	 requests=0;
	 CS=FP_SEG(obradaSig);
	 IP=FP_OFF(obradaSig);
	 registerHandler(0,nultiSig);
	 myPCB = new PCB(stackSize,timeSlice,this);
 }

Thread::~Thread(){ delete myPCB; }

ID Thread::getId(){return myPCB->ID;};

ID Thread::getRunningId(){ return PCB::running->myThread->getId();};

 Thread * Thread::getThreadById(ID id){ return PCB::allPCBs[id]->myThread; }

 void Thread::start(){
 	 myPCB->state=ready;
 	 Scheduler::put(this->myPCB);
  }

 void Thread::run() {}

void Thread::waitToComplete(){
	if(PCB::running==this->myPCB) return;
	lock;
	if(myPCB->state!=finished){
		Node* cvor=new Node(PCB::running);
		cvor->next=myPCB->waitingList;
		myPCB->waitingList=cvor;
		PCB:: running->state=blocked;
		dispatch();
	}
	unlock;
}

void Thread::sleep(int t){
	 TimerRequest* nov=new TimerRequest();
	 nov->pcb=PCB::running;
	 nov->pcb->state=blocked;
	 PCB::running->state=blocked;
	if (timerQueueHead==0) {
		timerQueueHead=nov;
		nov->time=t;
		if(firsttimerQueueHead==0) firsttimerQueueHead=timerQueueHead;
		return;
	}
	int pom=0;
	 TimerRequest* prev=timerQueueHead;
	 TimerRequest* pret=0;
	 while (prev) {
		 pom+=prev->time;
		if((t-pom)>0) {
			pret=prev;
			prev=prev->next;
			if(prev==0){
				pret->next=nov;
				nov->time=t-pom;
				return;
			}
			continue;
		}
		else if((t-pom)==0){
			 nov->next=prev->next;
			 prev->next=nov;
			 nov->time=0;
			 prev=nov->next;
			 return;
			}
		else if((t-pom)<0){
			 nov->next=prev;
			 if(pret){
			 pret->next=nov;
			 pom-=prev->time;
			 nov->time=t-pom;
			 prev=nov->next;}
			 else{
				 timerQueueHead=nov;
				 if(prev==firsttimerQueueHead)firsttimerQueueHead=nov;
			nov->time=t;

			 }}
		prev=nov->next;
		pret=nov;
		prev->time-=pret->time;
		return;

	 }
}


void Thread::blockSignal(SignalId signal){ localSig[signal]=0; }

 void Thread::blockSignalGlobally(SignalId signal){ globalSig[signal]=0; }

 void Thread::unblockSignal(SignalId s){
	 if(localSig[s]==1) return;
	 localSig[s]=1;
	 if(globalSig[s]==1) {
		while(postpSig[s]--)signal(s);
	 }
 }

 void Thread::unblockSignalGlobally(SignalId s){
	 if(globalSig[s]==1) return;
	 else globalSig[s]=1;
	 for(int i=0;i<PCB::IDs;i++){
		 if(PCB::allPCBs[i]->myThread->localSig[s]==1)
			 while(PCB::running->myThread->postpSig[s]-->0);
			 PCB::allPCBs[i]->myThread->signal(s);
	 }
 }

 void Thread::registerHandler(SignalId s, SignalHandler handler){
	 ListFunc *pom=functions[s];
	 ListFunc *nov=new ListFunc(handler);
	 if(pom==0){
		 functions[s]=nov;
		 return;
	 }
	 while(pom->next) pom=pom->next;
	 pom->next=nov;
 }


 void Thread::signal(SignalId signal){
	 if(localSig[signal]==1 && globalSig[signal]==1){
		 if(functions[signal]==0)return;
		 ListInt *nov=new ListInt(signal);
		 ListInt *pom=requests;
		 if(pom==0){
			 requests=nov;
			 return;
		 }
		 while(pom->next) pom=pom->next;
		 pom->next=nov;
	 }
	 else postpSig[signal]++;
}


 void Thread::obradaSig(){
	 ListInt *pom=PCB::running->myThread->requests;
	 while(pom){
		 ListFunc *f=PCB::running->myThread->functions[pom->x];
		 while(f!=0){
			 f->func();
			 f=f->next;
		 }
		 pom=pom->next;
	 }
	 PCB::running->myThread->requests=0;
	 dispatch();
}

 void Thread::swap(SignalId id, SignalHandler hand1, SignalHandler hand2){
	 ListFunc *f1=functions[id];
	 ListFunc *f2=functions[id];
	 while(f1->func!=hand1) f1=f1->next;
	 while(f2->func!=hand2) f2=f2->next;
	 if(f1==0 || f2==0) return;
	 f1->func=hand2;
	 f2->func=hand1;
 }

 void Thread::unregisterAllHandlers(SignalId id){
	 ListFunc *f=functions[id];
	 functions[id]=0;
	 delete f;
 }

 void dispatch(){
 	asm cli
 	inter_demand=1;
 	timer();
 	asm sti
 }

 void nultiSig(){
 	if(PCB::running->myThread->parent!=0) PCB::running->myThread->parent->signal(1);
 	PCB::running->myThread->signal(2);
 	PCB::running->oslobodiNiti();
 	PCB::running->state=finished;
 }
