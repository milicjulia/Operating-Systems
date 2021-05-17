#include <iostream.h>
#include <dos.h>
#include "KlSem.h"
#include "Thread.h"
#include "PCB.h"
#include "PetljaN.h"
#include "semaphor.h"
#include "SCHEDULE.H"
#include "intLock.h"

#define lock asm cli
#define unlock asm sti

class PCB;
class Thread;
class PetljaNit;
class KernelSem;

typedef void interrupt (*pInterrupt)(...);

pInterrupt oldISR;
volatile int cnt = 50;
int inter_demand=0,notforbiden=1;
volatile unsigned tsp,tss,signali=0;

void interrupt timer(...) {

	if(!inter_demand){
		if(Thread::timerQueueHead) Thread::timerQueueHead->time--;
		while (Thread::timerQueueHead && Thread::timerQueueHead->time==0) {
		 if(Thread::timerQueueHead->pcb->state==blocked){
			 Thread::timerQueueHead->pcb->state=ready;
			 Thread::timerQueueHead->pcb->blk=0;
			 Thread::timerQueueHead->pcb->myKernelSem->deletePCB( Thread::timerQueueHead->pcb);
			 Scheduler::put(Thread::timerQueueHead->pcb);
		 }
		 Thread::timerQueueHead=Thread::timerQueueHead->next;

		}
	}
	//#ifndef BCC_BLOCK_IGNORE
	if(!inter_demand) cnt--;

	if((cnt<=0 && notforbiden) || inter_demand){
	asm{
		mov tsp,sp
		mov tss,ss
	}
	if(signali==0){//ako nije radjen signal
		PCB::running->sp=tsp;
		PCB::running->ss=tss;
	}
	signali=0;
	if(PCB::running->state!=blocked && PCB::running->ID!=1 && PCB::running->state!=finished){
		Scheduler::put(PCB::running);
	}

	PCB::running=Scheduler::get();
	if(PCB::running==0) PCB::running=PCB::allPCBs[1];
	else PCB::running->state=running1;

	if(PCB::running->myThread->requests!=0){
		notforbiden=0;
		tsp=0x200;
		signali=1;
		asm{
			push tsp
		}
		tsp=Thread::CS;
		tss=Thread::IP;
		asm{
			push tsp
			push tss
			push tsp
			push tss
			push tsp
			push tss
			push tsp
			push tss
			push tsp
			push tss
			push tss
		}
	if(!inter_demand) asm int 60h;
	inter_demand=0;
	return;
	}

	tsp=PCB::running->sp;
	tss=PCB::running->ss;
	cnt=PCB::running->time;

	if(cnt==0) notforbiden=0;
	else notforbiden=1;

	asm{
		mov sp,tsp
		mov ss,tss
	}
	}

if(!inter_demand) asm int 60h;
	inter_demand=0;
//#endif
}

unsigned oldTimerOFF, oldTimerSEG;

void inic(){
	PCB::running=(new Thread(32,0))->myPCB;
	asm{
		cli
		push es
		push ax

		mov ax,0	// inicijalizuje rutinu za tajmer
		mov es,ax	// es = 0

		mov ax, word ptr es:0022h   // oldTimerSEG = [00072h]; - 1Ch*4h+2h=72h
		mov word ptr oldTimerSEG, ax

		mov ax, word ptr es:0020h   // oldTimerOFF = [00072h]; - 1Ch*4h=70h
		mov word ptr oldTimerOFF, ax

		// postavlja novu rutinu
		mov word ptr es:0022h, seg timer
		mov word ptr es:0020h, offset timer


		mov ax, word ptr oldTimerSEG
				mov word ptr es:0182h, ax
				mov ax, word ptr oldTimerOFF
				mov word ptr es:0180h, ax

		pop ax
		pop es
		sti
	}

}

void restore(){
	asm {
		cli
		push es
		push ax

		mov ax,0
		mov es,ax

		mov ax, word ptr oldTimerSEG
		mov word ptr es:0022h, ax
		mov ax, word ptr oldTimerOFF
		mov word ptr es:0020h, ax

		pop ax
		pop es
		sti
	}

}

extern int userMain(int argc, char* argv[]);
unsigned ta,tb,tc,td,t1,t2,t3,t4,t5,t6,t7,t8;

void main(){

	inic();
	PetljaN p;

	char  *argumenti[4];
	argumenti[0]="javni.exe";
	argumenti[1]="10";
	argumenti[2]="10";
	argumenti[3]="0";
	userMain(4,argumenti);


	dispatch();
	restore();

	TimerRequest *old=Thread::firsttimerQueueHead;
	int count=0;
	while(old!=0 &&count++<500){
		Thread::firsttimerQueueHead=Thread::firsttimerQueueHead->next;
		delete old;
		TimerRequest *old=Thread::firsttimerQueueHead;
	}

	delete PCB::running;

return;
}
