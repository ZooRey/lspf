#include "cups8583.h"
#include "common/app_conf.h"
#include "common/string_utility.h"
#include "log/log.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
using namespace std;

//域11与域7、域32和域33的组合值唯一标识一笔交易的编号
const unsigned int key_field_array[] = {7, 11, 32, 33};
//管理类交易的MAC域
const unsigned int mgmt_mac_field_array[] = {7, 11, 39, 53, 70, 100};
//其他类交易的MAC域
const unsigned int other_mac_field_array[] = {2, 3, 4, 7, 11, 18, 25, 28, 32, 33, 38, 39, 41, 42, 90, 102, 103};
//
const unsigned int cups8583_style_array[BITMAP_LEN*8] =
{
/*
**	BULL/ISO Bitmap expression
**  1 9     2 10    3 11    4 12    5 13    6 14    7 15    8 16
*/
    0x0000, 0x8213, 0x0206, 0x020c, 0x020c, 0x020c, 0x020a, 0x0000,
    0x0208, 0x0208, 0x0206, 0x0206, 0x0204, 0x0204, 0x0204, 0x0204,
/*
**  17 25   18 26   19 27   20 28   21 29   22 30   23 31   24 32
*/
    0x0000, 0x0204, 0x0203, 0x0000, 0x0000, 0x0203, 0x0203, 0x0000,
    0x0202, 0x0202, 0x0000, 0x0309, 0x0000, 0x0000, 0x0000, 0x820b,
/*
**  33 41   34 42   35 43   36 44   37 45   38 46   39 47   40 48(0x8700)
*/
    0x820b, 0x0000, 0xa225, 0xa268, 0x030c, 0x0306, 0x0302, 0x0000,
    0x0708, 0x070f, 0x0728, 0x8719, 0x874c, 0x0000, 0x0000, 0x8800,
/*
**  49 57   50 58   51 59   52 60   53 61   54 62   55 63   56 64
*/
    0x0303, 0x0303, 0x0203, 0x0808, 0x0210, 0x8300, 0x88ff, 0x0000,
    0x8764, 0x0000, 0x8700, 0x8764, 0x8700, 0x8700, 0x8700, 0x0000,

/*
**  65 73   66 74   67 75   68 76   69 77   70 78   71 79   72 80
*/
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0203, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,

/*
**  81 89   82 90   83 91   84 92   85 93   86 94   87 95   88 96
*/
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x022a, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0808,

/*
**  97 105  98 106  99 107  100 108  101 109  102 110  103 111  104 112
*/
    0x0000, 0x0000, 0x0000, 0x820b,  0x0000,  0x871e,  0x871e,  0x8800,
    0x0000, 0x0000, 0x0000, 0x0000,  0x0000,  0x0000,  0x0000,  0x0000,

/*
**  113 121  114 122  115 123  116 124  117 125  118 126  119 127  120 128
*/
    0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,
    0x8764,  0x8764,  0x8764,  0x0000,  0x0000,  0x0000,  0x0000,  0x0808
};

CUPS8583::CUPS8583()
{
	m_vec_item_style.assign(cups8583_style_array, cups8583_style_array + BITMAP_LEN*8);
}

CUPS8583::~CUPS8583()
{

}

/////////////////////////////////////////////////////
void CUPS8583::getStyle(ItemStyle &item_style, unsigned int index)
{
	unsigned int tmp_item_style = m_vec_item_style[index];

	item_style.type =(unsigned int)(tmp_item_style / 0x100);
	item_style.max_len = tmp_item_style % 0x100;

	if (item_style.type < 0x80)
	{
		item_style.var = 0;
	}
	else
	{
		if (item_style.max_len == 0)
		{
			item_style.max_len = 999;
		}
		item_style.type -= 0x80;
		/* if LLVAR or LLLVAR */
		item_style.var = ( char ) ((item_style.max_len < 100) ? 2 : 3 );
	}
}

