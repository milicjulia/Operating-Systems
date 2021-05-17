#ifndef _event_h_
#define _event_h_

typedef unsigned char IVTNo;
class KernelEv;

class Event {
private:
 KernelEv* myImpl;

public:
 Event (IVTNo ivtNo);
 ~Event ();
 void wait ();

protected:
 friend class KernelEv;
 void signal(); // can call KernelEv

};

#define PREPAREENTRY(numEntry, callOld)\
	void interrupt inter##numEntry(...); \
	IVTEntry newEntry##numEntry(numEntry, inter##numEntry); \
	void interrupt inter##numEntry(...) {\
		newEntry##numEntry.signal();\
		if (callOld == 1)\
		newEntry##numEntry.proslaRutina();\
	}
#endif
