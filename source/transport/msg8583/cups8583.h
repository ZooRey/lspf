#ifndef __CUPS_8583_H__
#define __CUPS_8583_H__

#include "base8583.h"

#define MSG_LEN	            4
#define HEADER_FIELD_COUNT  10
#define HEADER_LEN		    0x2E
#define HEADER_FLAG_VER	    0x02
#define MTI_LEN			    4
#define BITMAP_LEN		    16
#define MAC_VALUE_LEN	    8

class CUPS8583 : public Base8583
{
public:
    CUPS8583();
	~CUPS8583();
public:
    virtual void InitHeader();
	virtual string GetMsgKey();
	virtual int Pack(string &msg, bool mac_flag = true);
	virtual int Unpack(const string &msg);
private:
	void getStyle(ItemStyle &item_style, unsigned int index);
	int decoding(unsigned int index, unsigned int &offset, const ItemStyle &item_style, string &dest);
	int encoding(unsigned int index, const string &src, const ItemStyle &item_style, string &dest);

	bool isMacField(unsigned int index);
	void dealMacChar(string &value);

	void dealMsgHeader(const string &header);
private:
	vector<unsigned int> m_vec_item_style;
};

#endif //__CUPS_8583_H__