int CUPS8583::decoding(unsigned int index, unsigned int &offset, const ItemStyle &item_style, string &dest)
{
	int i_item_len = 0;
	string item_len;
	string item_value;
	string mac_field_data;

	//获取域长度字符串
	switch (item_style.var)
	{
		case 0:
			item_len = "";
			i_item_len = item_style.max_len;
			break;
		case 2:
			if (offset + 2 > m_content.size())
			{
				return -1;
			}
			item_len.assign(m_content, offset, 2);
			i_item_len = atoi(item_len.c_str());
			offset += 2;
			break;
		case 3:
			if (offset + 3 > m_content.size())
			{
				return -1;
			}
			item_len.assign(m_content, offset, 3);
			i_item_len = atoi(item_len.c_str());
			offset += 3;
			break;
		default:
			item_len = "";
			i_item_len = 0;
			break;
	}

	if (offset + i_item_len > m_content.size())
	{
		return -1;
	}
	item_value.assign(m_content, offset, i_item_len);
	dest = item_value;
	offset += i_item_len;

    if (isMacField(index))
    {
        mac_field_data = item_len + item_value;
        dealMacChar(mac_field_data);
        m_mac_data += string(" ") + mac_field_data;
    }

	return 0;
}

int CUPS8583::encoding(unsigned int index, const string &src, const ItemStyle &item_style, string &dest)
{
	int i_item_len = 0;
	char c_len_buff[3+1] = {0};
	string item_len;
	string item_value;

	//if (item_style.type == _8583_Binary)
	//{
	//	i_item_len = src.size() / 2;
	//}
	//else
	//{
		i_item_len = src.size();
	//}
	if (item_style.max_len < i_item_len)
	{
		return -1;
	}

	//获取域长度字符串
	switch (item_style.var)
	{
		case 2:
			snprintf(c_len_buff, sizeof(c_len_buff), "%02d", i_item_len);
			item_len.assign(c_len_buff);
			break;
		case 3:
			snprintf(c_len_buff, sizeof(c_len_buff), "%03d", i_item_len);
			item_len.assign(c_len_buff);
			break;
		default:
			item_len = "";
			break;
	}

	//根据域类型不同处理ASC或BCD编码
	switch (item_style.type)
	{
		case _8583_Binary:
			//StringUtility::asc2bcd_l(src, item_value);
			item_value = src;
			break;
		case _8583_Numeric:
			item_value = src;
			if (0 == item_style.var) //定长域存在需要补位的情况, 纯数字前补0, 其他后补空格
			{
				if (item_style.max_len > i_item_len)
				{
					item_value.insert(0, item_style.max_len-i_item_len, 0x30);
				}
			}
			break;
		default:
			item_value = src;
			if (0 == item_style.var) //定长域存在需要补位的情况, 纯数字前补0, 其他后补空格
			{
				if (item_style.max_len > i_item_len)
				{
					item_value.insert(i_item_len, item_style.max_len-i_item_len, 0x20);
				}
			}
			break;
	}
	dest = item_len + item_value;

    if (isMacField(index))
    {
        string mac_field_data;
        mac_field_data = dest;
        dealMacChar(mac_field_data);
        if (index == 90) // Original-data-elements:只取前20位数值
        {
            if (mac_field_data.size() > 20)
            {
                m_mac_data += string(" ") + mac_field_data.substr(0, 20);
            }
            else
            {
                m_mac_data += string(" ") + mac_field_data;
            }
        }
        else
        {
            m_mac_data += string(" ") + mac_field_data;
        }
    }

	return 0;
}

bool CUPS8583::isMacField(unsigned int index)
{
	int i;
	int array_len;

	if ("0800" == m_msg_type || "0810" == m_msg_type)
	{
		array_len = sizeof(mgmt_mac_field_array) / sizeof(unsigned int);
		for(i=0; i<array_len; i++)
		{
			if (mgmt_mac_field_array[i] == index)
			{
				return true;
			}
		}
	}
	else
	{
		array_len = sizeof(other_mac_field_array) / sizeof(unsigned int);
		for(i=0; i<array_len; i++)
		{
			if (other_mac_field_array[i] == index)
			{
				return true;
			}
		}
	}

	return false;
}

