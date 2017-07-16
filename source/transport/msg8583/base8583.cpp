#include "base8583.h"
#include "common/app_conf.h"
#include "common/string_utility.h"
#include "log/log.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
using namespace std;

Base8583::Base8583()
{
    Init();
}

Base8583::~Base8583()
{

}

/////////////////////////////////////////////////////

int Base8583::getBitmap(int bit_count)
{
	int i, j, k;

	for (i=0; i<bit_count; i++)
	{
		j = i / 8;
		k = i % 8;

		/* whether the item exist in the message */
		if ( m_bitmap[j] & (0x80 >> k ))
		{
			m_bitset_bitmap.set(i);
		}
	}

	return 0;
}
int Base8583::setBitmap(int bit_count)
{
	int i, j;
	unsigned char val;

	if (!m_bitmap.empty())
	{
		m_bitmap.erase();
	}

	if (bit_count > 64) //双位图， 第一位必须为1
	{
		m_bitset_bitmap.set(0);
	}

	for (i=0; i<(bit_count/8); ++i)
	{
		val = 0;
		for (j=0; j<8; j++)
		{
			if(m_bitset_bitmap[i*8 + j])
				val += (unsigned char )(0x80>>j);
		}
		m_bitmap.append(1, (char)val);
	}

	return 0;
}

void Base8583::Init()
{
    m_vec_field.clear();
	m_map_field.clear();
	m_bitset_bitmap.reset();
}

string Base8583::GetMsgType()
{
    return m_msg_type;
}
void Base8583::SetMsgType(const string &msg_type)
{
    m_msg_type = msg_type;
}

string  Base8583::GetMacData()
{
    return m_mac_data;
}

string Base8583::GetHeaderField(int index)
{
    if (index > MAX_HEADER_FILED_COUNT || index > m_vec_field.size())
    {
        return "";
    }

    return m_vec_field[index-1];
}
int Base8583::AddHeaderField(int index, const string &value)
{
    if (index > MAX_HEADER_FILED_COUNT || index > m_vec_field.size())
    {
        return -1;
    }

    m_vec_field[index-1] = value;
    return 0;
}

int Base8583::AddField(unsigned int index, const string &value)
{
	if (index > MAX_ITEM_NUM || value.empty())
	{
		return -1;
	}

	//设置位图标志位
	m_bitset_bitmap.set(index-1);

	m_map_field[index] = value;

	return 0;
}

int Base8583::DelField(unsigned int index)
{
	if (index > MAX_ITEM_NUM)
	{
		return -1;
	}

	//清除位图标志位
	m_bitset_bitmap.reset(index-1);

	m_map_field.erase(index);

	return 0;
}

string Base8583::GetField(unsigned int index)
{
	if (index > MAX_ITEM_NUM)
	{
		return "";
	}

	map<unsigned int, string>::iterator iter = m_map_field.find(index);
	if (iter == m_map_field.end())
	{
		return "";
	}

	return iter->second;
}


