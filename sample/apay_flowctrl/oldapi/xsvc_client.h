#ifndef __XSEVICE_CLIENT_H__
#define __XSEVICE_CLIENT_H__

#include <string>

#include "xsvc_define.h"

class XSvcClient
{
public:
    //*  strSvcServer[in] - 目标服务器标识
    //*  strSvcName[in]   - 目标服务名
    //*  xsvc_pid[in]     - 目标接收者(目标服务进程的子进程pid，默认值:0)
    //*                     xsvc_pid==0时，由目标服务进程的任意子进程处理
    //*  xsvc_cid[in]     - 调用者标识(附加在[XSVC_DATA]传给目标服务进程)
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