void CUPS8583::dealMacChar(string &value)
{
	unsigned int i;
	string::size_type position;
	string tmp;

	value = StringUtility::Trim(value); //去掉前后空格

	//多于一个的连续空格，由一个空格代替
	while( (position=value.find("  ")) != string::npos)
	{
		value.replace(position, 2, " ");
	}

	//除了字母(A-Z)，数字(0-9)，空格，逗号(，)和点号(.)以外的字符都删去
	for(i=0; i<value.size(); i++)
	{
		if ( (value[i] >= 'a' and value[i] <= 'z')
			|| (value[i] >= 'A' and value[i] <= 'Z')
			|| (value[i] >= '0' and value[i] <= '9')
			|| (value[i] == 0x20) || (value[i] == ',')  || (value[i] == '.'))
		{
			tmp.insert(tmp.size(), 1, value[i]);
		}
	}

	//所有的小写字母转换成大写字母
	for(i=0; i<tmp.size(); i++)
	{
		if (tmp[i] >= 'a' and tmp[i] <= 'z')
		{
			 tmp[i] += 'A' - 'a';
		}
	}

	value = tmp;
}

void CUPS8583::dealMsgHeader(const string &header)
{
    if (header.size() == HEADER_LEN)
    {
        m_vec_field.push_back(header.substr(0, 1));
        m_vec_field.push_back(header.substr(1, 1));
        m_vec_field.push_back(header.substr(2, 4));
        m_vec_field.push_back(header.substr(6, 11));
        m_vec_field.push_back(header.substr(17, 11));
        m_vec_field.push_back(header.substr(28, 3));
        m_vec_field.push_back(header.substr(31, 1));
        m_vec_field.push_back(header.substr(32, 8));
        m_vec_field.push_back(header.substr(40, 1));
        m_vec_field.push_back(header.substr(41, 5));
    }
}

void CUPS8583::InitHeader()
{
	string src_station_id;
	string dest_station_id;

	src_station_id = AppConf::Instance()->GetConfStr("TransParam", "SrcStationID");
	if (src_station_id.empty())
    {
        src_station_id = "48490000";
    }
	dest_station_id = AppConf::Instance()->GetConfStr("TransParam", "DestStationID");
	if (dest_station_id.empty())
    {
        dest_station_id = "00010000";
    }

    if (src_station_id.size() < 11)
    {
        src_station_id.insert(src_station_id.size(), 11-src_station_id.size(), 0x20);
    }
    else
    {
        src_station_id = src_station_id.substr(0, 11);
    }
    if (dest_station_id.size() < 11)
    {
        dest_station_id.insert(dest_station_id.size(), 11-dest_station_id.size(), 0x20);
    }
    else
    {
        dest_station_id = dest_station_id.substr(0, 11);
    }

    m_vec_field.clear();

    m_vec_field.push_back(string(1, HEADER_LEN));
    m_vec_field.push_back(string(1, HEADER_FLAG_VER));
    m_vec_field.push_back(string(4, 0x30));
    m_vec_field.push_back(dest_station_id);
    m_vec_field.push_back(src_station_id);
    m_vec_field.push_back(string(3, 0x00));
    m_vec_field.push_back(string(1, 0x00));
    m_vec_field.push_back(string(8, 0x30));
    m_vec_field.push_back(string(1, 0x00));
    m_vec_field.push_back(string(5, 0x30));
}

int CUPS8583::Pack(string &msg, bool mac_flag)
{
	int ret = 0;
	string header;
	string item_buff;
	string hex;
	ItemStyle item_style;

	m_content.erase();

	m_mac_data = m_msg_type;

	map<unsigned int, string>::iterator it = m_map_field.begin();
	for (; it!=m_map_field.end(); ++it)
	{
		getStyle(item_style, it->first-1);
		ret = encoding(it->first, it->second, item_style, item_buff);
		if (ret < 0)
		{
			return it->first;
		}

		if (item_style.type == _8583_Binary)
		{
			StringUtility::bcd2asc(it->second, hex);
		}
		else
		{
			hex = it->second;
		}
        if (it->first==2 || it->first==14 || it->first==35 || it->first==36 || it->first==52){
            hex.assign(hex.size(), '*');
        }

        PLOG_INFO("%03d:%s", it->first, hex.c_str());
		m_content += item_buff;
	}

	//计算位图数据
	if (mac_flag)
    {
        m_bitset_bitmap.set(BITMAP_LEN*8-1);
        m_content += string(MAC_VALUE_LEN, 0x00);
    }
	setBitmap(BITMAP_LEN*8);

	if (m_vec_field.size() != HEADER_FIELD_COUNT)
    {
        PLOG_ERROR("msg header is invalid, header field size=%d", (int)m_vec_field.size());
        return -1;
    }
    //msg_len + msg_hread + msg_type + bitmap + msg_content
	char total_msg_len[MSG_LEN+1] = {0};
	snprintf(total_msg_len, sizeof(total_msg_len), "%04d", HEADER_LEN+MTI_LEN+BITMAP_LEN+m_content.size());
    m_vec_field[2] = string(total_msg_len);

    for(int i=0; i<m_vec_field.size(); i++)
    {
        header += m_vec_field[i];
    }

	msg = string(total_msg_len) + header + m_msg_type + m_bitmap + m_content;
    StringUtility::bcd2asc(msg, hex);
    PLOG_INFO("msg:%s", hex.c_str());

	return 0;
}


