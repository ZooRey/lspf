
#ifndef LSPF_NET_COMMON_h_
#define LSPF_NET_COMMON_h_

#ifdef __GNUC__
#define LIKELY(val) (__builtin_expect((val), 1))
#define UNLIKELY(val) (__builtin_expect((val), 0))
#else
#define LIKELY(val) (val)
#define UNLIKELY(val) (val)
#endif

#define NET_LIB_ERR_RETURN(msg) \
    snprintf(Message::error_msg, sizeof(Message::error_msg), "%s", (msg));\
    Message::p_error_msg = Message::error_msg;\
    return -1;

#define TBUS_ERR_RETURN(ret) \
    Message::p_error_msg = tsf4g_tbus::BusEnv::GetErrorStr(ret);\
    return -1;

#define ERRNO_RETURN() \
    Message::p_error_msg = strerror(errno);\
    return -1;

namespace lspf { namespace net {

} /* namespace lspf */ } /* namespace net */

#endif // LSPF_NET_COMMON_h_
