#include "iso8583.h"
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
//
const unsigned int iso8583_style_array[BITMAP_LEN*8] =
{
/*
**    BULL/ISO Bitmap expression
**   1 9     2 10    3 11    4 12    5 13    6 14    7 15    8 16
*/
    0x0000, 0xa213, 0x0206, 0x020c, 0x020c, 0x020c, 0x020a, 0x0701,
    0x0208, 0x0208, 0x0206, 0x0206, 0x0204, 0x0204, 0x0204, 0x0204,
/*
**   17 25   18 26   19 27   20 28   21 29   22 30   23 31   24 32
*/
     0x0204, 0x0204, 0x0203, 0x0203, 0x0203, 0x0203, 0x0204, 0x0203,
    0x0202, 0x0202, 0x0201, 0x0308, 0x0308, 0x0308, 0x0208, 0x820b,
/*
**   33 41   34 42  35 43    36 44   37 45   38 46   39 47   40 48
*/
    0x820b, 0x861c, 0xa225, 0xa268, 0x030c, 0x0306, 0x0302, 0x0303,
    0x0708, 0x070f, 0x0728, 0x8719, 0x874c, 0x8700, 0x8700, 0x8200,
/*
**   49 57   50 58   51 59   52 60   53 61   54 62   55 63   56 64
*/
    0x0303, 0x0203, 0x0203, 0x0808, 0x0210, 0x83b4, 0x8800, 0x8700,
    0x8700, 0x8700, 0x8700, 0x8200, 0x8200, 0x8800, 0x87b3, 0x0308
};

ISO8583::ISO8583()
{
    m_vec_item_style.assign(iso8583_style_array, iso8583_style_array + BITMAP_LEN*8);
}

ISO8583::~ISO8583()
{

}

string ISO8583::GetInfo()
{
    return "ISO8583V20170713";
}
/////////////////////////////////////////////////////
void ISO8583::getStyle(ItemStyle &item_style, unsigned int index)
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

int ISO8583::decoding(unsigned int index, unsigned int &offset, const ItemStyle &item_style, string &dest)
{
    int i_item_len = 0;
    int i_item_len_bcd = 0;
    string item_len_bcd;
    string item_len;
    string item_value;
    string mac_field_data;

    //获取域长度字符串
    switch (item_style.var)
    {
        case 0:
            i_item_len = item_style.max_len;
            break;
        case 2:
            if (offset + 1 > m_content.size())
            {
                return -1;
            }
            item_len_bcd.assign(m_content, offset, 1);
            StringUtility::bcd2asc(item_len_bcd, item_len);
            i_item_len = atoi(item_len.c_str());
            offset += 1;
            break;
        case 3:
            if (offset + 2 > m_content.size())
            {
                return -1;
            }
            item_len_bcd.assign(m_content, offset, 2);
            StringUtility::bcd2asc(item_len_bcd, item_len);
            i_item_len = atoi(item_len.c_str());
            offset += 2;
            break;
        default:
            i_item_len = 0;
            break;
    }

    if (i_item_len > item_style.max_len)
    {
        return -1;
    }

    if (offset + i_item_len/2 + i_item_len%2 > m_content.size())
    {
        return -1;
    }

    switch (item_style.type)
    {
        case _8583_Binary:
            i_item_len_bcd = i_item_len;
            item_value.assign(m_content, offset, i_item_len_bcd);
            dest = item_value;
            offset += i_item_len_bcd;
            break;
        case _8583_Numeric:
        case _8583_Z:
            i_item_len_bcd = i_item_len/2 + i_item_len%2;
            item_value.assign(m_content, offset, i_item_len_bcd);
            StringUtility::bcd2asc(item_value, dest);
            if (i_item_len%2 != 0)
            {
                dest.erase(i_item_len);
            }
            offset += i_item_len_bcd;
            break;
        case _8583_RightBcd:
            i_item_len_bcd = i_item_len/2 + i_item_len%2;
            item_value.assign(m_content, offset, i_item_len_bcd);
            StringUtility::bcd2asc(item_value, dest);
            if (i_item_len%2 != 0)
            {
                dest = dest.substr(1);
            }
            offset += i_item_len_bcd;
            break;
        default:
            i_item_len_bcd = i_item_len;
            dest.assign(m_content, offset, i_item_len_bcd);
            offset += i_item_len_bcd;
            break;
    }

    return 0;
}

