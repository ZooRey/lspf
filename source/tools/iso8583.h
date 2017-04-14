#ifndef ISO8583_H
#define ISO8583_H

#include "iso8583field.h"

template<typename _Ty>
class CIso8583
{
public:
	typedef CIso8583<_Ty> _Myt;

	 // Ĭ�Ϲ���
    explicit CIso8583(unsigned char *pTPDU = NULL, unsigned char *pMsgHead = NULL);

    // ��������
    CIso8583(const _Myt & iso);

    // ����
    virtual ~CIso8583();

    // ��ֵ����������
    CIso8583 &operator=(const _Myt &iso);
public:
    // ���
    int Pack(unsigned char *pBuffer, unsigned & iLen);

    // ���
    int UnPack(const unsigned char * pBuffer, unsigned iLen);

    // ������ֵ
    int SetValueByHex(const int iIndex, const unsigned char *pHexData, int iDataLen, int iValidLen = 0);

    // ������ֵ
    int SetValueByStr(const int iIndex, const char * pStrData);

    // ������ֵ
    int CloneValue(const int iIndex, const _Myt *iso);

    // ȡ������
    int GetValueByHex(const int iIndex, unsigned char * pHexData, int iMaxLen);

    // ȡ������
    int GetValueByStr(const int iIndex, char * pStrData, int iMaxLen);

    // ɾ����
    int RemoveField(const int iIndex);

    // ȡMACDATA(����MACǩ��������)
    int GetMacData(unsigned char * pMacData, int iMaxLen) const;

    // ȡ����TPDU
    int GetTpdu(unsigned char * pTpdu, int iMaxLen) const;

    // ȡ����ͷ
    int GetMsgHead(unsigned char * pMsgHead, int iMaxLen) const;

    // ���ñ���ͷ
    int SetMsgHead(unsigned char * pMsgHead, int iLen);

    // ����TPDU
    int SetMsgTPDU(unsigned char * pMsgTPDU, int iLen);

    // ĳ���Ƿ����
    bool FieldExist(const int iIndex);

private:
    // λͼ����
    int SetBitMap(const int iIndex, int iFlag = 1);
    // λͼ�ж�
    bool BitExist(const int iIndex);
    // ��ʼ��Э������
    void InitProtocol();

private:
    // 8583����Э��ģ����
    _Ty *m_pIFF;
};

#endif // ISO8583_H
