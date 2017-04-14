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
    //��ֹ�ⲿʹ��Ĭ�Ϲ��캯��
    CIso8583Field():m_iIndex(0), m_iFormat(0), m_iLen(0),
                    m_iType(0), m_iLenType(0){};
    //��ֹ�ⲿʹ�ÿ������캯��
    CIso8583Field(const CIso8583Field &):m_iIndex(0), m_iFormat(0), m_iLen(0),
                                         m_iType(0), m_iLenType(0){};
    //��ֹ�ⲿʹ�ø�ֵ���������غ���
    CIso8583Field &operator=(const CIso8583Field &){return *this;}

private:
    const int m_iIndex;   //������
    const int m_iFormat;  //���ʽ(��ʾ����Ч����ǰ�������ĳ���)��FIX, LD, LLD��LLLD;
    const int m_iLen;     //����Ч���ݵĳ���,�������Ŷ����ĳ���;�䳤������󳤶�,BCD��ʽ��Ϊѹ��ǰ����Ч���ݳ���
    const int m_iType;    //���������ͣ�NL, NR, ASC, BIN
    const int m_iLenType; //�򳤶������ͣ�NR, ASC, HEXF, HEXA

    int m_iDataLen; //ʵ�ʳ��ȣ�����������
    unsigned char * m_pData;//ʵ�������ݣ�����������
};

//////////////////////////////////////////////////////////////////////////////////////////

class CIso8583FieldsFactory //8583����Э����࣬����8583����Э������̳и���
{
public:
	CIso8583FieldsFactory();
	virtual ~CIso8583FieldsFactory();

public:
	virtual CIso8583Field *GetField(const int iIndex) = 0;

    MsgFmt *GetMF() const;

    int SetMsgData(const unsigned char *pBuffer, unsigned int iBufLen);
    int GetMsgData(unsigned char *pBuffer, int iMaxLen) const;

    virtual int GetMacData(unsigned char *pMacData, int iMaxLen);//���MAC���ǲ���ȫ���ļ��ܣ����������а�Э��涨�㷨���Ǹú�����

protected:
    MsgFmt *m_pMF; //���������и���8583��Э���ʽ��ֵ

    int m_iMsgLen;    //���ĳ���
    unsigned char m_cMsgBuffer[PACK_MAXLEN];//��������

};

#endif // ISO8583FIELD_H
