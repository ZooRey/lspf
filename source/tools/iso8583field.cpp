#include "iso8583field.h"

CIso8583Field::CIso8583Field(int iIndex, int iFormat, int iLen, int iType, int iLenType):
m_iIndex(iIndex), m_iFormat(iFormat), m_iLen(iLen), m_iType(iType), m_iLenType(iLenType)
{
    m_iDataLen = 0;
    m_pData = NULL;
}

CIso8583Field::~CIso8583Field()
{
    if(m_pData)
    {
        free(m_pData);
        m_pData = NULL;
    }
}

/*******************************************************************
��������: PackFieldData
��������: ���������
��    ��: pData[in/out]:�Ѵ���õı�������
�� �� ֵ: ������ݵĳ���
��ص���:
��    ע:
*******************************************************************/
int CIso8583Field::PackFieldData(unsigned char * pData) const
{
    if(m_pData)
    {
        memcpy(pData, m_pData, m_iDataLen);
        return m_iDataLen;
    }
    return 0;
}

/*******************************************************************
��������: UnPackFieldData
��������: ���������
��    ��: iLeaveLen[in]:ʣ�µ�δ�������ݳ���
�� �� ֵ: >0 ������ʵ�ʳ��ȣ� ���� ʧ��
��ص���:
��    ע:
*******************************************************************/
int CIso8583Field::UnPackFieldData(int iLeaveLen, const unsigned char * pData)
{
    int iValidLen = 0, iDataLen = 0;
    if(m_iFormat == FIX)//����
    {
        switch(m_iType)
        {
            case NL:
            case NR:
                iDataLen = (m_iLen+1)/2;
                break;
            case ASC:
            case BIN:
                iDataLen = m_iLen;
                break;
            default ://δ������������
                return 0;
        }
    }
    else
    {
        if(m_iLenType == NR)
        {//BCD��ʾ�ĳ���
            iValidLen = Ccommon::Bcd2Int(pData, m_iFormat*2, NR);
        }
        else if(m_iLenType == ASC)
        {//ASC��ʾ����
            char cTmp[8]={0};
            memcpy(cTmp, pData, m_iFormat);
            iValidLen = atoi(cTmp);
        }
        else
        {//δ����ĳ�������
            return 0;
        }

        switch(m_iType)
        {
            case NL:
            case NR:
                iDataLen = (1 + iValidLen)/2;
                break;
            case ASC:
            case BIN:
                iDataLen = iValidLen;
                break;
            default ://δ������������
                return 0;
        }
    }

    if(iLeaveLen < iDataLen + m_iFormat)//�����쳣
    {
        return 0;
    }

    if(m_pData)
    {
        free(m_pData);
        m_pData = NULL;
    }

    if((m_pData = (unsigned char *) malloc(iDataLen+8)) == NULL)//�ڴ�����ʧ��
    {
        return 0;
    }

    m_iDataLen = iDataLen + m_iFormat;
    memcpy(m_pData, pData, m_iDataLen);

    return m_iDataLen;
}

/*******************************************************************
��������: SetValueByHex
��������: ��ʮ�����Ƹ�ʽ������ֵ
��    ��: pHexData[in]:ʮ�����Ƹ�ʽ����ֵ(�����),����������
          iDataLen[in]:pHexData����,����������
          iValidLen[in]:�����ݵ�ʵ����Ч���ȣ���Ϊ�����򣬿ɲ���
                        ͨ�������������Ϊ�䳤BCD�룬����Ч����Ϊ����λʱ�贫�ݣ�
                        ���2�����˺�Ϊ19λ��ֵ19����Ϊż��λ�ɲ���
�� �� ֵ: 1 �ɹ��� 0 ʧ��
��ص���:
��    ע:
********************************************************************/
int CIso8583Field::SetValueByHex(const unsigned char * pHexData, int iDataLen, int iValidLen)
{
    if(m_iFormat == FIX)//����
    {
        if(m_iType == NL || m_iType == NR)//bcd��
        {
            if(iDataLen != (m_iLen+1)/2)//���ݲ�ȫ����
            {
            	trace_log(ERR, "Data incomplete or overflow, index [%d].", m_iIndex);
            	trace_mem((unsigned char *)pHexData, iDataLen);
                return 0;
            }
        }
        else if(m_iType == ASC || m_iType == BIN)
        {
            if(iDataLen != m_iLen)//���ݲ�ȫ����
            {
            	trace_log(ERR, "Data incomplete or overflow, index [%d].", m_iIndex);
            	trace_mem((unsigned char *)pHexData, iDataLen);
                return 0;
            }
        }
        else //δ�������������
        {
        	trace_log(ERR, "Undefined data type, index [%d].", m_iIndex);
            return 0;
        }
    }
    else
    {
        if(m_iType == NL || m_iType == NR)//bcd��
        {
            if(iDataLen > (m_iLen+1)/2)//�����䳤����󳤶�����
            {
            	trace_log(ERR, "More than maximum length for variable-length domain, index [%d].", m_iIndex);
            	trace_mem((unsigned char *)pHexData, iDataLen);
                return 0;
            }
        }
        else if(m_iType == ASC || m_iType == BIN)
        {
            if(iDataLen > m_iLen)//�����䳤����󳤶�����
            {
            	trace_log(ERR, "More than maximum length for variable-length domain, index [%d].", m_iIndex);
            	trace_mem((unsigned char *)pHexData, iDataLen);
                return 0;
            }
        }
        else //δ�������������
        {
        	trace_log(ERR, "Undefined data type, index [%d].", m_iIndex);
            return 0;
        }
    }

    RemoveField();

    if((m_pData = (unsigned char *) malloc(iDataLen+8)) == NULL)//�ڴ�����ʧ��
    {
    	trace_log(ERR, "Memory allocation failed.");
        return 0;
    }

    if(m_iFormat != FIX)//�Ƕ���
    {
        if(iValidLen == 0)
        {
            iValidLen = ((m_iType == ASC || m_iType == BIN) ? iDataLen : iDataLen*2);
        }

        if(m_iLenType == NR)
        {//BCD��ʾ�ĳ���
            Ccommon::Int2Bcd(iValidLen, m_iFormat, m_pData, NR);//��䳤������
        }
        else if(m_iLenType == ASC)
        {
            sprintf((char *)m_pData, "%0*d", m_iFormat, iValidLen);
        }
    }

    memcpy(m_pData+m_iFormat, pHexData, iDataLen);//���������
    m_iDataLen = m_iFormat + iDataLen ;//����������ĳ��ȣ��������ĳ���+HEX����ֵ�ĳ���

    return 1 ;
}

