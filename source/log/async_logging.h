#ifndef __ASYNC_LOGGING_H__
#define __ASYNC_LOGGING_H__

#include "common/mutex.h"
#include "common/condition_variable.h"

#include <boost/scoped_ptr.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include <string>
#include <vector>

//using namespace std;

template<size_t SIZE>
class LogBuffer
{
public:
	LogBuffer()
	:m_max_element_count(SIZE)
	{
		m_data.reserve(m_max_element_count);
	}
	~LogBuffer(){}
public:
	int Size()
	{
		return m_data.size();
	}
	int Avail()
	{
		return static_cast<int>(m_max_element_count - m_data.size());
	}
	void Reset()
	{
		m_data.clear();
	}
	void Append(const std::string &log)
	{
		m_data.push_back(log);
	}
	void Data(std::string &data)
	{
		data = "";
		for(size_t i; i<m_data.size(); i++)
		{
			data += m_data[i] + "\n";
		}
	}

private:
	size_t m_max_element_count;
	std::vector<std::string> m_data;
};

class AsyncLogging
{
public:
	static AsyncLogging* Instance();

	int Init(int flush_interval=3000);
	void Append(const std::string &log);
	void Data(std::string &data);
private:
	AsyncLogging()
	:m_flush_interval(3000),
	m_mutex(),
	m_condition(),
	m_curr_buffer(new Buffer),
	m_next_buffer(new Buffer),
	m_buffers()
	{

	}
private:
	typedef LogBuffer<2000> Buffer;
	typedef boost::ptr_vector<Buffer> BufferVector;
	typedef BufferVector::auto_type BufferPtr;

	int m_flush_interval;
	TMutex m_mutex;
    ConditionVariable m_condition;

	BufferPtr m_curr_buffer;
	BufferPtr m_next_buffer;
	BufferVector m_buffers;

	static AsyncLogging* instance_;
};

#endif //__ASYNC_LOGGING_H__
