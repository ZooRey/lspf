/**
 *Unit Name: xservice.h
 *Current Version: 1.0
 *Description:
 *  xservice api.
 *Author:
 *   24291878@qq.com
 *Copyright:
 *   Copyright(C) 2013.06
 *History:
 *TODO:
**/
#ifndef _xservice_H_
#define _xservice_H_

#if defined(_WIN32) || defined(_WINDOWS) || defined(WIN32) || defined(_WINDOWS_)
#include <winsock2.h>
#include <windows.h>
#define DLL_IMPORT  extern "C" __declspec(dllimport)
#define WINAPI      __stdcall
#else
#define DLL_IMPORT  extern "C"
#define WINAPI
#endif

#define MAX_XSVC_PROCESS    64				//每个主进程最大支持的子进程数
#define MAX_SERVICE_NUM     64				//每个子进程最大支持的服务数
#define MAX_THREADS_NUM     16				//每个服务最大的处理线程数

#define MAX_XSVC_NAME       64				//[服务器名|服务名]的最大长度
#define MAX_XSVC_CID        13				//[调用者标识]最大长度
#define MAX_XSVC_DATALEN    7870			//节点数据缓存大小(7870 = 8 * 1024 - 312)

//XSERVICE数据节点
#pragma pack(8)
typedef struct _XSVC_DATA {
	char xsvc_srvr[MAX_XSVC_NAME];          //目标[服务器名]
	char xsvc_name[MAX_XSVC_NAME];          //目标[服务名]
	char xsvc_cid[MAX_XSVC_CID];            //[调用者标识]
	unsigned char data[MAX_XSVC_DATALEN];   //[消息]内容
	int data_len;                           //[消息]长度
	int xsvc_icd;                           //[消息]标识(icd)
	int xsvc_pid;                           //特定目标接收进程(pid, 大于0时消息仅推送给指定的进程)
	_XSVC_DATA() {
		memset(xsvc_srvr, 0, MAX_XSVC_NAME);
		memset(xsvc_name, 0, MAX_XSVC_NAME);
		memset(xsvc_cid,  0, MAX_XSVC_CID);
		memset(data, 0, MAX_XSVC_DATALEN);
		data_len = 0;
		xsvc_icd = 0;
		xsvc_pid = 0;
	};
} XSVC_DATA;
#pragma pack()

/*
 *回调函数:
 *  服务接收到数据时回调函数.
 *参数:
 *  XSVC_DATA - 数据
 *返回值:
 *  0 - 无需等待处理结果(共享内存队列，将立即释放节点)
 *  1 - 需要等待处理结果(共享内存队列，节点保持不释放，直至数据被读取, 或60s超时)
 */
typedef int (WINAPI* LPFN_XSVC_DATA)(const XSVC_DATA *xsvc_data);   //接收的数据

/*
 *回调函数:
 *  子进程启动/停止时回调函数.
 *参数:
 *  child_pid  - 子进程PID
 *  event_mode - 事件模式:
 *      0: 子进程停止.
 *      1: 子进程启动.
 *      2: 异常重启.
 *      3: 负载均衡运行模式下，本主进程与运行在其他主机的主进程失联(注:每分钟报告1次，执照联系成功).
 *返回值: 无
 */
typedef void (WINAPI* LPFN_XSVC_CHILD)(const int child_pid, const int event_mode);


/**------------------------------ XSERVICE 服务API(主进程) -------------------------**/
/*
 *初始化服务主进程(主进程API)
 *参数:
 *  xsvc_server[in]   - 服务器标识(每个主进程服务器，仅允许唯一的"服务器名称"标识)
 *  xsvc_process[in]  - 服务子进程数(最大值: 64)
 *  xsvc_child_cb[in] - 子进程启动/停止时的回调函数
 *  xsvc_timeout[in]  - 队列数据超时时间(默认值: 60秒)
 *  xsvc_udpaddr[in]  - 负载均衡UDP组播地址(224.0.1.0～238.255.255.255)
 *  xsvc_udpport[in]  - 负载均衡UDP组播端口
 *返回:
 *  0 - 失败
 * >0 - 主进程PID
 * <0 - 子进程PID
 *备注:
 * 1) 本函数类似fork, 一次调用，多次返回(主进程/子进程PID).
 * 2) 本函数仅供主进程调用!
 * 3) 不指定xsvc_udpaddr/xsvc_udpport参数，则取消[负载均衡]服务.
 */
DLL_IMPORT int xsvc_init(const char *xsvc_server,
                         int xsvc_process = 1,
                         LPFN_XSVC_CHILD xsvc_child_cb = NULL,
                         int xsvc_timeout = 60,
                         char *xsvc_udpaddr = NULL,
                         unsigned xsvc_udpport = 0);

/*
 *停止服务主进程(主进程API)
 *参数:   无
 *返回值: 无
 */
DLL_IMPORT void xsvc_free();

/*
 *获取注册信息(主进程API)
 *输出参数:
 *  serial_no[16] - 序列号
 *  reg_user[64]  - 用户名
 *返回:
 *  -1: 已过期
 *   0: 正试用
 *   1: 已注册
 */
DLL_IMPORT int xsvc_register_info(char *serial_no, char *reg_user);


