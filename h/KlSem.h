#ifndef _kernelsem_h_
#define _kernelsem_h_
typedef unsigned int Time;

#define lock asm cli
#define unlock asm sti
class PCB;

class KernelSem{
	struct Blokirani{
		Blokirani* next;
		PCB*  data;
		Blokirani(PCB* d){
			data=d;
			next=0;
		}
		~Blokirani(){
			data=0;
						next=0;
		}
		};
private:
 Blokirani *first;

public:
int val;
KernelSem(int v);
int signal();
int wait();
int block();
void unblock();
void deletePCB(PCB* pcb);
  ~KernelSem();


};
#endif
