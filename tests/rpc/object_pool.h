#ifndef __OBJECT_POOL_H__
#define __OBJECT_POOL_H__

#include <list>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
 
template<typename Object>
class ObjectPool
{
public:
	typedef boost::shared_ptr<Object> ObjectPtr;

    ObjectPool(size_t size) : m_size(size)
    {
        for (size_t i = 0; i < m_size; ++i)
		{
			m_pool.push_back(boost::make_shared<Object>());
		}
    }

    ~ObjectPool()
    {
		m_pool.clear();
		m_size = 0;
    }
 
    ObjectPtr GetObject()
    {
        if (0 == m_size)
        {
            return boost::make_shared<Object>();
        }
        else
        {
            ObjectPtr object = m_pool.front();
            m_pool.pop_front();
            -- m_size;
			return object;
        }
    }
 
    void ReturnObject(ObjectPtr pObj)
    {
        m_pool.push_back(pObj);
        ++ m_size;
    }
 
private:
 
    size_t m_size;
 
    std::list<ObjectPtr> m_pool;
};

#endif