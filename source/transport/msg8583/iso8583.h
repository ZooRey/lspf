#ifndef __CUPS_8583_H__
#define __CUPS_8583_H__

#include "base8583.h"

#define MSG_LEN	            2
#define HEADER_FIELD_COUNT  9
#define HEADER_LEN		    0x18
#define HEADER_ID	        "60"
#define MTI_LEN			    4
#define BITMAP_LEN		    8
#define MAC_VALUE_LEN	    8

class ISO8583 : public Base8583
{
public:
    ISO8583();
	~ISO8583();
public:
    string GetInfo();
    virtual void InitHeader();
	virtual string GetMsgKey();
	virtual int Pack(string &msg, bool mac_flag = true);
	virtual int Unpack(const string &msg);
private:
	void getStyle(ItemStyle &item_style, unsigned int index);
	int decoding(unsigned int index, unsigned int &offset, const ItemStyle &item_style, string &dest);
	int encoding(unsigned int index, const string &src, const ItemStyle &item_style, string &dest);

	void dealMsgHeader(const string &header);
private:
	vector<unsigned int> m_vec_item_style;
};

#endif //__CUPS_8583_H__
