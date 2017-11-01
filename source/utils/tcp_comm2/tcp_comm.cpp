#include "tcp_comm.h"
#include "log/log.h"
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#define BUFFER_MAX_LEN		8192
#define EPOLL_EVENT_COUNT	10000
#define CONNECT_INTERVAL	1
///////////////////////////////////////////////////////////////////////////////////////
TcpSocket::TcpSocket()
{
	m_socket = -1;
}

TcpSocket::~TcpSocket()
{
	Close();
}

void TcpSocket::setSocketOpt()
{
	int arg = 0;
	struct linger linger_str;

    //set option for socket
    arg=1;
    setsockopt(m_socket, SOL_SOCKET, SO_KEEPALIVE, (char *)&arg, sizeof(arg));//TCP层的心跳

	//setsockopt 设置 SO_LINGER 选项：作用就是在close关闭时，保证发送数据发送到对方后，再彻底关闭连接
	//1、设置 l_onoff为0，则该选项关闭，l_linger的值被忽略，等于内核缺省情况，close调用会立即返回给调用者，如果可能将会传输任何未发送的数据；
	//2、设置 l_onoff为非0，l_linger为0，则套接口关闭时TCP夭折连接，TCP将丢弃保留在套接口发送缓冲区中的任何数据并发送一个RST给对方，而不是通常的四分组终止序列，这避免了TIME_WAIT状态；
	//3、设置 l_onoff 为非0，l_linger为非0，当套接口关闭时内核将拖延一段时间（由l_linger决定）。如果套接口缓冲区中仍残留数据，进程将处于睡眠状态，直 到（a）所有数据发送完且被对方确认，之后进行正常的终止序列（描述字访问计数为0）或（b）延迟时间到。此种情况下，应用程序检查close的返回值是非常重要的，如果在数据发送完并被确认前时间到，close将返回EWOULDBLOCK错误且套接口发送缓冲区中的任何数据都丢失。close的成功返回仅告诉我们发送的数据（和FIN）已由对方TCP确认，它并不能告诉我们对方应用进程是否已读了数据。如果套接口设为非阻塞的，它将不等待close完成。
    linger_str.l_onoff = 0;
    linger_str.l_linger = 1;
    setsockopt(m_socket, SOL_SOCKET, SO_LINGER, &linger_str, sizeof(struct linger));
}

int TcpSocket::SetNonBlocking(bool blocking)
{
	int opts;

    opts = fcntl(m_socket, F_GETFL);
    if(opts < 0)
    {
		return -1;
	}
	if(blocking)
	{
		opts = opts & ~O_NONBLOCK;
	}
	else
	{
		opts = opts | O_NONBLOCK;
	}
    
    if(fcntl(m_socket, F_SETFL, opts) < 0)
    {
        return -1;
    }

	return 0;
}

