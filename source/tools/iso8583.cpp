#include "iso8583.h"

template<typename _Ty>
CIso8583<_Ty>::CIso8583(unsigned char *pTPDU, unsigned char *pMsgHead) : m_pIFF(NULL)
{
    InitProtocol();

    if(m_pIFF != NULL)
    {
        if(pTPDU)
        {
            memcpy(m_pIFF->GetMF()->cTpdu, pTPDU, m_pIFF->GetMF()->iTpduLen);
        }
        if(pMsgHead)
        {
            memcpy(m_pIFF->GetMF()->cHead, pMsgHead, m_pIFF->GetMF()->iHeadLen);
        }
    }
}

template<typename _Ty>
CIso8583<_Ty>::CIso8583(const _Myt &iso) : m_pIFF(NULL)
{
    InitProtocol();

    if(m_pIFF != NULL)
    {
        char cData[PACK_MAXLEN];
        int iLen;

        memcpy(m_pIFF->GetMF(), iso.m_pIFF->GetMF(), sizeof(MsgFmt));

        for(int i = 0; i < m_pIFF->GetMF()->iFieldCnt; i++)
        {
            if(const_cast<_Myt &>(iso).GetValueByStr(i, cData, FIELD_MAXLEN) > 0)
            {
                SetValueByStr(i, cData);
            }
        }
        if((iLen = const_cast<_Myt &>(iso).m_pIFF->GetMsgData((unsigned char *)cData, PACK_MAXLEN)) > 0)
        {
            m_pIFF->SetMsgData((unsigned char *)cData, iLen);
        }
    }
}

template<typename _Ty>
void CIso8583<_Ty>::InitProtocol()
{
	if (m_pIFF == NULL)
	    m_pIFF = new _Ty();
}

template<typename _Ty>
CIso8583<_Ty>::~CIso8583()
{
    //dtor
    if(m_pIFF)
    {
        delete m_pIFF;
        m_pIFF = NULL;
    }
}

template<typename _Ty>
CIso8583<_Ty> &CIso8583<_Ty>::operator=(const _Myt &iso)
{
    if(this != &iso)
    {
        if (m_pIFF != NULL)
        {
            delete m_pIFF;
            m_pIFF = NULL;
        }

        InitProtocol();

        if (m_pIFF != NULL)
        {
            char cData[PACK_MAXLEN];
            int iLen;

            memcpy(m_pIFF->GetMF(), iso.m_pIFF->GetMF(), sizeof(MsgFmt));

            for(int i = 0; i < m_pIFF->GetMF()->iFieldCnt; i++)
            {
                if(const_cast<_Myt&>(iso).GetValueByStr(i, cData, FIELD_MAXLEN) > 0)
                {
                    SetValueByStr(i, cData);
                }
            }
            if((iLen = const_cast<_Myt&>(iso).m_pIFF->GetMsgData((unsigned char *)cData, PACK_MAXLEN)) > 0)
            {
                m_pIFF->SetMsgData((unsigned char *)cData, iLen);
            }
        }
    }
    return *this;
}

/*******************************************************************
函数名称: SetBitMap
函数功能: 设置域索引对应位图位为1或0
参    数: iIndex[in]:域索引
           iFlag[in]:设置的值0或1
返 回 值: 1 成功， 0 失败
相关调用: 在设置域值时调用
备    注: 默认设为1
********************************************************************/
template<typename _Ty>
int CIso8583<_Ty>::SetBitMap(const int iIndex, int iFlag)
{
    if(iIndex == 0 || iIndex == 1)
    {//第0域与第1域返回成功
        return 1;
    }

    if(iIndex < 2 || iIndex >= m_pIFF->GetMF()->iFieldCnt)
    {
        return 0 ;
    }
    unsigned char ch = 0x01;
    ch = ch << (iIndex%8 == 0 ? 0 : (8 - iIndex%8));
    if(iFlag == 1)
    {
        m_pIFF->GetMF()->cBitMap[(iIndex-1)/8] |= ch ;
    }
    else
    {
        m_pIFF->GetMF()->cBitMap[(iIndex-1)/8] &= ~ch ;
    }
    return 1 ;
}

