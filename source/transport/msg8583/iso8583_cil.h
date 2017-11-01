#pragma once

#include "transport/base8583.h"

#define MSG_LEN	            2
#define HEADER_FIELD_COUNT  8
#define HEADER_LEN		    0x16
#define HEADER_ID	        "60"
#define MTI_LEN			    4
#define BITMAP_LEN		    8
#define MAC_VALUE_LEN	    8

class ISO8583_CIL : public Base8583
{
public:
    ISO8583_CIL();
	~ISO8583_CIL();
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