int CUPS8583::Unpack(const string &msg)
{
	int ret = 0;
	unsigned int offset = 0;
	string reject_code;
	string item_value;
	string log_hex;
	string msg_len;
	string header;
	ItemStyle item_style;

    if (msg.size() <= MSG_LEN+HEADER_LEN+MTI_LEN+BITMAP_LEN)
    {
        PLOG_ERROR("msg len is invalid, len=%d", (int)msg.size());
        return -1;
    }
    msg_len = msg.substr(0, MSG_LEN);

    header = msg.substr(MSG_LEN, HEADER_LEN);
    dealMsgHeader(header);
    if (m_vec_field.size() != HEADER_FIELD_COUNT)
    {
        StringUtility::bcd2asc(header, log_hex);
        PLOG_ERROR("msg header is invalid, msg header=%s, header field size=%d", log_hex.c_str(), (int)m_vec_field.size());
        return -1;
    }
    reject_code = m_vec_field[9];

    PLOG_INFO("reject_code:%s", reject_code.c_str());
    if (reject_code != "00000") //有拒绝码,表示还有一个报文头(原始报文头)
    {
        offset = MSG_LEN+HEADER_LEN+HEADER_LEN;
    }
    else
    {
        offset = MSG_LEN+HEADER_LEN;
    }

    m_msg_type = msg.substr(offset, MTI_LEN);
    PLOG_DEBUG("msg type:%s", m_msg_type.c_str());

    m_bitmap = msg.substr(offset+MTI_LEN, BITMAP_LEN);
    StringUtility::bcd2asc(m_bitmap, log_hex);
    PLOG_DEBUG("msg bitmap:%s", log_hex.c_str());

    m_content = msg.substr(offset+MTI_LEN+BITMAP_LEN);
    StringUtility::bcd2asc(m_content, log_hex);
    PLOG_DEBUG("msg content:%s", log_hex.c_str());

    m_mac_data = m_msg_type;

    if (getBitmap(BITMAP_LEN*8) != 0)
    {
        return -1;
    }

	//解包消息内容
	offset = 0;
	for (unsigned int i=1; i<BITMAP_LEN*8; ++i)
	{
		if (m_bitset_bitmap[i] == 1)
		{
			getStyle(item_style, i);

			ret = decoding(i+1, offset, item_style, item_value);
			if (ret < 0)
			{
			    PLOG_ERROR("unpack %d field failed", i+1);
				return i+1;
			}
			if (item_style.type == _8583_Binary)
			{
				StringUtility::bcd2asc(item_value, log_hex);
			}
			else
			{
				log_hex = item_value;
			}

            if (i==1 || i==13 || i==34 || i==35 ||i==51){
                log_hex.assign(log_hex.size(), '*');
            }

            PLOG_INFO("%03d:%s", i+1, log_hex.c_str());

			m_map_field.insert(make_pair(i+1, item_value));
			item_value.erase();
		}
	}

	return 0;
}

string CUPS8583::GetMsgKey()
{
	int array_len;
	string key;
	map<unsigned int, string>::iterator iter;

	array_len = sizeof(key_field_array) / sizeof(unsigned int);
	for(int i=0; i<array_len; i++)
	{
		iter = m_map_field.find(key_field_array[i]);
		if (iter != m_map_field.end())
		{
			key += iter->second;
		}
	}

	return key;
}