/*******************************************************************
函数名称: BitExist
函数功能: 判断某一域是否存在
参    数: iIndex[in]:域序号
返 回 值: 1 存在， 0 不存在
相关调用:
备    注:
********************************************************************/
template<typename _Ty>
bool CIso8583<_Ty>::BitExist(const int iIndex)
{
    if(m_pIFF == NULL || m_pIFF->GetField(iIndex) == NULL)
    {
        return 0;
    }

    if(iIndex == 0 || iIndex == 1)
    {//第0域与第1域返回成功
        return 1;
    }

    if(iIndex < 2 || iIndex >= m_pIFF->GetMF()->iFieldCnt)
    {
        return 0 ;
    }

    return (m_pIFF->GetMF()->cBitMap[(iIndex-1)/8] >> (iIndex%8 == 0 ? 0 : (8 - iIndex%8))) & 0x01 ;
}

/*******************************************************************
函数名称: Pack
函数功能: 将各域数据打包
参    数: pBuffer[out]:打包后的报文数据
          iLen[in/out]:in:pBuffer的最大容量，out:报文数据长度
返 回 值: 1 成功， 0 失败
相关调用:
备    注: 如果报文有MAC，需将计算出来的MAC覆盖报文数据的后8个字节
********************************************************************/
template<typename _Ty>
int CIso8583<_Ty>::Pack(unsigned char *pBuffer, unsigned &iLen)
{
    if(m_pIFF == NULL)
    {
    	trace_log(ERR, "The parameters m_pIFF is empty.");
        return 0;
    }
    unsigned int iDataLen = 0, iMsgLen = m_pIFF->GetMF()->iOffSet;//报文长度数据占位
    unsigned char cMsgBuffer[PACK_MAXLEN]={0}, cFieldData[FIELD_MAXLEN]={0};
    char cTmpData[8]={0};

    memcpy(cMsgBuffer + iMsgLen, m_pIFF->GetMF()->cTpdu, m_pIFF->GetMF()->iTpduLen);//tpdu
    iMsgLen += m_pIFF->GetMF()->iTpduLen;

    memcpy(cMsgBuffer + iMsgLen, m_pIFF->GetMF()->cHead, m_pIFF->GetMF()->iHeadLen);//报文头
    iMsgLen += m_pIFF->GetMF()->iHeadLen;

    if(m_pIFF->GetField(0) == NULL)
    {
    	trace_log(ERR, "Result is an empty implementation of m_pIFF-> GetField (0).");
        return 0;
    }
    iDataLen = m_pIFF->GetField(0)->PackFieldData(cMsgBuffer + iMsgLen);//消息类型
    if(iDataLen == 0)
    {
    	trace_log(ERR, "Packaged message type failed, a length of 0.");
        return 0;
    }
    iMsgLen += iDataLen;

    memcpy(cMsgBuffer + iMsgLen, m_pIFF->GetMF()->cBitMap, m_pIFF->GetMF()->iBitMapLen);//位图
    iMsgLen += m_pIFF->GetMF()->iBitMapLen;

    if(1 != SetValueByHex(1, m_pIFF->GetMF()->cBitMap, m_pIFF->GetMF()->iBitMapLen))
    {
    	trace_log(ERR, "Set one domain bitmap failure.");
        return 0;
    }

    for(int i = 2; i < m_pIFF->GetMF()->iFieldCnt; i++)
    {
        if(BitExist(i))
        {
            if(m_pIFF->GetField(i) == NULL)
            {
            	trace_log(ERR, "Access to fields[%d] result is empty.", i);
                return 0;
            }
            iDataLen = m_pIFF->GetField(i)->PackFieldData(cFieldData);
            if(iDataLen == 0)
            {
            	trace_log(ERR, "Packaging field[%d] failure.", i);
                return 0;
            }
            if(iMsgLen + iDataLen > sizeof(cMsgBuffer))//数组溢出
            {
            	trace_log(ERR, "Array overflow, current length is [%d]", iMsgLen + iDataLen);
                return 0;
            }
            memcpy(cMsgBuffer + iMsgLen, cFieldData, iDataLen);
            iMsgLen += iDataLen;
        }
    }

    if((unsigned)iLen < iMsgLen)//数组溢出
    {
    	trace_log(ERR, "Array overflow, in param buffer length is [%d], current length is [%d]", iLen, iMsgLen);
        return 0;
    }

    iDataLen = iMsgLen - m_pIFF->GetMF()->iOffSet;//应用数据(ISO8583)长度
    switch(m_pIFF->GetMF()->iLenType)
    {
        case NR://如果长度为BCD应为右靠
            Ccommon::Int2Bcd(iDataLen, m_pIFF->GetMF()->iOffSet, cMsgBuffer, m_pIFF->GetMF()->iLenType);
            break;
        case ASC://ASC表示的长度
            sprintf(cTmpData, "%0*d", m_pIFF->GetMF()->iOffSet, iDataLen);
            memcpy(cMsgBuffer, cTmpData, m_pIFF->GetMF()->iOffSet);
            break;
        case HEXF://16进制表示的高位在前的长度
            Ccommon::MemxCpy(cMsgBuffer, &iDataLen, m_pIFF->GetMF()->iOffSet);
            break;
        case HEXA://16进制表示的高位在后的长度
            memcpy(cMsgBuffer, &iDataLen, m_pIFF->GetMF()->iOffSet);
            break;
        default://非法的长度类型
        {
        	trace_log(ERR, "Illegal length type.");
            return 0;
        }
    }

    memcpy(pBuffer, cMsgBuffer, iMsgLen);
    iLen = iMsgLen;

    return m_pIFF->SetMsgData(cMsgBuffer, iMsgLen);

}

