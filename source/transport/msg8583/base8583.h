#ifndef __BASE_8583_H__
#define __BASE_8583_H__

#include <map>
#include <vector>
#include <string>
#include <bitset>
using namespace std;

#define MAX_ITEM_NUM            192
#define MAX_HEADER_FILED_COUNT  16

#define _8583_ASCII		0x01
#define _8583_Numeric	0x02    //LeftBcd
#define _8583_RightBcd	0x04
#define _8583_Binary	0x08
#define _8583_Variable	0x80
#define _8583_Z	        0x22

typedef struct
{
   unsigned int   no;
   unsigned int   type;
   unsigned int   var;
   int  max_len;
   string value;
}ItemStyle;

class Base8583
{
public:
    Base8583();
	virtual ~Base8583();

    void Init();

    string GetMsgType();
    void SetMsgType(const string &msg_type);

    string  GetMacData();

    string GetHeaderField(int index);
    int AddHeaderField(int index, const string &value);

	int AddField(unsigned int index, const string &value);
	int DelField(unsigned int index);
	string GetField(unsigned int index);
public:
    virtual void InitHeader() = 0;
	virtual string GetMsgKey() = 0;

	virtual int Pack(string &msg, bool mac_flag = true) = 0;
	virtual int Unpack(const string &msg) = 0;
protected:
	int getBitmap(int bit_count);
    int setBitmap(int bit_count);
protected:
	string m_msg_type;
	string m_bitmap;
    string m_content;
	string m_mac_data;
	vector<string> m_vec_field; //Header Field
	map<unsigned int, string> m_map_field; //Content Field
	bitset<MAX_ITEM_NUM> m_bitset_bitmap;
};

#endif //__BASE_8583_H__
