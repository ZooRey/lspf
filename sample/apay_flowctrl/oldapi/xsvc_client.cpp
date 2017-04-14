#include "xsvc_client.h"
#include <string.h>
#include "xservice.h"

XSvcClient::XSvcClient(const std::string & strSvcServer, const std::string &strSvcName, const std::string &strXSvcCid, int nXSvcPid)
    :m_strSvcServer(strSvcServer), m_strSvcName(strSvcName), m_strXSvcCid(strXSvcCid), m_nXSvcPid(nXSvcPid)
{

}
XSvcClient::~XSvcClient()
{

}

bool XSvcClient::xcall(const std::string &strInput, std::string &strOutput, const int nTimeout)
{
    int iDataLen = 0;
    int timeout = 0;
    char xSvcBuf[MAX_XSVC_DATALEN];

    memset(xSvcBuf, 0, MAX_XSVC_DATALEN);
    memcpy(xSvcBuf, strInput.c_str(), strInput.size());

    iDataLen = strInput.size();

    timeout = nTimeout;
    if (timeout < 10 || timeout > 90)
    {
        timeout = 60;
    }

    int iCallRet = xsvc_call(m_strSvcServer.c_str(), m_strSvcName.c_str(), (unsigned char*)xSvcBuf, &iDataLen, timeout, m_nXSvcPid, (char*)m_strXSvcCid.c_str());
    if (iCallRet != 1)
    {
        return false;
    }

    strOutput.assign(xSvcBuf, iDataLen);

    return true;
}

bool XSvcClient::xacall(const std::string &strInput, int & nAsynId)
{
    int iDataLen = 0;
    char xSvcBuf[MAX_XSVC_DATALEN];

    memset(xSvcBuf, 0, MAX_XSVC_DATALEN);
    memcpy(xSvcBuf, strInput.c_str(), strInput.size());

    iDataLen = strInput.size();

    //int icd = xsvc_acall(ACC_PAY, ACC_PAY_SVC, (unsigned char*)xBuf, iDataLen, 0, m_xsvc_cid);
    nAsynId = xsvc_acall(m_strSvcServer.c_str(), m_strSvcName.c_str(), (unsigned char*)xSvcBuf, iDataLen, m_nXSvcPid, (char*)m_strXSvcCid.c_str());
    if (nAsynId > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}