/*******************************************************************
函数名称: UnPack
函数功能: 解析报文各域数据
参    数: pBuffer[in]:报文数据
           iLen[in]:报文数据长度
返 回 值: 1 成功， 其他 失败
相关调用:
备    注:
********************************************************************/
template<typename _Ty>
int CIso8583<_Ty>::UnPack(const unsigned char *pBuffer, unsigned iLen)
{
    if(m_pIFF == NULL)
    {
    	trace_log(ERR, "The parameters m_pIFF is empty.");
        return 0;
    }

    if(pBuffer == NULL)
    {
    	trace_log(ERR, "The parameters pBuffer is empty.");
        return 0;
    }

    if(iLen < (unsigned int)(m_pIFF->GetMF()->iOffSet + m_pIFF->GetMF()->iTpduLen + m_pIFF->GetMF()->iHeadLen + m_pIFF->GetMF()->iBitMapLen))
    {
    	trace_log(ERR, "In parameter cache too little");
        return 0;
    }

    if(iLen > PACK_MAXLEN)
    {
    	trace_log(ERR, "In parameter cache too large");
        return 0;
    }

    int iPos = 0, iDataLen;

    iPos += m_pIFF->GetMF()->iOffSet;

    memcpy(m_pIFF->GetMF()->cTpdu, pBuffer+iPos, m_pIFF->GetMF()->iTpduLen);//tpdu
    iPos += m_pIFF->GetMF()->iTpduLen;

    memcpy(m_pIFF->GetMF()->cHead, pBuffer+iPos, m_pIFF->GetMF()->iHeadLen);//报文头
    iPos += m_pIFF->GetMF()->iHeadLen;

    if(m_pIFF->GetField(0) == NULL)
    {
    	trace_log(ERR, "Result is an empty implementation of m_pIFF-> GetField (0).");
        return 0;
    }
    iDataLen = m_pIFF->GetField(0)->UnPackFieldData(iLen-iPos, pBuffer+iPos);//消息类型
    if(iDataLen > 0)
    {
        iPos += iDataLen;
    }
    else
    {
    	trace_log(ERR, "Packaged message type failed, a length of 0.");
        return 0;
    }

    if(m_pIFF->GetField(1) == NULL)
    {
    	trace_log(ERR, "get one domain bitmap failure.");
        return 0;
    }
    iDataLen = m_pIFF->GetField(1)->UnPackFieldData(iLen-iPos, pBuffer+iPos);//位图
    if(iDataLen == m_pIFF->GetMF()->iBitMapLen)
    {
        iPos += iDataLen;
    }
    else
    {
    	trace_log(ERR, "unpack failed[1] bitmap domain failure.");
        return 0;
    }
    GetValueByHex(1, m_pIFF->GetMF()->cBitMap, sizeof(m_pIFF->GetMF()->cBitMap));

    for(int i = 2; i < m_pIFF->GetMF()->iFieldCnt; i++)
    {
        if(BitExist(i))
        {
            if(m_pIFF->GetField(i) == NULL)
            {
            	trace_log(ERR, "Access to fields[%d] result is empty.", i);
                return 0;
            }
            if((iDataLen = m_pIFF->GetField(i)->UnPackFieldData(iLen-iPos, pBuffer+iPos)) > 0)
            {
                iPos += iDataLen;
            }
            else
            {
            	trace_log(ERR, "unpack failed[%d] failure.", i);
                return 0;
            }
        }
    }

    return m_pIFF->SetMsgData(pBuffer, iLen);

}