/*******************************************************************
��������: SetValueByStr
��������: ���ַ�����ʽ������ֵ
��    ��: pStrData[in]:�ַ�����ʽ����ֵ(δ����),����������
�� �� ֵ: 1 �ɹ��� 0 ʧ��
��ص���:
��    ע:
********************************************************************/
int CIso8583Field::SetValueByStr(const char * pStrData)
{
    int iDataLen = 0, iValidLen = strlen(pStrData);
    char cAscData[FIELD_MAXLEN];
    unsigned char cHexData[FIELD_MAXLEN];

    if(iValidLen == 0)
    {
    	trace_log(ERR, "The data length is 0, index [%d].", m_iIndex);
        return 0;
    }

    if((m_iType == BIN && iValidLen > m_iLen*2)||(m_iType != BIN && iValidLen > m_iLen))//���ȳ������ֵ
    {
    	trace_log(ERR, "Length exceeds its maximum, data [%s], index [%d].", pStrData, m_iIndex);
        return 0;
    }

    int iTmpLen = 0;
    memset(cAscData, 0, sizeof(cAscData));
    if(m_iFormat == FIX)//����
    {
        iTmpLen = (m_iLen % 2 == 0) ? m_iLen : (m_iLen + 1);
        if(m_iType == NL)//��BCD��0
        {
            memset(cAscData, '0', iTmpLen);
            memcpy(cAscData, pStrData, iValidLen);
        }
        else if(m_iType == NR)//�ҿ�BCDǰ��0
        {
            memset(cAscData, '0', iTmpLen);
            memcpy(cAscData+iTmpLen-iValidLen, pStrData, iValidLen);
        }
        else if(m_iType == BIN)//�����ƺ�F
        {
            memset(cAscData, 'F', m_iLen*2);
            memcpy(cAscData, pStrData, iValidLen);
        }
        else if(m_iType == ASC)//ASC�󲹿ո�
        {
            memset(cAscData, ' ', m_iLen);
            memcpy(cAscData, pStrData, iValidLen);
        }
        else //δ�������������
        {
        	trace_log(ERR, "Undefined data type.");
            return 0;
        }
    }
    else//�䳤
    {
        memcpy(cAscData, pStrData, iValidLen);
    }

    switch(m_iType)
    {
        case NL :
        case NR :
        case BIN:
            Ccommon::Asc2Bcd(cAscData, cHexData, strlen(cAscData), m_iType);
            iDataLen = (strlen(cAscData)+1)/2;
            break;
        case ASC:
            memcpy(cHexData, cAscData, strlen(cAscData));
            iDataLen = strlen(cAscData);
            break;
        default ://δ������������
        {
        	trace_log(ERR, "Undefined data type.");
            return 0;
        }
    }
    return SetValueByHex(cHexData, iDataLen, iValidLen);
}

