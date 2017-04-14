#ifndef LSPF_THREADLOCAL_H
#define LSPF_THREADLOCAL_H

#include <boost/noncopyable.hpp>
#include <assert.h>
#include <pthread.h>

template<typename T>
class ThreadLocal : boost::noncopyable
{
public:

	static T* Instance()
	{
		if (t_value == NULL)
		{
			t_value = new T();
			deleter.Set(t_value);
		}
		
		return t_value;
	}

private:
	ThreadLocal();
	~ThreadLocal();

	static void Destructor(void* obj)
	{
		assert(obj == t_value);
		
		typedef char Type_must_be_complete[sizeof(T) == 0 ? -1 : 1];
		(void) sizeof(Type_must_be_complete);
		
		delete t_value;
		t_value = NULL;
	}

	class Deleter
	{
	public:
		Deleter()
		{
			pthread_key_create(&pkey, &ThreadLocal::Destructor);
		}

		~Deleter()
		{
			pthread_key_delete(pkey);
		}

		void Set(T* newObj)
		{
			assert(pthread_getspecific(pkey) == NULL);
			pthread_setspecific(pkey, newObj);
		}

		pthread_key_t pkey;
	};

	static __thread T* t_value;
	static Deleter deleter;
};

template<typename T>
__thread T* ThreadLocal<T>::t_value = NULL;

template<typename T>
typename ThreadLocal<T>::Deleter ThreadLocal<T>::deleter;

#endif
