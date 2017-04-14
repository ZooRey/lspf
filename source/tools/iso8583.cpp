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
��������: SetBitMap
��������: ������������ӦλͼλΪ1��0
��    ��: iIndex[in]:������
           iFlag[in]:���õ�ֵ0��1
�� �� ֵ: 1 �ɹ��� 0 ʧ��
��ص���: ��������ֵʱ����
��    ע: Ĭ����Ϊ1
********************************************************************/
template<typename _Ty>
int CIso8583<_Ty>::SetBitMap(const int iIndex, int iFlag)
{
    if(iIndex == 0 || iIndex == 1)
    {//��0�����1�򷵻سɹ�
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
��������: BitExist
��������: �ж�ĳһ���Ƿ����
��    ��: iIndex[in]:�����
�� �� ֵ: 1 ���ڣ� 0 ������
��ص���:
��    ע:
********************************************************************/
template<typename _Ty>
bool CIso8583<_Ty>::BitExist(const int iIndex)
{
    if(m_pIFF == NULL || m_pIFF->GetField(iIndex) == NULL)
    {
        return 0;
    }

    if(iIndex == 0 || iIndex == 1)
    {//��0�����1�򷵻سɹ�
        return 1;
    }

    if(iIndex < 2 || iIndex >= m_pIFF->GetMF()->iFieldCnt)
    {
        return 0 ;
    }

    return (m_pIFF->GetMF()->cBitMap[(iIndex-1)/8] >> (iIndex%8 == 0 ? 0 : (8 - iIndex%8))) & 0x01 ;
}

/*******************************************************************
��������: Pack
��������: ���������ݴ��
��    ��: pBuffer[out]:�����ı�������
          iLen[in/out]:in:pBuffer�����������out:�������ݳ���
�� �� ֵ: 1 �ɹ��� 0 ʧ��
��ص���:
��    ע: ���������MAC���轫���������MAC���Ǳ������ݵĺ�8���ֽ�
********************************************************************/
template<typename _Ty>
int CIso8583<_Ty>::Pack(unsigned char *pBuffer, unsigned &iLen)
{
    if(m_pIFF == NULL)
    {
    	trace_log(ERR, "The parameters m_pIFF is empty.");
        return 0;
    }
    unsigned int iDataLen = 0, iMsgLen = m_pIFF->GetMF()->iOffSet;//���ĳ�������ռλ
    unsigned char cMsgBuffer[PACK_MAXLEN]={0}, cFieldData[FIELD_MAXLEN]={0};
    char cTmpData[8]={0};

    memcpy(cMsgBuffer + iMsgLen, m_pIFF->GetMF()->cTpdu, m_pIFF->GetMF()->iTpduLen);//tpdu
    iMsgLen += m_pIFF->GetMF()->iTpduLen;

    memcpy(cMsgBuffer + iMsgLen, m_pIFF->GetMF()->cHead, m_pIFF->GetMF()->iHeadLen);//����ͷ
    iMsgLen += m_pIFF->GetMF()->iHeadLen;

    if(m_pIFF->GetField(0) == NULL)
    {
    	trace_log(ERR, "Result is an empty implementation of m_pIFF-> GetField (0).");
        return 0;
    }
    iDataLen = m_pIFF->GetField(0)->PackFieldData(cMsgBuffer + iMsgLen);//��Ϣ����
    if(iDataLen == 0)
    {
    	trace_log(ERR, "Packaged message type failed, a length of 0.");
        return 0;
    }
    iMsgLen += iDataLen;

    memcpy(cMsgBuffer + iMsgLen, m_pIFF->GetMF()->cBitMap, m_pIFF->GetMF()->iBitMapLen);//λͼ
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
            if(iMsgLen + iDataLen > sizeof(cMsgBuffer))//�������
            {
            	trace_log(ERR, "Array overflow, current length is [%d]", iMsgLen + iDataLen);
                return 0;
            }
            memcpy(cMsgBuffer + iMsgLen, cFieldData, iDataLen);
            iMsgLen += iDataLen;
        }
    }

    if((unsigned)iLen < iMsgLen)//�������
    {
    	trace_log(ERR, "Array overflow, in param buffer length is [%d], current length is [%d]", iLen, iMsgLen);
        return 0;
    }

    iDataLen = iMsgLen - m_pIFF->GetMF()->iOffSet;//Ӧ������(ISO8583)����
    switch(m_pIFF->GetMF()->iLenType)
    {
        case NR://�������ΪBCDӦΪ�ҿ�
            Ccommon::Int2Bcd(iDataLen, m_pIFF->GetMF()->iOffSet, cMsgBuffer, m_pIFF->GetMF()->iLenType);
            break;
        case ASC://ASC��ʾ�ĳ���
            sprintf(cTmpData, "%0*d", m_pIFF->GetMF()->iOffSet, iDataLen);
            memcpy(cMsgBuffer, cTmpData, m_pIFF->GetMF()->iOffSet);
            break;
        case HEXF://16���Ʊ�ʾ�ĸ�λ��ǰ�ĳ���
            Ccommon::MemxCpy(cMsgBuffer, &iDataLen, m_pIFF->GetMF()->iOffSet);
            break;
        case HEXA://16���Ʊ�ʾ�ĸ�λ�ں�ĳ���
            memcpy(cMsgBuffer, &iDataLen, m_pIFF->GetMF()->iOffSet);
            break;
        default://�Ƿ��ĳ�������
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
��������: UnPack
��������: �������ĸ�������
��    ��: pBuffer[in]:��������
           iLen[in]:�������ݳ���
�� �� ֵ: 1 �ɹ��� ���� ʧ��
��ص���:
��    ע:
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

    memcpy(m_pIFF->GetMF()->cHead, pBuffer+iPos, m_pIFF->GetMF()->iHeadLen);//����ͷ
    iPos += m_pIFF->GetMF()->iHeadLen;

    if(m_pIFF->GetField(0) == NULL)
    {
    	trace_log(ERR, "Result is an empty implementation of m_pIFF-> GetField (0).");
        return 0;
    }
    iDataLen = m_pIFF->GetField(0)->UnPackFieldData(iLen-iPos, pBuffer+iPos);//��Ϣ����
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
    iDataLen = m_pIFF->GetField(1)->UnPackFieldData(iLen-iPos, pBuffer+iPos);//λͼ
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
��������: SetValueByHex
��������: ��ʮ�����Ƹ�ʽ������ֵ
��    ��: iIndex[in]:�����
           pHexData[in]:ʮ�����Ƹ�ʽ����ֵ(�����),����������
           iDataLen[in]:�����ݳ���,����������
           iValidLen[in]:�����ݵ�ʵ����Ч���ȣ���Ϊ�����򣬿ɲ���
                        ͨ�������������Ϊ�䳤BCD�룬����Ч����Ϊ����λʱ�贫�ݣ�
                        ���2�����˺�Ϊ19λ��ֵ19����Ϊż��λ�ɲ���
�� �� ֵ: 1 �ɹ��� 0 ʧ��
��ص���:
��    ע:
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
��������: SetValueByStr
��������: ���ַ�����ʽ������ֵ
��    ��: iIndex[in]:�����
           pStrData[in]:�ַ�����ʽ����ֵ(δ����),����������
�� �� ֵ: 1 �ɹ��� 0 ʧ��
��ص���:
��    ע:
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
��������: CloneValue
��������: ������ֵ
��    ��: iIndex[in]:�����
       iso[in]:������Դ����
�� �� ֵ: 1 �ɹ��� 0 ʧ��
��ص���:
��    ע:
********************************************************************/
template<typename _Ty>
int CIso8583<_Ty>::CloneValue(const int iIndex, const _Myt *iso)
{
	if (iso == NULL || this == iso)
	{
		// ���Դ����ͱ�������ͬ�򷵻�ʧ��
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
��������: GetValueByHex
��������: ��ʮ�����Ƹ�ʽ��ȡ��ֵ
��    ��: iIndex[in]:�����
          pHexData[out]:ʮ�����Ƹ�ʽ����ֵ,����������
          iMaxLen[in]:pHexData�����洢��
�� �� ֵ: >0 �ɹ��� 0 ʧ��
��ص���:
��    ע:
********************************************************************/
template<typename _Ty>
int CIso8583<_Ty>::GetValueByHex(const int iIndex, unsigned char *pHexData, int iMaxLen)
{
    if(!BitExist(iIndex))
    {
        return 0 ;//�򲻴���
    }
    return m_pIFF->GetField(iIndex)->GetValueByHex(pHexData, iMaxLen);
}

/*******************************************************************
��������: GetValueByStr
��������: ��ʮ�����Ƹ�ʽ��ȡ��ֵ
��    ��: iIndex[in]:�����
          pStrData[out]:�ַ�����ʽ����ֵ,����������
          iMaxLen[in]:pStrData�����洢��
�� �� ֵ: >0 �ɹ��� 0 ʧ��
��ص���:
��    ע:
********************************************************************/
template<typename _Ty>
int CIso8583<_Ty>::GetValueByStr(const int iIndex, char *pStrData, int iMaxLen)
{
    if(!BitExist(iIndex))
    {
        return 0 ;//�򲻴���
    }
    return m_pIFF->GetField(iIndex)->GetValueByStr(pStrData, iMaxLen);
}

/*******************************************************************
��������: RemoveField
��������: ɾ����
��    ��: iIndex[in]:�����
�� �� ֵ: 1 �ɹ��� 0 ʧ��
��ص���: ���ֻ��ɾ��ĳ��ɵ��ô˺���������ĳ���Ѵ��ڣ�
          ����ɾ��������������ֵ��ֱ�ӵ��ú���SetValueByHex��SetValueByStr����������øú���
��    ע: �ú�������ɾ����Ϣ��������λͼ��
********************************************************************/
template<typename _Ty>
int CIso8583<_Ty>::RemoveField(const int iIndex)
{
    if(iIndex == 0 || iIndex == 1)
    {//��0�����1����ɾ��
        return 0;
    }

    if(!BitExist(iIndex))
    {
        return 1 ;//�򲻴���,���سɹ�
    }

    m_pIFF->GetField(iIndex)->RemoveField();

    return SetBitMap(iIndex, 0);
}
/*******************************************************************
��������: GetMacData
��������: ��ȡ����MACǩ��������
��    ��: pMacData[out] ���ڼ���MAC������
          iMaxLen[in] �����������
�� �� ֵ: >0 �������ݵĳ���, =0 δ��ȡ������
��ص���:
��    ע:
********************************************************************/
template<typename _Ty>
int CIso8583<_Ty>::GetMacData(unsigned char *pMacData, int iMaxLen) const
{
    return m_pIFF->GetMacData(pMacData, iMaxLen);
}

// ȡ����TPDU
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

// ȡ����ͷ
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

// ĳ���Ƿ����
template<typename _Ty>
bool CIso8583<_Ty>::FieldExist(const int iIndex)
{
    return BitExist(iIndex);
}

// ���ñ���ͷ
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

// ����TPDU
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