int ISO8583::encoding(unsigned int index, const string &src, const ItemStyle &item_style, string &dest)
{
    int i_item_len = 0;
    char c_len_buff[4+1] = {0};
    string item_len;
    string item_len_bcd;
    string item_value;
    string item_value_bcd;

    i_item_len = src.size();
    if (item_style.max_len < i_item_len)
    {
        return -1;
    }

    //获取域长度字符串
    switch (item_style.var)
    {
        case 0:
            if (item_style.max_len != i_item_len)
            {
                return -1;
            }
            item_len = "";
            break;
        case 2:
            snprintf(c_len_buff, sizeof(c_len_buff), "%02d", i_item_len);
            item_len.assign(c_len_buff);
            StringUtility::asc2bcd_r(item_len, item_len_bcd);
            item_len = item_len_bcd;
            break;
        case 3:
            snprintf(c_len_buff, sizeof(c_len_buff), "%04d", i_item_len);
            item_len.assign(c_len_buff);
            StringUtility::asc2bcd_r(item_len, item_len_bcd);
            item_len = item_len_bcd;
            break;
        default:
            item_len = "";
            break;
    }

    //根据域类型不同处理ASC或BCD编码
    switch (item_style.type)
    {
        case _8583_Binary:
            item_value = src;
            break;
        case _8583_Numeric:
        case _8583_Z:
            StringUtility::asc2bcd_l(src, item_value);
            break;
        case _8583_RightBcd:
            StringUtility::asc2bcd_r(src, item_value);
            break;
        default:
            item_value = src;
            break;
    }
    dest = item_len + item_value;

    return 0;
}

void ISO8583::dealMsgHeader(const string &header)
{
    if (header.size() == HEADER_LEN)
    {
        m_vec_field.push_back(header.substr(0, 2));
        m_vec_field.push_back(header.substr(2, 4));
        m_vec_field.push_back(header.substr(6, 4));
        m_vec_field.push_back(header.substr(10, 2));
        m_vec_field.push_back(header.substr(12, 2));
        m_vec_field.push_back(header.substr(14, 2));
        m_vec_field.push_back(header.substr(16, 1));
        m_vec_field.push_back(header.substr(17, 1));
        m_vec_field.push_back(header.substr(18, 6));
    }
}

void ISO8583::InitHeader()
{
    m_vec_field.clear();

    m_vec_field.push_back(HEADER_ID);
    m_vec_field.push_back(string(4, 0x30));
    m_vec_field.push_back(string(4, 0x30));
    m_vec_field.push_back(string(2, 0x30));
    m_vec_field.push_back(string(2, 0x30));
    m_vec_field.push_back(string(2, 0x30));
    m_vec_field.push_back(string(1, 0x30));
    m_vec_field.push_back(string(1, 0x30));
    m_vec_field.push_back(string(6, 0x30));
}

