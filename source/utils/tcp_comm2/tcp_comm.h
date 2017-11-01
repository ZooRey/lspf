#ifndef _TCP_COMM_H
#define _TCP_COMM_H

#include <pthread.h>
#include <string>
#include <iostream>
using namespace std;

enum SocketType{
	ST_BLOCKING = 0,
	ST_NON_BLOCKING
};

typedef int (*do_sync_msg_t)(const string &recv_buf, string &send_buf);
//typedef int (*do_comm_stat_t)(bool comm_stat);
//typedef int (*do_send_msg_t)(string &send_buff);
//typedef int (*do_recv_msg_t)(const string &recv_buff);

class TcpSocket
{

public:
	TcpSocket();
	~TcpSocket();
	int Connect(const string &remote_ip, int remote_port, int sock_type=ST_BLOCKING);
	int ConnectWithTimeout(const string &remote_ip, int remote_port, int time_out_ms);
	int Accept(int listen_socket);
	int Write(const string &buf);
	int Read(string &buf, int len = -1);
	int Close();
	int SetNonBlocking(bool blocking = false);

	void SetSocket(int sock)
	{
		m_socket = sock;
	}
	int GetSocket()
	{
		return m_socket;

	}

	string GetClientIP()
	{
		return m_client_ip;
	}
private:
	void setSocketOpt();
private:
	int m_socket;
	string m_client_ip;
};

class TcpClient
{

public:
	TcpClient();
	~TcpClient();
	//带超时时间参数的Connect是非阻塞
	int Connect(const string &remote_ip, int remote_port);
	int Connect(const string &remote_ip, int remote_port, int timeout_ms);
	int Write(const string &buf);
	int Read(string &buf);
	int Close();
	bool GetConnStat()
	{
		return m_conn_stat;
	}
	int GetSocket()
	{
		return m_tcp_socket.GetSocket();
	}
private:
	bool m_conn_stat;
	TcpSocket m_tcp_socket;
};

class TcpServer
{
public:
	TcpServer();
	~TcpServer();
	int Init(int port);
	int Accept();
	int Write(const string &buf);
	int Read(string &buf);
	int Close();
	bool GetConnStat()
	{
		return m_conn_stat;
	}
	int GetSocket()
	{
		return m_tcp_socket.GetSocket();
	}
	string GetClientIP()
	{
		return m_tcp_socket.GetClientIP();
	}
private:
	bool m_conn_stat;
	int m_listen_socket;
	int m_port;
	TcpSocket m_tcp_socket;
};

class TcpSyncClient
{

public:
	TcpSyncClient();
	~TcpSyncClient();
	//带超时时间参数的Connect是非阻塞
	int Connect(const string &remote_ip, int remote_port);
	int Connect(const string &remote_ip, int remote_port, int timeout_ms);
	int Close();
	int SyncComm(int timeout_ms, const string &req, string &rsp);
private:
	int m_epoll_handle;
	TcpSocket m_tcp_socket;
};

class TcpSyncServer
{
public:
	static TcpSyncServer* Instance();
public:
	int Init(int port, do_sync_msg_t do_msg);

	bool Start(int thread_count = 1);
	void Run();
private:
	TcpSyncServer()
	{
		m_do_msg = NULL;
	}
private:
	int m_port;
	int m_listen_socket;
	int m_epoll_handle;
	do_sync_msg_t m_do_msg;

	static TcpSyncServer* instance_;
};

//长连接异步通讯
class TcpAsync
{
public:
	TcpAsync();
	~TcpAsync();
public:
	virtual int DoBeforeConnect() {return 0;}
	virtual int DoAfterConnect() {return 0;}
	virtual int DoBeforeAccept() {return 0;}
	virtual int DoAfterAccept() {return 0;}
	virtual int DoBeforeSend(string &send_buff) {return 0;}
	virtual int DoAfterSend() {return 0;}
	virtual int DoBeforeRecv() {return 0;}
	virtual int DoAfterRecv(const string &recv_buff) {return 0;}

	virtual int CalcMsgSize(const string &len_buff) {return 0;}

	virtual void Write() {return;}
	virtual void Read() {return;}

	int Init(int len_buff_size, int timeout_ms, int local_port, int remote_port, const string &remote_ip);
	bool Start();
protected:
	bool m_connect_stat;
	bool m_accept_stat;
	int m_remote_port;
	int m_len_buff_size;
	int m_timeout_ms;
	int m_epoll_handle;
	int m_listen_socket;
	string m_remote_ip;
};

//单工双链通讯模型
class TcpAsyncSimplex : public TcpAsync
{
public:
	TcpAsyncSimplex(){}
	~TcpAsyncSimplex(){}
public:
	virtual void Write();
	virtual void Read();
};

//双工单链通讯模型
class TcpAsyncDuplexClient : public TcpAsync
{
public:
	TcpAsyncDuplexClient();
	virtual ~TcpAsyncDuplexClient();
public:
	virtual void Write();
	virtual void Read();
private:
	void WaitConnectStat(bool connect_stat);
	void SetConnectStat(bool connect_stat);
private:
	TcpSocket m_tcp_socket;
	pthread_mutex_t m_mutex;
	pthread_cond_t m_cond;
};


//双工单链通讯模型
class TcpAsyncDuplexServer : public TcpAsync
{
public:
	TcpAsyncDuplexServer();
	virtual ~TcpAsyncDuplexServer();
public:
	virtual void Write();
	virtual void Read();
private:
	void WaitAcceptStat(bool accept_stat);
	void SetAcceptStat(bool accept_stat);
private:
	TcpSocket m_tcp_socket;
	pthread_mutex_t m_mutex;
	pthread_cond_t m_cond;
};

#endif //_TCP_COMM_H
