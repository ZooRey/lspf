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
函数名称: PackFieldData
函数功能: 组各域数据
参    数: pData[in/out]:已打包好的报文数据
返 回 值: 打包数据的长度
相关调用:
备    注:
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
函数名称: UnPackFieldData
函数功能: 拆各域数据
参    数: iLeaveLen[in]:剩下的未拆报文数据长度
返 回 值: >0 域数据实际长度， 其他 失败
相关调用:
备    注:
*******************************************************************/
int CIso8583Field::UnPackFieldData(int iLeaveLen, const unsigned char * pData)
{
    int iValidLen = 0, iDataLen = 0;
    if(m_iFormat == FIX)//定长
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
            default ://未定义数据类型
                return 0;
        }
    }
    else
    {
        if(m_iLenType == NR)
        {//BCD表示的长度
            iValidLen = Ccommon::Bcd2Int(pData, m_iFormat*2, NR);
        }
        else if(m_iLenType == ASC)
        {//ASC表示长度
            char cTmp[8]={0};
            memcpy(cTmp, pData, m_iFormat);
            iValidLen = atoi(cTmp);
        }
        else
        {//未定义的长度类型
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
            default ://未定义数据类型
                return 0;
        }
    }

    if(iLeaveLen < iDataLen + m_iFormat)//长度异常
    {
        return 0;
    }

    if(m_pData)
    {
        free(m_pData);
        m_pData = NULL;
    }

    if((m_pData = (unsigned char *) malloc(iDataLen+8)) == NULL)//内存申请失败
    {
        return 0;
    }

    m_iDataLen = iDataLen + m_iFormat;
    memcpy(m_pData, pData, m_iDataLen);

    return m_iDataLen;
}

/*******************************************************************
函数名称: SetValueByHex
函数功能: 以十六进制格式设置域值
参    数: pHexData[in]:十六进制格式的域值(补齐后),不含长度区
          iDataLen[in]:pHexData长度,不含长度区
          iValidLen[in]:域数据的实际有效长度，若为定长域，可不传
                        通常情况下若该域为变长BCD码，且有效数据为奇数位时需传递，
                        如第2域若账号为19位则传值19，若为偶数位可不传
返 回 值: 1 成功， 0 失败
相关调用:
备    注:
********************************************************************/
int CIso8583Field::SetValueByHex(const unsigned char * pHexData, int iDataLen, int iValidLen)
{
    if(m_iFormat == FIX)//定长
    {
        if(m_iType == NL || m_iType == NR)//bcd码
        {
            if(iDataLen != (m_iLen+1)/2)//数据不全或超限
            {
            	trace_log(ERR, "Data incomplete or overflow, index [%d].", m_iIndex);
            	trace_mem((unsigned char *)pHexData, iDataLen);
                return 0;
            }
        }
        else if(m_iType == ASC || m_iType == BIN)
        {
            if(iDataLen != m_iLen)//数据不全或超限
            {
            	trace_log(ERR, "Data incomplete or overflow, index [%d].", m_iIndex);
            	trace_mem((unsigned char *)pHexData, iDataLen);
                return 0;
            }
        }
        else //未定义的数据类型
        {
        	trace_log(ERR, "Undefined data type, index [%d].", m_iIndex);
            return 0;
        }
    }
    else
    {
        if(m_iType == NL || m_iType == NR)//bcd码
        {
            if(iDataLen > (m_iLen+1)/2)//超过变长域最大长度限制
            {
            	trace_log(ERR, "More than maximum length for variable-length domain, index [%d].", m_iIndex);
            	trace_mem((unsigned char *)pHexData, iDataLen);
                return 0;
            }
        }
        else if(m_iType == ASC || m_iType == BIN)
        {
            if(iDataLen > m_iLen)//超过变长域最大长度限制
            {
            	trace_log(ERR, "More than maximum length for variable-length domain, index [%d].", m_iIndex);
            	trace_mem((unsigned char *)pHexData, iDataLen);
                return 0;
            }
        }
        else //未定义的数据类型
        {
        	trace_log(ERR, "Undefined data type, index [%d].", m_iIndex);
            return 0;
        }
    }

    RemoveField();

    if((m_pData = (unsigned char *) malloc(iDataLen+8)) == NULL)//内存申请失败
    {
    	trace_log(ERR, "Memory allocation failed.");
        return 0;
    }

    if(m_iFormat != FIX)//非定长
    {
        if(iValidLen == 0)
        {
            iValidLen = ((m_iType == ASC || m_iType == BIN) ? iDataLen : iDataLen*2);
        }

        if(m_iLenType == NR)
        {//BCD表示的长度
            Ccommon::Int2Bcd(iValidLen, m_iFormat, m_pData, NR);//填充长度数据
        }
        else if(m_iLenType == ASC)
        {
            sprintf((char *)m_pData, "%0*d", m_iFormat, iValidLen);
        }
    }

    memcpy(m_pData+m_iFormat, pHexData, iDataLen);//填充域数据
    m_iDataLen = m_iFormat + iDataLen ;//报文数据域的长度：长度区的长度+HEX数据值的长度

    return 1 ;
}