/**-------------------------------- XSERVICE 服务API(子进程) -----------------------**/
/*
 *注册/启动服务(子进程API)
 *参数:
 *  xsvc_name[in]     - 服务名
 *  xsvc_threads[in]  - 服务线程数(仅对指定服务: xsvc_name)
 *  xsvc_data_cb[in]  - 接收数据时回调函数
 *  xsvc_sysclass[in] - 服务队列大小(每个[服务], 一个队列):
 *                      0 - 中大系统(队列缓存:128M, 队列节点数: 10000, 缺省值)
 *                      1 - 大型系统(队列缓存:256M，队列节点数: 20000)
 *                      2 - 巨型系统(队列缓存:512M, 队列节点数: 40000)
 *返回值: 无
 */
DLL_IMPORT bool xsvc_open(const char *xsvc_name,
                          const int xsvc_threads = 1,
                          const LPFN_XSVC_DATA xsvc_data_cb = NULL,
                          const int xsvc_sysclass = 0);

/*
 *停止服务(子进程API)
 *参数:
 *  xsvc_name[in] - 服务名
 *返回值: 无
 */
DLL_IMPORT void xsvc_close(const char *xsvc_name);

/*
 *返回服务响应数据(子进程API)
 *参数:
 *  xsvc_data - 返回数据
 *返回值:
 *  true - 成功
 *  false - 失败
 */
DLL_IMPORT bool xsvc_return(XSVC_DATA *xsvc_data);


/**------------------------------ XSERVICE 客户端API -----------------------------**/
/*
 *初始化客户端环境.
 *参数:
 *  xsvc_udpaddr[in]  - 负载均衡UDP组播地址(224.0.1.0～238.255.255.255)
 *  xsvc_udpport[in]  - 负载均衡UDP组播端口
 *  local_weight[in]  - 同时存在本地/远程服务时，调用本地服务的权重;
 *                      (取值:1~9, 如取值:7 即:调用[本地服务:远程服务]的比例为[70%:30%];
 *                      如果存在多个远程服务主机, 则自动选择一个CPU占用低的主机)
 *备注:
 *  不指定xsvc_udpaddr/xsvc_udpport参数，则xsvc_call/xsvc_acall/xsvc_forward仅调用本地服务，
 *  不调用远程[负载均衡]服务, 详见: xsvc_init服务端函数.
 */
DLL_IMPORT void xsvc_init_client(char *xsvc_udpaddr = NULL,
                                 unsigned xsvc_udpport = 0,
                                 int local_weight = 7);

/*
 *释放客户端环境.
 */
DLL_IMPORT void xsvc_free_client();

/*
 *同步调用服务(客户端API)
 *参数:
 *  xsvc_server[in] - 目标服务器标识
 *  xsvc_name[in]   - 目标服务名
 *  data[in/out]    - 服务请求/响应数据
 *  len[in/out]     - 数据长度
 *  timeout[in]     - 超时(单位:秒; 为0时，取默认值: 60s)
 *  xsvc_pid[in]    - 目标接收者(目标服务进程的子进程pid，默认值:0)
 *                    xsvc_pid==0时，由目标服务进程的任意子进程处理
 *  xsvc_cid[in]    - 调用者标识(附加在[XSVC_DATA]传给目标服务进程)
 *返回值:
 *  1 - 发送且接收成功
 *  0 - 发送成功, 接收超时
 * -1 - 发送成功, 接收失败
 * -2 - 发送失败 (或所请求的服务不存在)
 * -3 - 发送失败 (负载均衡)
 * -4 - 参数错误
 */
DLL_IMPORT int xsvc_call(const char *xsvc_server, const char *xsvc_name,
                         unsigned char *data, int *len, int timeout = 60,
                         int xsvc_pid = 0, char *xsvc_cid = NULL);

/*
 *异步调用服务(客户端API)
 *参数:
 *  xsvc_server[in]  - 目标服务器名
 *  xsvc_name[in]    - 目标服务名
 *  data[in]         - 服务请求数据
 *  len[in]          - 数据长度
 *  xsvc_pid[in]     - 目标接收者(目标服务进程的子进程pid，默认值:0)
 *                     xsvc_pid==0时，由目标服务进程的任意子进程处理
 *  xsvc_cid[in]     - 调用者标识(附加在[XSVC_DATA]传给目标服务进程)
 *返回值:
 *  >0 - 返回icd(服务响应ID), 以提供xsvc_getreply()函数异步获取服务响应数据.
 *  -1 - 失败
 */
DLL_IMPORT int xsvc_acall(const char *xsvc_server, const char *xsvc_name,
                          unsigned char *data, int len,
                          int xsvc_pid = 0, char *xsvc_cid = NULL);

/*
 *查询服务响应数据(客户端API)
 *参数:
 *  xsvc_server[in]  - 目标服务器标识
 *  xsvc_icd[in]     - 服务响应ID(xsvc_acall的返回值)
 *  data[out]        - 服务响应数据
 *  len[out]         - 数据长度
 *返回值:
 *  true  - 成功
 *  false - 失败
 */
DLL_IMPORT bool xsvc_getreply(const char *xsvc_server, const int xsvc_icd,
                              unsigned char *data, int *len);

/*
 *异步数据转发服务(客户端API)
 *参数:
 *  xsvc_server[in] - 目标服务器标识
 *  xsvc_name[in]   - 目标服务名
 *  xsvc_data[in]   - 源数据
 *返回值:
 *  >=0 - 成功(forward_icd)
 *   <0 - 失败(-1:参数错/-2:数据入队失败/-3:目标服务不存在)
 */
DLL_IMPORT int  xsvc_forward(const char *xsvc_server, const char *xsvc_name, XSVC_DATA *xsvc_data);
//----------------------------------------------------------------------------


#endif  // _xservice_H_
