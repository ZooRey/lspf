#include "async_logging.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>
using namespace std;

AsyncLogging* AsyncLogging::instance_ = NULL;

AsyncLogging* AsyncLogging::Instance()
{
    if (instance_ == NULL){
        instance_ =  new AsyncLogging();
    }
    return instance_;
}

int AsyncLogging::Init(int flush_interval)
{
    if (flush_interval == 0){
        flush_interval = 3000;
    }

	m_flush_interval = flush_interval;
	m_buffers.reserve(16);	//reserve是容器预留空间，但在空间内不真正创建元素对象，所以在没有添加新的对象之前，不能引用容器内的元素

	return 0;
}

void AsyncLogging::Append(const std::string &log)
{
	AutoLocker locker(&m_mutex);
	if (m_curr_buffer->Avail() > 0)
	{
		m_curr_buffer->Append(log);
	}
	else
	{
		m_buffers.push_back(m_curr_buffer.release());

		if (m_next_buffer)
		{
			m_curr_buffer = boost::ptr_container::move(m_next_buffer);
		}
		else
		{
			m_curr_buffer.reset(new Buffer); // Rarely happens
		}
		m_curr_buffer->Append(log);
		m_condition.Broadcast();
	}
}

void AsyncLogging::Data(std::string &data)
{
	char buf[256] = {0};
	std::string tmp;
	BufferPtr new_buffer1(new Buffer);
	BufferPtr new_buffer2(new Buffer);
	BufferVector buffers_write;
	buffers_write.reserve(16);

	{
		AutoLocker locker(&m_mutex);
		if (m_buffers.empty())  // unusual usage!
		{
			m_condition.TimedWait(&m_mutex, m_flush_interval);
		}
		m_buffers.push_back(m_curr_buffer.release());
		m_curr_buffer = boost::ptr_container::move(new_buffer1);
		buffers_write.swap(m_buffers);
		if (!m_next_buffer)
		{
			m_next_buffer = boost::ptr_container::move(new_buffer2);
		}
	}

	if (buffers_write.size() > 25)
	{
		snprintf(buf, sizeof(buf), "Dropped log messages, %zd larger buffers\n",
               buffers_write.size()-2);
		buffers_write.erase(buffers_write.begin()+2, buffers_write.end());
    }

	for (size_t i = 0; i < buffers_write.size(); ++i)
	{
		buffers_write[i].Data(tmp);
		data += tmp;
	}
	data += std::string(buf);

	if (buffers_write.size() > 2)
	{
		// drop non-bzero-ed buffers, avoid trashing
		buffers_write.resize(2);
	}

	if (!new_buffer1)
	{
		new_buffer1 = buffers_write.pop_back();
		new_buffer1->Reset();
	}

	if (!new_buffer2)
	{
		new_buffer2 = buffers_write.pop_back();
		new_buffer2->Reset();
	}

	buffers_write.clear();
}