/*******************************************************************
函数名称: SetValueByStr
函数功能: 以字符串格式设置域值
参    数: pStrData[in]:字符串格式的域值(未补齐),不含长度区
返 回 值: 1 成功， 0 失败
相关调用:
备    注:
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

    if((m_iType == BIN && iValidLen > m_iLen*2)||(m_iType != BIN && iValidLen > m_iLen))//长度超出最大值
    {
    	trace_log(ERR, "Length exceeds its maximum, data [%s], index [%d].", pStrData, m_iIndex);
        return 0;
    }

    int iTmpLen = 0;
    memset(cAscData, 0, sizeof(cAscData));
    if(m_iFormat == FIX)//定长
    {
        iTmpLen = (m_iLen % 2 == 0) ? m_iLen : (m_iLen + 1);
        if(m_iType == NL)//左靠BCD后补0
        {
            memset(cAscData, '0', iTmpLen);
            memcpy(cAscData, pStrData, iValidLen);
        }
        else if(m_iType == NR)//右靠BCD前补0
        {
            memset(cAscData, '0', iTmpLen);
            memcpy(cAscData+iTmpLen-iValidLen, pStrData, iValidLen);
        }
        else if(m_iType == BIN)//二进制后补F
        {
            memset(cAscData, 'F', m_iLen*2);
            memcpy(cAscData, pStrData, iValidLen);
        }
        else if(m_iType == ASC)//ASC后补空格
        {
            memset(cAscData, ' ', m_iLen);
            memcpy(cAscData, pStrData, iValidLen);
        }
        else //未定义的数据类型
        {
        	trace_log(ERR, "Undefined data type.");
            return 0;
        }
    }
    else//变长
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
        default ://未定义数据类型
        {
        	trace_log(ERR, "Undefined data type.");
            return 0;
        }
    }
    return SetValueByHex(cHexData, iDataLen, iValidLen);
}

/*******************************************************************
函数名称: GetValueByHex
函数功能: 以十六进制格式获取域值
参    数: pHexData[out]:十六进制格式的域值,不含长度区
          iMaxLen[in]:pHexData的最大存储量
返 回 值: >0 成功， 0 失败
相关调用:
备    注:
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
        return iRetValue;//数组溢出
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
函数名称: GetValueByStr
函数功能: 以十六进制格式获取域值
参    数: pStrData[out]:字符串格式的域值,不含长度区
          iMaxLen[in]:pStrData的最大存储量
返 回 值: >0 成功， 0 失败
相关调用:
备    注:
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

    if(m_iType != ASC)//非ASC码，需转换
    {
        if(iRetValue*2 > iMaxLen)
        {
            return 0;//数组溢出,=为保留结束符的位置
        }
        Ccommon::Hex2Str(cHexData, pStrData, iRetValue);
        iRetValue *= 2 ;
    }
    else
    {
        memcpy(pStrData, cHexData, iRetValue);
    }

    if(m_iFormat != FIX)//非定长
    {
        if(m_iLenType == NR)
        {//长度类型为BCD
            iRetValue = Ccommon::Bcd2Int(m_pData, m_iFormat+1, m_iLenType, m_iFormat);
        }
        else if(m_iLenType == ASC)
        {//长度类型为ASC
            char cTmpData[16]={0};
            memcpy(cTmpData, m_pData, m_iFormat);
            iRetValue = atoi(cTmpData);
        }
        if(m_iType == NR)
		{	//有效数据前移，fixed by lijingwang拷贝计算错误
			memcpy(pStrData, pStrData+strlen(pStrData)-iRetValue, iRetValue);
			pStrData[m_iLen] = '\0';
		}
    }
    else
    {
        if(m_iType == NL)
        {//丢掉补位数据
            pStrData[m_iLen] = '\0';
        }
        else if(m_iType == NR)
        {//有效数据前移
            memcpy(pStrData, pStrData+iRetValue-m_iLen, m_iLen);
            pStrData[m_iLen] = '\0';
        }
    }

    if(iRetValue >= iMaxLen)
    {
        return 0;//数组溢出，=预留结束符
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