int TcpSocket::Connect(const string &remote_ip, int remote_port, int sock_type)
{
	struct sockaddr_in addr;

	// 连接服务端口
	memset(&addr,0,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(remote_ip.c_str());
	addr.sin_port = htons(remote_port);

	m_socket = socket(AF_INET, SOCK_STREAM, 0);

	setSocketOpt();
	if (sock_type == ST_NON_BLOCKING)
	{
		SetNonBlocking();
	}
	return connect(m_socket, (const struct sockaddr *)&addr, sizeof(addr));
}

int TcpSocket::ConnectWithTimeout(const string &remote_ip, int remote_port, int time_out_ms)
{
	struct sockaddr_in addr;

	// 连接服务端口
	memset(&addr,0,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(remote_ip.c_str());
	addr.sin_port = htons(remote_port);

	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(m_socket < 0)
	{
		return -1;
	}

	setSocketOpt();
	SetNonBlocking();
	int connectRet = connect(m_socket, (const struct sockaddr *)&addr, sizeof(addr));
	if(connectRet == -1 && errno != EINPROGRESS)
	{
		close(m_socket);
		return -1;
	}
	else
	{
		int epollfd = epoll_create(10);
		
		struct epoll_event epollevents[10];
		struct epoll_event ev;
		ev.data.fd = m_socket;
		ev.events = EPOLLIN|EPOLLOUT|EPOLLERR|EPOLLHUP|EPOLLET;
		if (epoll_ctl(epollfd, EPOLL_CTL_ADD, m_socket, &ev) < 0)
		{
			close(m_socket);
			close(epollfd);
			return -1;
		}
		int nfds = epoll_wait(epollfd, epollevents, 10, time_out_ms);
		for(int n = 0; n < nfds; n++)
		{
			if ((epollevents[n].events & EPOLLERR) || (epollevents[n].events & EPOLLRDHUP) || (epollevents[n].events & EPOLLHUP))
			{
				close(m_socket);
				epoll_ctl(epollfd, EPOLL_CTL_DEL, m_socket, NULL);
				close(epollfd);
				return -1;
			}
			else if(epollevents[n].events&EPOLLOUT)//socket可写，表示连接OK
			{
				SetNonBlocking(true);
				epoll_ctl(epollfd, EPOLL_CTL_DEL, m_socket, NULL);
				close(epollfd);
				return 0;
			}
			else if(epollevents[n].events&EPOLLIN)
			{
				continue;
			}
			
		}
		close(m_socket);
		epoll_ctl(epollfd, EPOLL_CTL_DEL, m_socket, NULL);
		close(epollfd);
		return -1;
	}
}

int TcpSocket::Accept(int listen_socket)
{
    struct sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr);
    if((m_socket = accept(listen_socket, (struct sockaddr *)&client_addr, &client_addr_size)) == -1)
    {
        return -1;
    }
	m_client_ip.assign(inet_ntoa(client_addr.sin_addr));

    setSocketOpt();
    SetNonBlocking();

    return 0;
}

int TcpSocket::Write(const string &buf)
{
    int ret = 0;
    int index = 0;
    int left = (int)buf.size();

	signal(SIGPIPE, SIG_IGN);
    while(left > 0)
    {
		ret = send(m_socket, buf.c_str() + index, left, 0);
		if (ret > 0) //发送成功
		{
			left -= ret;
			index += ret;
		}
		else if (0 == ret) //套接字关闭
		{
			break;
		}
		else
		{
			//EINTR == errno:连接正常，操作被中断，可继续发送
			//EWOULDBLOCK和EAGAIN这两个宏定义的值相同:连接正常，但发送缓冲区没有空间，等待下一次发送
			if (EINTR == errno || EWOULDBLOCK == errno || EAGAIN== errno)
			{
				continue;
			}
			else //出错
			{
				break;
			}
		}

	}

    if(left > 0)
        return -1;

    return 0;
}

int TcpSocket::Read(string &buf, int len)
{
    int ret = 0;
    int index = 0;
    int left = BUFFER_MAX_LEN;
	char tmp [BUFFER_MAX_LEN] = {0};

	if (len == 0)
	{
		return 0;
	}
	else if (len > 0)
	{
		left = len;
	}
	else
	{
		left = BUFFER_MAX_LEN;
	}
    while(left > 0)
    {
		ret = recv(m_socket, tmp + index, left, 0);
		if (ret > 0) //读取到正常数据
		{
		    left -= ret;
			index += ret;
			if (left == 0) //数据已读完
			{
				break;
			}
		}
		else if (0 == ret) //套接字关闭，读到FIN
		{
			break;
		}
		else
		{
			//套接字正常，可以继续读取
			//EINTR:操作被中断，可以继续读取
			//EWOULDBLOCK和EAGAIN这两个宏定义的值相同:当套接字是非阻塞时，且当前没有数据可以读取，返回这个错误码，表示这个操作本来（如果阻塞时）会被阻塞住。
			if (EINTR == errno || EWOULDBLOCK == errno || EAGAIN== errno)
			{
			    if (len < 0)
                {
                    left = 0;
                    break;
                }
                continue;
			}
			else
			{
				break;
			}
		}
    }

    if(left > 0)
        return -1;

	buf = string(tmp, index);
    return 0;

}

int TcpSocket::Close()
{
    if (m_socket <= 0)
    {
    	return 0;
    }

	shutdown(m_socket, 2);

    struct linger lin;

    lin.l_onoff = 1;
    lin.l_linger = 0;
    setsockopt(m_socket, SOL_SOCKET, SO_LINGER, (const char *)&lin, sizeof(lin));

    close(m_socket);
	m_socket = -1;
    return 0;
}

////////////////////////////////////////////////////////
TcpClient::TcpClient()
{
	m_conn_stat = false;
}

TcpClient::~TcpClient()
{
	Close();
}

int TcpClient::Connect(const string &remote_ip, int remote_port)
{
	if (m_tcp_socket.Connect(remote_ip, remote_port) != 0)
	{
		return -1;
	}
	if (m_tcp_socket.SetNonBlocking() != 0)
	{
		return -1;
	}
	m_conn_stat = true;
	return 0;
}

int TcpClient::Read(string &buf)
{
	return m_tcp_socket.Read(buf);
}
int TcpClient::Write(const string &buf)
{
	return m_tcp_socket.Write(buf);
}
int TcpClient::Close()
{
	m_conn_stat = false;
	return m_tcp_socket.Close();
}


//////////////////////////////////////////////
TcpServer::TcpServer()
{
	m_conn_stat = false;
}
TcpServer::~TcpServer()
{
	close(m_listen_socket);
	Close();
}

int TcpServer::Init(int port)
{
	int	bReuseProt = 1;
	//int opts;
    struct sockaddr_in serverAddr;

    memset(&serverAddr, 0, sizeof(struct sockaddr_in));
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);

    m_listen_socket = socket(AF_INET, SOCK_STREAM, 0);

	//设置为非阻塞
    // opts = fcntl(m_listen_socket, F_GETFL);
    // opts = opts | O_NONBLOCK;
    // fcntl(m_listen_socket, F_SETFL, opts);

	//端口重用
	setsockopt(m_listen_socket, SOL_SOCKET, SO_REUSEADDR, (const char *)&bReuseProt, sizeof(int));

    if(bind(m_listen_socket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        close(m_listen_socket);
        return -1;
    }

    if(listen(m_listen_socket, SOMAXCONN) == -1)
    {
        close(m_listen_socket);
        return -1;
    }

    return 0;
}

int TcpServer::Accept()
{
    if (m_tcp_socket.Accept(m_listen_socket) != 0)
	{
		return -1;
	}

	m_conn_stat = true;
	return 0;
}
int TcpServer::Read(string &buf)
{
	return m_tcp_socket.Read(buf);
}
int TcpServer::Write(const string &buf)
{
	return m_tcp_socket.Write(buf);
}
int TcpServer::Close()
{
	m_conn_stat = false;
	return m_tcp_socket.Close();
}

////////////////////////////////////////////////////////////////////////////////////////
TcpSyncClient::TcpSyncClient()
{
	m_epoll_handle = epoll_create(EPOLL_EVENT_COUNT);
}

TcpSyncClient::~TcpSyncClient()
{
	Close();
}

int TcpSyncClient::Connect(const string &remote_ip, int remote_port)
{
	int sock;
    struct epoll_event ev_reg;

	if (m_tcp_socket.Connect(remote_ip, remote_port) != 0)
	{
		return -1;
	}
	if (m_tcp_socket.SetNonBlocking() != 0)
	{
		return -1;
	}

	sock = m_tcp_socket.GetSocket();
	//注册epoll事件
	ev_reg.data.fd = sock;
	//ev_reg.events = EPOLLIN | EPOLLET;
	ev_reg.events = EPOLLIN;
	epoll_ctl(m_epoll_handle, EPOLL_CTL_ADD, sock, &ev_reg);

	return 0;
}

int TcpSyncClient::Connect(const string &remote_ip, int remote_port, int timeout_ms)
{
	int ret;
	int sock;
    struct epoll_event ev_reg;

	ret = m_tcp_socket.Connect(remote_ip, remote_port, ST_NON_BLOCKING);
	sock = m_tcp_socket.GetSocket();
	if (ret == 0) //客户端和服务器已经建立连接
	{
		//注册epoll事件
		ev_reg.data.fd = sock;
		//ev_reg.events = EPOLLIN | EPOLLET;
		ev_reg.events = EPOLLIN;
		epoll_ctl(m_epoll_handle, EPOLL_CTL_ADD, sock, &ev_reg);

		return 0;
	}
	else
	{
		//表示此时tcp三次握手仍旧进行，如果errno不是EINPROGRESS，则说明连接错误，程序结束
        if (errno != EINPROGRESS)
		{
			return -1;
		}
		else
		{
			//通过epoll判断是否可写， 如果可写且getsockopt不返回错，则表示连接已建立，然后再把监听事件改为可读
			//注册epoll事件
			int epoll_count;
			struct epoll_event ev_wait[EPOLL_EVENT_COUNT];

			ev_reg.data.fd = sock;
			//ev_reg.events = EPOLLOUT | EPOLLET;
			ev_reg.events = EPOLLOUT;
			epoll_ctl(m_epoll_handle, EPOLL_CTL_ADD, sock, &ev_reg);
			epoll_count = epoll_wait(m_epoll_handle, ev_wait, EPOLL_EVENT_COUNT, timeout_ms);
			if (epoll_count > 0)
			{
				//getsockopt 源自 Berkeley 的实现将在变量 error 中返回错误，getsockopt 本身返回0；
				//然而 Solaris 却让 getsockopt 返回 -1，并把错误保存在 errno 变量中
				int error;
				socklen_t len;

				ret = getsockopt(sock, SOL_SOCKET, SO_ERROR, &error, &len);
				if (ret || error)
				{
					errno = error;
					return -1;
				}
				else
				{
					//cout << "non black connect" << endl;
					//ev_reg.events = EPOLLIN | EPOLLET;
					ev_reg.events = EPOLLIN;
					epoll_ctl(m_epoll_handle, EPOLL_CTL_MOD, sock, &ev_reg);
					return 0;
				}
			}
			else
			{
				return -1;
			}
		}
	}

	return 0;
}

int TcpSyncClient::Close()
{
	int sock;

	sock = m_tcp_socket.GetSocket();
	epoll_ctl(m_epoll_handle, EPOLL_CTL_DEL, sock, NULL);
	close(m_epoll_handle);

	m_tcp_socket.Close();

	return 0;
}

int TcpSyncClient::SyncComm(int timeout_ms, const string &req, string &rsp)
{
	int i;
	int ret = 0;
	int sock;
	int epoll_count;
	struct epoll_event ev_wait[EPOLL_EVENT_COUNT];

	ret = m_tcp_socket.Write(req);
	if (ret != 0)
	{
		ret = -1;
		goto EXIT;
	}

	sock = m_tcp_socket.GetSocket();
	epoll_count = epoll_wait(m_epoll_handle, ev_wait, EPOLL_EVENT_COUNT, timeout_ms);
	if (epoll_count <=0)
	{
		ret = -2;
		goto EXIT;
	}
	for(i=0; i<epoll_count; i++)
	{
		if ( (ev_wait[i].data.fd == sock) && (ev_wait[i].events & EPOLLIN) )//如果是已经连接的，并且收到数据，那么进行读入。
		{
			ret = m_tcp_socket.Read(rsp);
			if (ret != 0)
			{
				ret = -3;
				goto EXIT;
			}
		}
	}
EXIT:
	//if (ret != 0)
	{
		Close();
	}
	return ret;
}

//////////////////////////////////////////////////////////////
TcpSyncServer* TcpSyncServer::instance_ = NULL;

TcpSyncServer* TcpSyncServer::Instance()
{
    if (instance_ == NULL)
	{
        instance_ =  new TcpSyncServer();
    }
    return instance_;
}

static void* SyncCommThreadEntry(void* arg)
{
    TcpSyncServer* tcp_sync_server = reinterpret_cast<TcpSyncServer*>(arg);

    tcp_sync_server->Run();

    return NULL;
}

int TcpSyncServer::Init(int port, do_sync_msg_t do_msg)
{
	int	bReuseProt = 1;
	int opts;
    struct sockaddr_in serverAddr;
	struct epoll_event ev_reg;

    memset(&serverAddr, 0, sizeof(struct sockaddr_in));
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);

    m_listen_socket = socket(AF_INET, SOCK_STREAM, 0);

	//设置为非阻塞
    opts = fcntl(m_listen_socket, F_GETFL);
    opts = opts | O_NONBLOCK;
    fcntl(m_listen_socket, F_SETFL, opts);

	//端口重用
	setsockopt(m_listen_socket, SOL_SOCKET, SO_REUSEADDR, (const char *)&bReuseProt, sizeof(int));

    if(bind(m_listen_socket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        close(m_listen_socket);
        return -1;
    }

    if(listen(m_listen_socket, SOMAXCONN) == -1)
    {
        close(m_listen_socket);
        return -1;
    }

	m_epoll_handle = epoll_create(EPOLL_EVENT_COUNT);
	//注册epoll事件
	ev_reg.data.fd = m_listen_socket;
	//ev_reg.events = EPOLLIN | EPOLLET;
	ev_reg.events = EPOLLIN;
	epoll_ctl(m_epoll_handle, EPOLL_CTL_ADD, m_listen_socket, &ev_reg);

	m_do_msg = do_msg;

    return 0;
}

bool TcpSyncServer::Start(int thread_count)
{
	pthread_t thread_id;

	for(int i=0; i<thread_count; i++)
	{
		if (::pthread_create(&thread_id, NULL, SyncCommThreadEntry, this) != 0)
		{
			return false;
		}
	}
	return true;
}

void TcpSyncServer::Run()
{
	int i;
	int sock;
	int epoll_count;
	string req, rsp;
	TcpSocket tcp_socket;
	struct epoll_event ev_wait[EPOLL_EVENT_COUNT];

	while(1)
	{
		epoll_count = epoll_wait(m_epoll_handle, ev_wait, EPOLL_EVENT_COUNT, -1);
		for(i=0; i<epoll_count; i++)
		{
			if(ev_wait[i].data.fd == m_listen_socket)//如果新监测到一个SOCKET用户连接到了绑定的SOCKET端口，建立新的连接。
			{
				if (tcp_socket.Accept(m_listen_socket) != 0)
				{
					continue;
				}
				struct epoll_event ev_reg;

				sock = tcp_socket.GetSocket();
                ev_reg.data.fd = sock;
                //ev_reg.events = EPOLLIN | EPOLLET;
                ev_reg.events = EPOLLIN;
                epoll_ctl(m_epoll_handle, EPOLL_CTL_ADD, sock, &ev_reg);
			}
			else if ( (ev_wait[i].data.fd == sock) && (ev_wait[i].events & EPOLLIN) )//如果是已经连接的用户，并且收到数据，那么进行读入。
			{
				if (tcp_socket.Read(req) != 0)
				{
					epoll_ctl(m_epoll_handle, EPOLL_CTL_DEL, sock, NULL);
					tcp_socket.Close();
					continue;
				}
				if (m_do_msg != NULL)
				{
					if (m_do_msg(req, rsp) != 0)
					{
						tcp_socket.Write(rsp);
					}
				}
				epoll_ctl(m_epoll_handle, EPOLL_CTL_DEL, sock, NULL);
				tcp_socket.Close();
				continue;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////
static void* TcpAsyncWriteThreadEntry(void* arg)
{
    TcpAsync* tcp_async = reinterpret_cast<TcpAsync*>(arg);

    tcp_async->Write();

    return NULL;
}
static void* TcpAsyncReadThreadEntry(void* arg)
{
    TcpAsync* tcp_async = reinterpret_cast<TcpAsync*>(arg);

    tcp_async->Read();

    return NULL;
}

TcpAsync::TcpAsync()
{
	m_connect_stat = false;
	m_accept_stat = false;

	m_epoll_handle = epoll_create(EPOLL_EVENT_COUNT);
}
TcpAsync::~TcpAsync()
{
	close(m_epoll_handle);
}

int TcpAsync::Init(int len_buff_size, int timeout_ms, int local_port, int remote_port, const string &remote_ip)
{
    int ret;
	m_len_buff_size = len_buff_size;
	m_timeout_ms = timeout_ms;
	m_remote_port = remote_port;
	m_remote_ip = remote_ip;

	if (local_port > 0)
	{
		int	bReuseProt = 1;
		//int opts;
		struct sockaddr_in serverAddr;

		memset(&serverAddr, 0, sizeof(struct sockaddr_in));
		serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(local_port);
		m_listen_socket = socket(AF_INET, SOCK_STREAM, 0);

		//设置为非阻塞
		// opts = fcntl(m_listen_socket, F_GETFL);
		// opts = opts | O_NONBLOCK;
		// fcntl(m_listen_socket, F_SETFL, opts);

		//端口重用
		setsockopt(m_listen_socket, SOL_SOCKET, SO_REUSEADDR, (const char *)&bReuseProt, sizeof(int));

		ret=bind(m_listen_socket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
		if(ret == -1)
		{
			close(m_listen_socket);
			return -1;
		}

		ret = listen(m_listen_socket, SOMAXCONN);
		if(ret == -1)
		{
			close(m_listen_socket);
			return -1;
		}
		PLOG_INFO("listen port:%d", local_port);
	}

    return 0;
}

bool TcpAsync::Start()
{
	PLOG_DEBUG("tcp comm thread start ...");
	pthread_t thread_id;

	if (::pthread_create(&thread_id, NULL, TcpAsyncWriteThreadEntry, this) != 0)
	{
		PLOG_ERROR("start TcpAsyncWriteThreadEntry failed");
		return false;
	}
	PLOG_DEBUG("start TcpAsyncWriteThreadEntry success");
	if (::pthread_create(&thread_id, NULL, TcpAsyncReadThreadEntry, this) != 0)
	{
		PLOG_ERROR("start TcpAsyncReadThreadEntry failed");
		return false;
	}
	PLOG_DEBUG("start TcpAsyncReadThreadEntry success");

	return true;
}

void TcpAsyncSimplex::Write()
{
	string send_buff;
	TcpSocket tcp_socket;

	while(1)
	{
		DoBeforeConnect();
		if (!m_connect_stat)
		{
			PLOG_DEBUG("start to connect..., ip=%s, port=%d", m_remote_ip.c_str(), m_remote_port);
			//cout << "start to connect..., ip=" << m_remote_ip << ", port=" << m_remote_port << endl;
			tcp_socket.Close();
			while(1)
			{
				//if (tcp_socket.Connect(m_remote_ip, m_remote_port) != 0)
				if (tcp_socket.ConnectWithTimeout(m_remote_ip, m_remote_port, 1000) != 0)
				{
					PLOG_ERROR("connect server failed, ip=%s, port=%d", m_remote_ip.c_str(), m_remote_port);
					//sleep(CONNECT_INTERVAL);
					continue;
				}
				break;
			}
			PLOG_INFO("connect server success, ip=%s, port=%d", m_remote_ip.c_str(), m_remote_port);
			//cout << "connect success, ip=" << m_remote_ip << ", port=" << m_remote_port << endl;
			m_connect_stat = true;
			tcp_socket.SetNonBlocking();
			DoAfterConnect();
		}

		if (DoBeforeSend(send_buff) != 0)
		{
			continue;
		}

		if (tcp_socket.Write(send_buff) != 0)
		{
			PLOG_ERROR("send msg failed");
			m_connect_stat = false;
			tcp_socket.Close();
		}
		else
        {
            PLOG_INFO("send msg success");
        }
		DoAfterSend();
	}
}

void TcpAsyncSimplex::Read()
{
	int i;
	int sock;
	int epoll_count;
	string data_buff;
	string len_buff;
	TcpSocket tcp_socket;
	struct epoll_event ev_reg, ev_wait[EPOLL_EVENT_COUNT];;

	while(1)
	{
		DoBeforeAccept();
		if (!m_accept_stat)
		{
			PLOG_DEBUG("start to accept...");
            //cout << "start to accept..." << endl;
			while(1)
			{
				if (tcp_socket.Accept(m_listen_socket) != 0)
				{
					PLOG_ERROR("accept failed");
					continue;
				}
				break;
			}
			PLOG_INFO("accept success, client ip=%s", tcp_socket.GetClientIP().c_str());
			//cout << "accept success, client ip=" << tcp_socket.GetClientIP() << endl;
			m_accept_stat = true;
			tcp_socket.SetNonBlocking();

			sock = tcp_socket.GetSocket();
			ev_reg.data.fd = sock;
			//ev_reg.events = EPOLLIN | EPOLLET;
			ev_reg.events = EPOLLIN;
			epoll_ctl(m_epoll_handle, EPOLL_CTL_ADD, sock, &ev_reg);

			DoAfterAccept();
		}

        epoll_count = epoll_wait(m_epoll_handle, ev_wait, EPOLL_EVENT_COUNT, m_timeout_ms);
		for(i=0; i<epoll_count; i++)
		{
			if ( (ev_wait[i].data.fd == sock)
				&& (ev_wait[i].events & EPOLLIN) )//如果是已经连接的用户，并且收到数据，那么进行读入。
			{
				if (DoBeforeRecv() != 0)
				{
					break;
				}
				len_buff = "";
				data_buff = "";
				if (tcp_socket.Read(len_buff, m_len_buff_size) != 0)
				{
					PLOG_ERROR("read len failed");
					m_accept_stat = false;
					epoll_ctl(m_epoll_handle, EPOLL_CTL_DEL, sock, NULL);
					tcp_socket.Close();
					DoAfterRecv("");
					break;
				}
				int data_len = CalcMsgSize(len_buff);
				PLOG_DEBUG("data len is %d", data_len);
                if (data_len <= 0)
                {
                    PLOG_INFO("recv msg success");
                    DoAfterRecv(len_buff);
                    break;
                }
				if (tcp_socket.Read(data_buff, data_len) != 0)
				{
					PLOG_ERROR("read data failed");
					m_accept_stat = false;
					epoll_ctl(m_epoll_handle, EPOLL_CTL_DEL, sock, NULL);
					tcp_socket.Close();
					DoAfterRecv("");
					break;
				}
				PLOG_INFO("recv msg success");
				DoAfterRecv(len_buff+data_buff);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////
TcpAsyncDuplexClient::TcpAsyncDuplexClient()
{
	pthread_mutex_init(&m_mutex, NULL);

	pthread_condattr_t cond_attr;
	pthread_condattr_init(&cond_attr);
	pthread_cond_init(&m_cond, &cond_attr);
	pthread_condattr_destroy(&cond_attr);
}
TcpAsyncDuplexClient::~TcpAsyncDuplexClient()
{
	pthread_mutex_destroy(&m_mutex);

	pthread_cond_destroy(&m_cond);
}

void TcpAsyncDuplexClient::WaitConnectStat(bool connect_stat)
{
	pthread_mutex_lock(&m_mutex);
	while(m_connect_stat != connect_stat)
	{
		pthread_cond_wait(&m_cond, &m_mutex);
	}
	pthread_mutex_unlock(&m_mutex);
}
void TcpAsyncDuplexClient::SetConnectStat(bool connect_stat)
{
	pthread_mutex_lock(&m_mutex);
	m_connect_stat = connect_stat;
	pthread_mutex_unlock(&m_mutex);
	pthread_cond_broadcast(&m_cond);
}

void TcpAsyncDuplexClient::Write()
{
	//int ret;
	int sock;
	string send_buff;
	struct epoll_event ev_reg;

	while(1)
	{
		DoBeforeConnect();

		if (!m_connect_stat)
		{
			sock = m_tcp_socket.GetSocket();
			if (sock > 0)
			{
				epoll_ctl(m_epoll_handle, EPOLL_CTL_DEL, sock, NULL);
			}
			m_tcp_socket.Close();
			while(1)
			{
			    PLOG_DEBUG("start to connect..., ip=%s, port=%d", m_remote_ip.c_str(), m_remote_port);
				//cout << "start to connect ..., ip=" << m_remote_ip << ", port=" << m_remote_port << endl;
				if (m_tcp_socket.Connect(m_remote_ip, m_remote_port) != 0)
				{
					PLOG_ERROR("connect server failed, ip=%s, port=%d", m_remote_ip.c_str(), m_remote_port);
					sleep(CONNECT_INTERVAL);
					continue;
				}
				break;
			}
			PLOG_INFO("connect server success, ip=%s, port=%d", m_remote_ip.c_str(), m_remote_port);
			//cout << "connect success, ip=" << m_remote_ip << ", port=" << m_remote_port << endl;
			SetConnectStat(true);
			m_tcp_socket.SetNonBlocking();

			sock = m_tcp_socket.GetSocket();
			ev_reg.data.fd = sock;
			//ev_reg.events = EPOLLIN | EPOLLET;
			ev_reg.events = EPOLLIN;
			epoll_ctl(m_epoll_handle, EPOLL_CTL_ADD, sock, &ev_reg);

			DoAfterConnect();
		}

		if (DoBeforeSend(send_buff) != 0)
		{
			continue;
		}

		if (m_tcp_socket.Write(send_buff) != 0)
		{
			PLOG_ERROR("send msg failed");
			SetConnectStat(false);
		}
		DoAfterSend();
	}
}

void TcpAsyncDuplexClient::Read()
{
	int i;
	//int ret;
	int epoll_count;
	struct epoll_event ev_wait[EPOLL_EVENT_COUNT];
	string data_buff;
	string len_buff;
	PLOG_DEBUG("start to read...");

	while(1)
	{
		WaitConnectStat(true);

		epoll_count = epoll_wait(m_epoll_handle, ev_wait, EPOLL_EVENT_COUNT, m_timeout_ms);
		for(i=0; i<epoll_count; i++)
		{
			if ( (ev_wait[i].data.fd == m_tcp_socket.GetSocket())
				&& (ev_wait[i].events & EPOLLIN) )//如果是已经连接的用户，并且收到数据，那么进行读入。
			{
				if (DoBeforeRecv() != 0)
				{
					break;
				}
				len_buff = "";
				data_buff = "";
				if (m_tcp_socket.Read(len_buff, m_len_buff_size) != 0)
				{
					PLOG_ERROR("read len failed");
					SetConnectStat(false);
					DoAfterRecv("");
					break;
				}
				int data_len = CalcMsgSize(len_buff);
				PLOG_DEBUG("data len is %d", data_len);
                if (data_len <= 0)
                {
                    PLOG_INFO("recv msg success");
                    DoAfterRecv(len_buff);
                    break;
                }
				if (m_tcp_socket.Read(data_buff, data_len) != 0)
				{
                    PLOG_ERROR("read data failed");
					SetConnectStat(false);
					DoAfterRecv("");
					break;
				}
				PLOG_INFO("recv msg success");
				DoAfterRecv(len_buff+data_buff);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////
TcpAsyncDuplexServer::TcpAsyncDuplexServer()
{
	pthread_mutex_init(&m_mutex, NULL);

	pthread_condattr_t cond_attr;
	pthread_condattr_init(&cond_attr);
	pthread_cond_init(&m_cond, &cond_attr);
	pthread_condattr_destroy(&cond_attr);
}
TcpAsyncDuplexServer::~TcpAsyncDuplexServer()
{
	pthread_mutex_destroy(&m_mutex);

	pthread_cond_destroy(&m_cond);
}

void TcpAsyncDuplexServer::WaitAcceptStat(bool accept_stat)
{
	pthread_mutex_lock(&m_mutex);
	while(m_accept_stat != accept_stat)
	{
		pthread_cond_wait(&m_cond, &m_mutex);
	}
	pthread_mutex_unlock(&m_mutex);
}
void TcpAsyncDuplexServer::SetAcceptStat(bool accept_stat)
{
	pthread_mutex_lock(&m_mutex);
	m_accept_stat = accept_stat;
	pthread_mutex_unlock(&m_mutex);
	pthread_cond_broadcast(&m_cond);
}

void TcpAsyncDuplexServer::Write()
{
	//int ret;
	string send_buff;

	while(1)
	{
		WaitAcceptStat(true);

		if (DoBeforeSend(send_buff) != 0)
		{
			continue;
		}

		if (m_tcp_socket.Write(send_buff) != 0)
		{
			PLOG_ERROR("send msg failed");
			SetAcceptStat(false);
		}
		DoAfterSend();
	}
}

void TcpAsyncDuplexServer::Read()
{
	int i;
	int sock;
	int epoll_count;
	string data_buff;
	string len_buff;
	struct epoll_event ev_reg, ev_wait[EPOLL_EVENT_COUNT];;

	while(1)
	{
		DoBeforeAccept();

		if (!m_accept_stat)
		{
			while(1)
			{
			    PLOG_DEBUG("start to accept...");
				//cout << "start to accept ..." << endl;
				if (m_tcp_socket.Accept(m_listen_socket) != 0)
				{
					PLOG_ERROR("accept failed");
					continue;
				}
				break;
			}
			PLOG_INFO("accept success, client ip=%s", m_tcp_socket.GetClientIP().c_str());
			//cout << "accept success, client ip=" << m_tcp_socket.GetClientIP() << endl;
			SetAcceptStat(true);
			m_tcp_socket.SetNonBlocking();

			sock = m_tcp_socket.GetSocket();
			ev_reg.data.fd = sock;
			//ev_reg.events = EPOLLIN | EPOLLET;
			ev_reg.events = EPOLLIN;
			epoll_ctl(m_epoll_handle, EPOLL_CTL_ADD, sock, &ev_reg);

			DoAfterAccept();
		}

		epoll_count = epoll_wait(m_epoll_handle, ev_wait, EPOLL_EVENT_COUNT, m_timeout_ms);
		for(i=0; i<epoll_count; i++)
		{
			if ( (ev_wait[i].data.fd == sock)
				&& (ev_wait[i].events & EPOLLIN) )//如果是已经连接的用户，并且收到数据，那么进行读入。
			{
				if (DoBeforeRecv() != 0)
				{
					break;
				}
				len_buff = "";
				data_buff = "";
				if (m_tcp_socket.Read(len_buff, m_len_buff_size) != 0)
				{
					PLOG_ERROR("read len failed");
					SetAcceptStat(false);
					epoll_ctl(m_epoll_handle, EPOLL_CTL_DEL, sock, NULL);
					m_tcp_socket.Close();
					DoAfterRecv("");
					break;
				}
				int data_len = CalcMsgSize(len_buff);
				PLOG_DEBUG("data len is %d", data_len);
                if (data_len <= 0)
                {
                    PLOG_INFO("recv msg success");
                    DoAfterRecv(len_buff);
                    break;
                }
				if (m_tcp_socket.Read(data_buff, data_len) != 0)
				{
					PLOG_ERROR("read data failed");
					SetAcceptStat(false);
					epoll_ctl(m_epoll_handle, EPOLL_CTL_DEL, sock, NULL);
					m_tcp_socket.Close();
					DoAfterRecv("");
					break;
				}
				PLOG_INFO("recv msg success");
				DoAfterRecv(len_buff+data_buff);
			}
		}
	}
}