/*******************************************************************
函数名称: SetValueByHex
函数功能: 以十六进制格式设置域值
参    数: iIndex[in]:域序号
           pHexData[in]:十六进制格式的域值(补齐后),不含长度区
           iDataLen[in]:域数据长度,不含长度区
           iValidLen[in]:域数据的实际有效长度，若为定长域，可不传
                        通常情况下若该域为变长BCD码，且有效数据为奇数位时需传递，
                        如第2域若账号为19位则传值19，若为偶数位可不传
返 回 值: 1 成功， 0 失败
相关调用:
备    注:
********************************************************************/
template<typename _Ty>
int CIso8583<_Ty>::SetValueByHex(const int iIndex, const unsigned char *pHexData, int iDataLen, int iValidLen)
{
    if(m_pIFF == NULL || m_pIFF->GetField(iIndex) == NULL)
    {
    	trace_log(ERR, "The parameters m_pIFF is empty.");
        return 0;
    }
    if(iIndex < 0 || iIndex >= m_pIFF->GetMF()->iFieldCnt)
    {
    	trace_log(ERR, "iIndex overflow.");
        return 0;
    }
/*
    if(pHexData[0] == 0)
    {
    	trace_log(ERR, "The parameters iIndex[%d] pStrData is empty.",iIndex);
        return 0;
    }
*/
    int iRet = m_pIFF->GetField(iIndex)->SetValueByHex(pHexData, iDataLen, iValidLen);
    if(iRet && !SetBitMap(iIndex))
    {
        return 0;
    }
    return iRet;
}

/*******************************************************************
函数名称: SetValueByStr
函数功能: 以字符串格式设置域值
参    数: iIndex[in]:域序号
           pStrData[in]:字符串格式的域值(未补齐),不含长度区
返 回 值: 1 成功， 0 失败
相关调用:
备    注:
********************************************************************/
template<typename _Ty>
int CIso8583<_Ty>::SetValueByStr(const int iIndex, const char *pStrData)
{
    if(m_pIFF == NULL || m_pIFF->GetField(iIndex) == NULL)
    {
    	trace_log(ERR, "The parameters m_pIFF is empty.");
        return 0;
    }
	/*
    if(pStrData[0] == 0)
    {
    	trace_log(ERR, "The parameters iIndex[%d] pStrData is empty.",iIndex);
        return 0;
    }
	*/

    if(iIndex < 0 || iIndex >= m_pIFF->GetMF()->iFieldCnt)
    {
    	trace_log(ERR, "iIndex overflow.");
        return 0;
    }
    int iRet = m_pIFF->GetField(iIndex)->SetValueByStr(pStrData);
    if(iRet && !SetBitMap(iIndex))
    {
        return 0;
    }
    return iRet;
}

/*******************************************************************
函数名称: CloneValue
函数功能: 拷贝域值
参    数: iIndex[in]:域序号
       iso[in]:拷贝的源对象
返 回 值: 1 成功， 0 失败
相关调用:
备    注:
********************************************************************/
template<typename _Ty>
int CIso8583<_Ty>::CloneValue(const int iIndex, const _Myt *iso)
{
	if (iso == NULL || this == iso)
	{
		// 如果源对象和本对象相同则返回失败
		return 0;
	}

	if (m_pIFF == NULL || m_pIFF->GetField(iIndex) == NULL)
	{
		return 0;
	}
	if (iIndex < 0 || iIndex >= m_pIFF->GetMF()->iFieldCnt)
	{
		return 0;
	}

	char bFieldData[FIELD_MAXLEN] = {0};
	int iFieldDataLen = 0;
    iFieldDataLen = const_cast<_Myt*>(iso)->GetValueByStr(iIndex, bFieldData, sizeof(bFieldData));
	int iRet = 0;

	if (iFieldDataLen > 0)
	{
		iRet = m_pIFF->GetField(iIndex)->SetValueByStr(bFieldData);
	}

	if (iRet && !SetBitMap(iIndex))
	{
		return 0;
	}

	return iRet;
}

