#ifndef __XSEVICE_CLIENT_H__
#define __XSEVICE_CLIENT_H__

#include <string>

#include "xsvc_define.h"

class XSvcClient
{
public:
    //*  strSvcServer[in] - Ŀ���������ʶ
    //*  strSvcName[in]   - Ŀ�������
    //*  xsvc_pid[in]     - Ŀ�������(Ŀ�������̵��ӽ���pid��Ĭ��ֵ:0)
    //*                     xsvc_pid==0ʱ����Ŀ�������̵������ӽ��̴���
    //*  xsvc_cid[in]     - �����߱�ʶ(������[XSVC_DATA]����Ŀ��������)
    explicit XSvcClient(const std::string & strSvcServer, const std::string &strSvcName, const std::string &strXSvcCid="", int nXSvcPid = 0);
    ~XSvcClient();
	bool xcall(const std::string &strInput, std::string &strOutput, const int nTimeout = 60);
    bool xacall(const std::string &strInput, int & nAsynId);

    void setSvcServer(const std::string & strSvcServer);
    void setSvcName(const std::string & strSvcName);
    void setSvcPid(const int & nSvcPid);
    void setSvcCid(const std::string & strSvcCid);
private:
    std::string m_strSvcServer;
    std::string m_strSvcName;
    std::string m_strXSvcCid;
    int m_nXSvcPid;
};


inline void XSvcClient::setSvcServer(const std::string & strSvcServer)
{
    m_strSvcServer = strSvcServer;
}

inline void XSvcClient::setSvcName(const std::string & strSvcName)
{
    m_strSvcName = strSvcName;
}

inline void XSvcClient::setSvcPid(const int & nSvcPid)
{
    m_nXSvcPid = nSvcPid;
}

inline void XSvcClient::setSvcCid(const std::string & strXSvcCid)
{
    m_strXSvcCid = strXSvcCid;
}

#endif
