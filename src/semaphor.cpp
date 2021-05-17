#include "KlSem.h"
#include "PCB.h"
#include "semaphor.h"
#include "Thread.h"

Semaphore::Semaphore(int init){
	 myImpl=new KernelSem(init);
 }

int Semaphore::wait (Time maxTimeToWait){
	lock;
	int ret=0;
	if(myImpl->val<=0){
		if(maxTimeToWait>0)
		PCB::running->myThread->sleep(maxTimeToWait);
		int ret=myImpl->wait();
	}
	else ret=myImpl->wait();
	unlock;
	return ret;
}

int Semaphore::val()const{ return myImpl->val; }

int Semaphore::signal(int n){
	int ret=0;
	lock;
	if(n>=2){
		for(int i=0;i<n;i++) ret+=myImpl->signal();
	}
	else{
		ret=myImpl->signal();
		ret=0;
	}
	unlock;
	return ret;
}

Semaphore::~Semaphore(){ delete myImpl;}