/*******************************************************************
函数名称: GetValueByHex
函数功能: 以十六进制格式获取域值
参    数: iIndex[in]:域序号
          pHexData[out]:十六进制格式的域值,不含长度区
          iMaxLen[in]:pHexData的最大存储量
返 回 值: >0 成功， 0 失败
相关调用:
备    注:
********************************************************************/
template<typename _Ty>
int CIso8583<_Ty>::GetValueByHex(const int iIndex, unsigned char *pHexData, int iMaxLen)
{
    if(!BitExist(iIndex))
    {
        return 0 ;//域不存在
    }
    return m_pIFF->GetField(iIndex)->GetValueByHex(pHexData, iMaxLen);
}

/*******************************************************************
函数名称: GetValueByStr
函数功能: 以十六进制格式获取域值
参    数: iIndex[in]:域序号
          pStrData[out]:字符串格式的域值,不含长度区
          iMaxLen[in]:pStrData的最大存储量
返 回 值: >0 成功， 0 失败
相关调用:
备    注:
********************************************************************/
template<typename _Ty>
int CIso8583<_Ty>::GetValueByStr(const int iIndex, char *pStrData, int iMaxLen)
{
    if(!BitExist(iIndex))
    {
        return 0 ;//域不存在
    }
    return m_pIFF->GetField(iIndex)->GetValueByStr(pStrData, iMaxLen);
}

/*******************************************************************
函数名称: RemoveField
函数功能: 删除域
参    数: iIndex[in]:域序号
返 回 值: 1 成功， 0 失败
相关调用: 如果只是删除某域可调用此函数；若是某域已存在，
          现需删除后重新设置域值可直接调用函数SetValueByHex或SetValueByStr，而无需调用该函数
备    注: 该函数不能删除消息类型域与位图域
********************************************************************/
template<typename _Ty>
int CIso8583<_Ty>::RemoveField(const int iIndex)
{
    if(iIndex == 0 || iIndex == 1)
    {//第0域与第1域不能删除
        return 0;
    }

    if(!BitExist(iIndex))
    {
        return 1 ;//域不存在,返回成功
    }

    m_pIFF->GetField(iIndex)->RemoveField();

    return SetBitMap(iIndex, 0);
}
/*******************************************************************
函数名称: GetMacData
函数功能: 获取计算MAC签名的数据
参    数: pMacData[out] 用于计算MAC的数据
          iMaxLen[in] 数据最大容量
返 回 值: >0 返回数据的长度, =0 未获取到数据
相关调用:
备    注:
********************************************************************/
template<typename _Ty>
int CIso8583<_Ty>::GetMacData(unsigned char *pMacData, int iMaxLen) const
{
    return m_pIFF->GetMacData(pMacData, iMaxLen);
}

// 取报文TPDU
template<typename _Ty>
int CIso8583<_Ty>::GetTpdu(unsigned char *pTpdu, int iMaxLen) const
{
    if(iMaxLen < m_pIFF->GetMF()->iTpduLen)
    {
        return 0;
    }
    memcpy(pTpdu, m_pIFF->GetMF()->cTpdu, m_pIFF->GetMF()->iTpduLen);
    return m_pIFF->GetMF()->iTpduLen;
}

// 取报文头
template<typename _Ty>
int CIso8583<_Ty>::GetMsgHead(unsigned char *pMsgHead, int iMaxLen) const
{
    if(iMaxLen < m_pIFF->GetMF()->iHeadLen)
    {
        return 0;
    }
    memcpy(pMsgHead, m_pIFF->GetMF()->cHead, m_pIFF->GetMF()->iHeadLen);
    return m_pIFF->GetMF()->iHeadLen;
}

// 某域是否存在
template<typename _Ty>
bool CIso8583<_Ty>::FieldExist(const int iIndex)
{
    return BitExist(iIndex);
}

// 设置报文头
template<typename _Ty>
int CIso8583<_Ty>::SetMsgHead(unsigned char * pMsgHead, int iLen)
{
	if (iLen > m_pIFF->GetMF()->iHeadLen)
	{
		return 0;
	}
	memcpy(m_pIFF->GetMF()->cHead, pMsgHead, iLen);
	return iLen;
}

// 设置TPDU
template<typename _Ty>
int CIso8583<_Ty>::SetMsgTPDU(unsigned char * pMsgTPDU, int iLen)
{
	if (iLen > m_pIFF->GetMF()->iTpduLen)
	{
		return 0;
	}
	memcpy(m_pIFF->GetMF()->cTpdu, pMsgTPDU, iLen);
	return iLen;
}
