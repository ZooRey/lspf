#ifndef LSPF_SCLOG_LOG_H
#define LSPF_SCLOG_LOG_H

#include <stdint.h>
#include <string>
#include "common/thread.h"

namespace lspf {
namespace log {

//�ݽ�֧���̳߳��ռ���־����֧�ֲ����½������̼߳�¼��־
class ScribeLog {
public:
    static void Start();

    static void SetHostAndPort(const std::string &address, const int port);

    static void SetLogCategory(const std::string &category);

    static void SetErrorCategory(const std::string &category);

    /// @brief дlog�ӿ�
    /// @para id,cls
    static void Write(int pri, const char* file, uint32_t line,
                const char* function, const char *id, const char* msg);

    static void Flush();

private:
    ScribeLog(){}
	~ScribeLog(){}

    class InnerThread : public Thread {
    public:
        InnerThread();
        virtual ~InnerThread();

        virtual void Run();
    private:
    };

private:
    static std::string m_address;
    static int m_port;
    static InnerThread m_thread;
};

} // namespace sclog
} // namespace lspf

#endif //#ifndef LSPF_SCLOG_LOG_H
