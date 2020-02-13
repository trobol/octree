#ifndef _SYSTEMS_SYSTEM_H
#define _SYSTEMS_SYSTEM_H

#include "../containers/singleton.h"

template <typename T>
class System : Singleton<T> {
public:
	
	virtual void Update(float deltaTime) = 0;
	virtual void Startup(void) = 0;
	virtual void Shutdown(void) = 0;
	virtual ~System() {}

};



#endif