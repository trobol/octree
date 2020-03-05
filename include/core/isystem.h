#ifndef _CORE_SYSTEM_H
#define _CORE_SYSTEM_H


struct Engine;

class ISystem {
public:
	virtual void Startup() = 0;
	virtual void Shutdown() = 0;
	virtual void Update(float dt) = 0;
private:
	ISystem();
	~ISystem();
	friend Engine;
};

#endif // !_CORE_SYSTEM_H
