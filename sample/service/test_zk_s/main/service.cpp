#include "service.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <iostream>
#include <sys/types.h> /* about files */
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>

#include "common/app_conf.h"
#include "common/process_control.h"
#include "log/log.h"
#include "myserver.h"

static struct sigaction act;

bool Service::loadConfig(int argc, char *argv[])
{
    std::string filename;
    if (argc > 1){
        filename.assign(argv[1]);
    }else{
        filename = std::string(argv[0]) + ".ini";
    }

    return AppConf::Instance()->LoadConf(filename);
}

//�ػ�KILL/PKILL�ź�
void Service::catcher(int signo)
{
	//ֹͣ����
	MyServer::stop();
	exit(0);
}

//CTRL_C�˳�
void Service::trapPrompt(int signo)
{
	MyServer::stop();
	exit(0);
		
	char prompt[] = "Are you sure exit program now[y|n]? ";
	char op[80] = {0};
	std::cin.clear();
	std::cout.write(prompt, strlen(prompt));
	std::cin.read(op, 1);

	if ('y' == op[0] || 'Y' == op[0])
	{
		MyServer::stop();
		exit(0);
	}
}

void Service::trapSigint()
{
	struct sigaction act;
	act.sa_handler = trapPrompt;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;

	if (sigaction(SIGINT, &act, NULL) < 0)
	{
		PLOG_ERROR("Install signal action error: %s\n", strerror(errno));
		exit(1);
	}
}

//��װ�ػ�����
void Service::initDaemon(const std::string &app_name)
{
	int pid;
	//�����ն�I/O�ź�,STOP�ź�
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);

	if ((pid = fork()) > 0){
		exit(0);    //�Ǹ����̣�����������
	}else if (pid < 0){
		exit(1);    //forkʧ�ܣ��˳�
	}

	//�ǵ�һ�ӽ���,��̨����ִ��
	setsid();

	//������ն˷���
	if ((pid = fork()) > 0){
		exit(0);    //�ǵ�һ�ӽ��̣�������һ�ӽ���
	}else if (pid < 0){
		exit(1);    //forkʧ�ܣ��˳�
	}
    ///��������ظ�����
    if (!ProcessControl::AlreadyRunning()){
        std::cout << "process is already running!" << std::endl;
        exit(1);
    }

	signal(SIGCHLD, SIG_IGN);   //������ʬ���̣���ϵͳ����
	signal(SIGINT, SIG_IGN);    //����Control-C
	signal(SIGHUP, SIG_IGN);    //����LoggedOut
	//��ȡkill/pkill����
	act.sa_handler = catcher;
	act.sa_flags = 0;
	sigfillset(&(act.sa_mask));
	sigaction(SIGTERM, &act, NULL);     //KILL
	sigaction(SIGKILL, &act, NULL);     //PKILL

    char* chCurPath = getcwd(NULL, 0); // ��ǰ����Ŀ¼
	//���õ�ǰ����Ŀ¼
	chdir(chCurPath);
    std::cout << "chCurPath=" << chCurPath << std::endl;
    free(chCurPath);
}
//��װ��ͨ����
void Service::initNodaemon()
{
    trapSigint();
}

void Service::startService(int port)
{
	//��ʼ������
	if (!MyServer::init(port)){
		PLOG_FATAL("Server init failed!");
		return ;
	}

	//��ʼ�˿ڷ���
	if (!MyServer::start(port)){
		PLOG_FATAL("Server Start failed!");
		return ;
	}
}