/*******************************************************************
��������: GetValueByHex
��������: ��ʮ�����Ƹ�ʽ��ȡ��ֵ
��    ��: pHexData[out]:ʮ�����Ƹ�ʽ����ֵ,����������
          iMaxLen[in]:pHexData�����洢��
�� �� ֵ: >0 �ɹ��� 0 ʧ��
��ص���:
��    ע:
********************************************************************/
int CIso8583Field::GetValueByHex(unsigned char * pHexData, int iMaxLen) const
{
    int iRetValue = 0;

    if(m_pData == NULL)
    {
        return iRetValue;
    }

    if(iMaxLen < m_iDataLen - m_iFormat)
    {
        return iRetValue;//�������
    }

    if(m_pData == NULL)
    {
        return iRetValue;
    }

    iRetValue = m_iDataLen - m_iFormat;
    memcpy(pHexData, &m_pData[m_iFormat], iRetValue);
    return iRetValue;
}

/*******************************************************************
��������: GetValueByStr
��������: ��ʮ�����Ƹ�ʽ��ȡ��ֵ
��    ��: pStrData[out]:�ַ�����ʽ����ֵ,����������
          iMaxLen[in]:pStrData�����洢��
�� �� ֵ: >0 �ɹ��� 0 ʧ��
��ص���:
��    ע:
********************************************************************/
int CIso8583Field::GetValueByStr(char * pStrData, int iMaxLen) const
{
    int iRetValue = 0;
    unsigned char cHexData[iMaxLen+1];

    iRetValue = GetValueByHex(cHexData, iMaxLen);
    if(iRetValue == 0)
    {
        return iRetValue;
    }

    if(m_iType != ASC)//��ASC�룬��ת��
    {
        if(iRetValue*2 > iMaxLen)
        {
            return 0;//�������,=Ϊ������������λ��
        }
        Ccommon::Hex2Str(cHexData, pStrData, iRetValue);
        iRetValue *= 2 ;
    }
    else
    {
        memcpy(pStrData, cHexData, iRetValue);
    }

    if(m_iFormat != FIX)//�Ƕ���
    {
        if(m_iLenType == NR)
        {//��������ΪBCD
            iRetValue = Ccommon::Bcd2Int(m_pData, m_iFormat+1, m_iLenType, m_iFormat);
        }
        else if(m_iLenType == ASC)
        {//��������ΪASC
            char cTmpData[16]={0};
            memcpy(cTmpData, m_pData, m_iFormat);
            iRetValue = atoi(cTmpData);
        }
        if(m_iType == NR)
		{	//��Ч����ǰ�ƣ�fixed by lijingwang�����������
			memcpy(pStrData, pStrData+strlen(pStrData)-iRetValue, iRetValue);
			pStrData[m_iLen] = '\0';
		}
    }
    else
    {
        if(m_iType == NL)
        {//������λ����
            pStrData[m_iLen] = '\0';
        }
        else if(m_iType == NR)
        {//��Ч����ǰ��
            memcpy(pStrData, pStrData+iRetValue-m_iLen, m_iLen);
            pStrData[m_iLen] = '\0';
        }
    }

    if(iRetValue >= iMaxLen)
    {
        return 0;//���������=Ԥ��������
    }

    pStrData[iRetValue] = '\0';

    return iRetValue;
}

int CIso8583Field::RemoveField()
{
    if(m_pData)
    {
        free(m_pData);
        m_pData = NULL;
    }
    return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////

CIso8583FieldsFactory::CIso8583FieldsFactory()
{
    m_pMF = (MsgFmt *) malloc(sizeof(MsgFmt));
    memset(m_pMF, 0, sizeof(MsgFmt));

    memset(m_cMsgBuffer, 0, sizeof(m_cMsgBuffer));
    m_iMsgLen = 0;
}

CIso8583FieldsFactory::~CIso8583FieldsFactory()
{
    if(m_pMF)
    {
        free(m_pMF);
        m_pMF = NULL;
    }
}

MsgFmt *CIso8583FieldsFactory::GetMF() const
{
    return m_pMF;
}

int CIso8583FieldsFactory::SetMsgData(const unsigned char *pBuffer, unsigned int iBufLen)
{
    if(iBufLen > sizeof(m_cMsgBuffer))
    {
        return 0;
    }
    memcpy(m_cMsgBuffer, pBuffer, iBufLen);
    m_iMsgLen = iBufLen;
    return 1;
}

int CIso8583FieldsFactory::GetMsgData(unsigned char *pBuffer, int iMaxLen) const
{
    if(iMaxLen < m_iMsgLen)
    {
        return 0;
    }
    memcpy(pBuffer, m_cMsgBuffer, m_iMsgLen);
    return m_iMsgLen;
}

int CIso8583FieldsFactory::GetMacData(unsigned char *pMacData, int iMaxLen)
{
    int iPos = m_pMF->iOffSet + m_pMF->iTpduLen + m_pMF->iHeadLen;
    int iLen = m_iMsgLen - iPos - 8;
    if(iLen > 0 && iLen <= iMaxLen)
    {
        memcpy(pMacData, m_cMsgBuffer + iPos, iLen);
        return iLen;
    }
    return 0;
}

