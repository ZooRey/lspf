#ifndef ISO8583FIELD_H
#define ISO8583FIELD_H

#include "pubfunc.h"

class CIso8583Field
{
public:
    CIso8583Field(int iIndex, int iFormat, int iLen, int iType = NL, int iLenType = NR);
    virtual ~CIso8583Field();

public:
    int PackFieldData(unsigned char * pData) const;
    int UnPackFieldData(int iLeaveLen, const unsigned char * pData);

    int SetValueByHex(const unsigned char * pHexData, int iDataLen, int iValidLen);
    int SetValueByStr(const char * pStrData);

    int GetValueByHex(unsigned char * pHexData, int iMaxLen) const;
    int GetValueByStr(char * pStrData, int iMaxLen) const;

    int RemoveField();

private:
    //禁止外部使用默认构造函数
    CIso8583Field():m_iIndex(0), m_iFormat(0), m_iLen(0),
                    m_iType(0), m_iLenType(0){};
    //禁止外部使用拷贝构造函数
    CIso8583Field(const CIso8583Field &):m_iIndex(0), m_iFormat(0), m_iLen(0),
                                         m_iType(0), m_iLenType(0){};
    //禁止外部使用赋值操作符重载函数
    CIso8583Field &operator=(const CIso8583Field &){return *this;}

private:
    const int m_iIndex;   //域索引
    const int m_iFormat;  //域格式(表示域有效数据前长度区的长度)：FIX, LD, LLD，LLLD;
    const int m_iLen;     //域有效数据的长度,定长域存放定长的长度;变长域存放最大长度,BCD格式域为压缩前的有效数据长度
    const int m_iType;    //域数据类型：NL, NR, ASC, BIN
    const int m_iLenType; //域长度区类型：NR, ASC, HEXF, HEXA

    int m_iDataLen; //实际长度，包括长度区
    unsigned char * m_pData;//实际域数据，包括长度区
};

//////////////////////////////////////////////////////////////////////////////////////////

class CIso8583FieldsFactory //8583报文协议基类，各种8583报文协议类需继承该类
{
public:
	CIso8583FieldsFactory();
	virtual ~CIso8583FieldsFactory();

public:
	virtual CIso8583Field *GetField(const int iIndex) = 0;

    MsgFmt *GetMF() const;

    int SetMsgData(const unsigned char *pBuffer, unsigned int iBufLen);
    int GetMsgData(unsigned char *pBuffer, int iMaxLen) const;

    virtual int GetMacData(unsigned char *pMacData, int iMaxLen);//如果MAC不是采用全报文加密，需在子类中按协议规定算法覆盖该函数。

protected:
    MsgFmt *m_pMF; //需在子类中根据8583包协议格式赋值

    int m_iMsgLen;    //报文长度
    unsigned char m_cMsgBuffer[PACK_MAXLEN];//报文数据

};

#endif // ISO8583FIELD_H
