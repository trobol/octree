#ifndef _CONTAINERS_SINGLETON_H
#define _CONTAINERS_SINGLETON_H


//https://www.theimpossiblecode.com/blog/c11-generic-singleton-pattern/

template <typename T>
class Singleton
{
public:
    // Very fast inlined operator accessors
    T* operator->() { return mpInstance; }
    const T* operator->() const { return mpInstance; }
    T& operator*() { return *mpInstance; }
    const T& operator*() const { return *mpInstance; }

 
protected:
    Singleton()
    {
        static bool static_init = []()->bool {
            mpInstance = new T;
            return true;
        }();
    }



private:
    static T* mpInstance;
};

template <typename T>
T* Singleton<T>::mpInstance;

#endif 
