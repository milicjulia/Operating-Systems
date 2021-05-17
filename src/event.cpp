#include "event.h"
#include "KernelEv.h"

Event::Event(IVTNo ivtNO){
myImpl=new KernelEv(ivtNO);
}

void Event::wait(){
	this->myImpl->wait();
}

void Event::signal(){
	this->myImpl->signal();
}

 Event::~Event(){
	 delete myImpl;
}