int ISO8583::Pack(string &msg, bool mac_flag)
{
    int ret = 0;
    string header;
    string header_bcd;
    string msg_type_bcd;
    string item_buff;
    string hex;
    ItemStyle item_style;

    m_content.erase();

    StringUtility::asc2bcd_l(m_msg_type, msg_type_bcd);
    m_mac_data = msg_type_bcd;

    //计算位图数据
    if (mac_flag)
    {
        m_bitset_bitmap.set(BITMAP_LEN*8-1);
    }

    setBitmap(BITMAP_LEN*8);

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
        PLOG_INFO("%03d:%s", it->first, hex.c_str());
        m_content += item_buff;
    }

    //计算位图数据
    if (mac_flag)
    {
        m_bitset_bitmap.set(BITMAP_LEN*8-1);
    }
    setBitmap(BITMAP_LEN*8);

    m_mac_data += m_bitmap + m_content;

    if (mac_flag)
    {
        m_content += string(8, 0x00);
    }

    if (m_vec_field.size() != HEADER_FIELD_COUNT)
    {
        PLOG_ERROR("msg header is invalid, header field size=%d", (int)m_vec_field.size());
        return -1;
    }
    //msg_len + msg_hread + msg_type + bitmap + msg_content
    char total_msg_len[MSG_LEN+1] = {0};
    total_msg_len[0] = (HEADER_LEN/2+MTI_LEN/2+BITMAP_LEN+m_content.size()) / 256;
    total_msg_len[1] = (HEADER_LEN/2+MTI_LEN/2+BITMAP_LEN+m_content.size()) % 256;

    for(unsigned int i=0; i<m_vec_field.size(); i++)
    {
        header += m_vec_field[i];
    }

    StringUtility::asc2bcd_l(header, header_bcd);
    StringUtility::asc2bcd_l(m_msg_type, msg_type_bcd);

    msg = string(total_msg_len, MSG_LEN) + header_bcd + msg_type_bcd + m_bitmap + m_content;
    StringUtility::bcd2asc(msg, hex);
    PLOG_INFO("msg:%s", hex.c_str());

    return 0;
}


int ISO8583::Unpack(const string &msg)
{
    int ret = 0;
    int len = 0;
    unsigned int offset = 0;
    string msg_type_bcd;
    string item_value;
    string log_hex;
    string msg_len;
    string header;
    string header_bcd;
    ItemStyle item_style;

    if (msg.size() <= MSG_LEN+HEADER_LEN/2+MTI_LEN/2+BITMAP_LEN)
    {
        PLOG_ERROR("msg len is invalid, len=%d", (int)msg.size());
        return -1;
    }
    msg_len = msg.substr(0, MSG_LEN);
    len = (unsigned char)msg_len[0] * 256 + (unsigned char)msg_len[1];

    header_bcd = msg.substr(MSG_LEN, HEADER_LEN/2);
    StringUtility::bcd2asc(header_bcd, header);
    dealMsgHeader(header);
    if (m_vec_field.size() != HEADER_FIELD_COUNT)
    {
        PLOG_ERROR("msg header is invalid, msg header=%s, header field size=%d", header.c_str(), (int)m_vec_field.size());
        return -1;
    }

    offset = MSG_LEN+HEADER_LEN/2;

    msg_type_bcd = msg.substr(offset, MTI_LEN/2);
    StringUtility::bcd2asc(msg_type_bcd, m_msg_type);
    PLOG_DEBUG("msg type:%s", m_msg_type.c_str());

    m_bitmap = msg.substr(offset+MTI_LEN/2, BITMAP_LEN);
    StringUtility::bcd2asc(m_bitmap, log_hex);
    PLOG_DEBUG("msg bitmap:%s", log_hex.c_str());

    m_content = msg.substr(offset+MTI_LEN/2+BITMAP_LEN);
    StringUtility::bcd2asc(m_content, log_hex);
    PLOG_DEBUG("msg content:%s", log_hex.c_str());

    if (getBitmap(BITMAP_LEN*8) != 0)
    {
        return -1;
    }

    //if (m_bitset_bitmap[BITMAP_LEN*8-1] == 1)
    //{
    //    m_mac_data = msg_type_bcd+m_bitmap+m_content.substr(0, m_content.size()-MAC_VALUE_LEN);
    //}
    //else
    //{
    //    m_mac_data = msg_type_bcd+m_bitmap+m_content;
    //}

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
            PLOG_INFO("%03d:%s", i+1, log_hex.c_str());
            m_map_field.insert(make_pair(i+1, item_value));
            item_value.erase();
        }
    }

    if (m_bitset_bitmap[BITMAP_LEN*8-1] == 1)
    {
        m_mac_data = msg_type_bcd+m_bitmap+m_content.substr(0, offset-MAC_VALUE_LEN);
    }
    else
    {
        m_mac_data = msg_type_bcd+m_bitmap+m_content.substr(0,offset);
    }

    return 0;
}

string ISO8583::GetMsgKey()
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

