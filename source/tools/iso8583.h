#ifndef ISO8583_H
#define ISO8583_H

#include "iso8583field.h"

template<typename _Ty>
class CIso8583
{
public:
	typedef CIso8583<_Ty> _Myt;

	 // 默认构造
    explicit CIso8583(unsigned char *pTPDU = NULL, unsigned char *pMsgHead = NULL);

    // 拷贝构造
    CIso8583(const _Myt & iso);

    // 析构
    virtual ~CIso8583();

    // 赋值操作符重载
    CIso8583 &operator=(const _Myt &iso);
public:
    // 组包
    int Pack(unsigned char *pBuffer, unsigned & iLen);

    // 解包
    int UnPack(const unsigned char * pBuffer, unsigned iLen);

    // 设置域值
    int SetValueByHex(const int iIndex, const unsigned char *pHexData, int iDataLen, int iValidLen = 0);

    // 设置域值
    int SetValueByStr(const int iIndex, const char * pStrData);

    // 拷贝域值
    int CloneValue(const int iIndex, const _Myt *iso);

    // 取域数据
    int GetValueByHex(const int iIndex, unsigned char * pHexData, int iMaxLen);

    // 取域数据
    int GetValueByStr(const int iIndex, char * pStrData, int iMaxLen);

    // 删除域
    int RemoveField(const int iIndex);

    // 取MACDATA(用于MAC签名的数据)
    int GetMacData(unsigned char * pMacData, int iMaxLen) const;

    // 取报文TPDU
    int GetTpdu(unsigned char * pTpdu, int iMaxLen) const;

    // 取报文头
    int GetMsgHead(unsigned char * pMsgHead, int iMaxLen) const;

    // 设置报文头
    int SetMsgHead(unsigned char * pMsgHead, int iLen);

    // 设置TPDU
    int SetMsgTPDU(unsigned char * pMsgTPDU, int iLen);

    // 某域是否存在
    bool FieldExist(const int iIndex);

private:
    // 位图设置
    int SetBitMap(const int iIndex, int iFlag = 1);
    // 位图判断
    bool BitExist(const int iIndex);
    // 初始化协议类型
    void InitProtocol();

private:
    // 8583报文协议模板类
    _Ty *m_pIFF;
};

#endif // ISO8583_H
