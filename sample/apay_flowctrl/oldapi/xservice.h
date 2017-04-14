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

#define MAX_XSVC_PROCESS    64				//ÿ�����������֧�ֵ��ӽ�����
#define MAX_SERVICE_NUM     64				//ÿ���ӽ������֧�ֵķ�����
#define MAX_THREADS_NUM     16				//ÿ���������Ĵ����߳���

#define MAX_XSVC_NAME       64				//[��������|������]����󳤶�
#define MAX_XSVC_CID        13				//[�����߱�ʶ]��󳤶�
#define MAX_XSVC_DATALEN    7870			//�ڵ����ݻ����С(7870 = 8 * 1024 - 312)

//XSERVICE���ݽڵ�
#pragma pack(8)
typedef struct _XSVC_DATA {
	char xsvc_srvr[MAX_XSVC_NAME];          //Ŀ��[��������]
	char xsvc_name[MAX_XSVC_NAME];          //Ŀ��[������]
	char xsvc_cid[MAX_XSVC_CID];            //[�����߱�ʶ]
	unsigned char data[MAX_XSVC_DATALEN];   //[��Ϣ]����
	int data_len;                           //[��Ϣ]����
	int xsvc_icd;                           //[��Ϣ]��ʶ(icd)
	int xsvc_pid;                           //�ض�Ŀ����ս���(pid, ����0ʱ��Ϣ�����͸�ָ���Ľ���)
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
 *�ص�����:
 *  ������յ�����ʱ�ص�����.
 *����:
 *  XSVC_DATA - ����
 *����ֵ:
 *  0 - ����ȴ�������(�����ڴ���У��������ͷŽڵ�)
 *  1 - ��Ҫ�ȴ�������(�����ڴ���У��ڵ㱣�ֲ��ͷţ�ֱ�����ݱ���ȡ, ��60s��ʱ)
 */
typedef int (WINAPI* LPFN_XSVC_DATA)(const XSVC_DATA *xsvc_data);   //���յ�����

/*
 *�ص�����:
 *  �ӽ�������/ֹͣʱ�ص�����.
 *����:
 *  child_pid  - �ӽ���PID
 *  event_mode - �¼�ģʽ:
 *      0: �ӽ���ֹͣ.
 *      1: �ӽ�������.
 *      2: �쳣����.
 *      3: ���ؾ�������ģʽ�£���������������������������������ʧ��(ע:ÿ���ӱ���1�Σ�ִ����ϵ�ɹ�).
 *����ֵ: ��
 */
typedef void (WINAPI* LPFN_XSVC_CHILD)(const int child_pid, const int event_mode);


/**------------------------------ XSERVICE ����API(������) -------------------------**/
/*
 *��ʼ������������(������API)
 *����:
 *  xsvc_server[in]   - ��������ʶ(ÿ�������̷�������������Ψһ��"����������"��ʶ)
 *  xsvc_process[in]  - �����ӽ�����(���ֵ: 64)
 *  xsvc_child_cb[in] - �ӽ�������/ֹͣʱ�Ļص�����
 *  xsvc_timeout[in]  - �������ݳ�ʱʱ��(Ĭ��ֵ: 60��)
 *  xsvc_udpaddr[in]  - ���ؾ���UDP�鲥��ַ(224.0.1.0��238.255.255.255)
 *  xsvc_udpport[in]  - ���ؾ���UDP�鲥�˿�
 *����:
 *  0 - ʧ��
 * >0 - ������PID
 * <0 - �ӽ���PID
 *��ע:
 * 1) ����������fork, һ�ε��ã���η���(������/�ӽ���PID).
 * 2) ���������������̵���!
 * 3) ��ָ��xsvc_udpaddr/xsvc_udpport��������ȡ��[���ؾ���]����.
 */
DLL_IMPORT int xsvc_init(const char *xsvc_server,
                         int xsvc_process = 1,
                         LPFN_XSVC_CHILD xsvc_child_cb = NULL,
                         int xsvc_timeout = 60,
                         char *xsvc_udpaddr = NULL,
                         unsigned xsvc_udpport = 0);

/*
 *ֹͣ����������(������API)
 *����:   ��
 *����ֵ: ��
 */
DLL_IMPORT void xsvc_free();

/*
 *��ȡע����Ϣ(������API)
 *�������:
 *  serial_no[16] - ���к�
 *  reg_user[64]  - �û���
 *����:
 *  -1: �ѹ���
 *   0: ������
 *   1: ��ע��
 */
DLL_IMPORT int xsvc_register_info(char *serial_no, char *reg_user);


/**-------------------------------- XSERVICE ����API(�ӽ���) -----------------------**/
/*
 *ע��/��������(�ӽ���API)
 *����:
 *  xsvc_name[in]     - ������
 *  xsvc_threads[in]  - �����߳���(����ָ������: xsvc_name)
 *  xsvc_data_cb[in]  - ��������ʱ�ص�����
 *  xsvc_sysclass[in] - ������д�С(ÿ��[����], һ������):
 *                      0 - �д�ϵͳ(���л���:128M, ���нڵ���: 10000, ȱʡֵ)
 *                      1 - ����ϵͳ(���л���:256M�����нڵ���: 20000)
 *                      2 - ����ϵͳ(���л���:512M, ���нڵ���: 40000)
 *����ֵ: ��
 */
DLL_IMPORT bool xsvc_open(const char *xsvc_name,
                          const int xsvc_threads = 1,
                          const LPFN_XSVC_DATA xsvc_data_cb = NULL,
                          const int xsvc_sysclass = 0);

/*
 *ֹͣ����(�ӽ���API)
 *����:
 *  xsvc_name[in] - ������
 *����ֵ: ��
 */
DLL_IMPORT void xsvc_close(const char *xsvc_name);

/*
 *���ط�����Ӧ����(�ӽ���API)
 *����:
 *  xsvc_data - ��������
 *����ֵ:
 *  true - �ɹ�
 *  false - ʧ��
 */
DLL_IMPORT bool xsvc_return(XSVC_DATA *xsvc_data);


/**------------------------------ XSERVICE �ͻ���API -----------------------------**/
/*
 *��ʼ���ͻ��˻���.
 *����:
 *  xsvc_udpaddr[in]  - ���ؾ���UDP�鲥��ַ(224.0.1.0��238.255.255.255)
 *  xsvc_udpport[in]  - ���ؾ���UDP�鲥�˿�
 *  local_weight[in]  - ͬʱ���ڱ���/Զ�̷���ʱ�����ñ��ط����Ȩ��;
 *                      (ȡֵ:1~9, ��ȡֵ:7 ��:����[���ط���:Զ�̷���]�ı���Ϊ[70%:30%];
 *                      ������ڶ��Զ�̷�������, ���Զ�ѡ��һ��CPUռ�õ͵�����)
 *��ע:
 *  ��ָ��xsvc_udpaddr/xsvc_udpport��������xsvc_call/xsvc_acall/xsvc_forward�����ñ��ط���
 *  ������Զ��[���ؾ���]����, ���: xsvc_init����˺���.
 */
DLL_IMPORT void xsvc_init_client(char *xsvc_udpaddr = NULL,
                                 unsigned xsvc_udpport = 0,
                                 int local_weight = 7);

/*
 *�ͷſͻ��˻���.
 */
DLL_IMPORT void xsvc_free_client();

/*
 *ͬ�����÷���(�ͻ���API)
 *����:
 *  xsvc_server[in] - Ŀ���������ʶ
 *  xsvc_name[in]   - Ŀ�������
 *  data[in/out]    - ��������/��Ӧ����
 *  len[in/out]     - ���ݳ���
 *  timeout[in]     - ��ʱ(��λ:��; Ϊ0ʱ��ȡĬ��ֵ: 60s)
 *  xsvc_pid[in]    - Ŀ�������(Ŀ�������̵��ӽ���pid��Ĭ��ֵ:0)
 *                    xsvc_pid==0ʱ����Ŀ�������̵������ӽ��̴���
 *  xsvc_cid[in]    - �����߱�ʶ(������[XSVC_DATA]����Ŀ��������)
 *����ֵ:
 *  1 - �����ҽ��ճɹ�
 *  0 - ���ͳɹ�, ���ճ�ʱ
 * -1 - ���ͳɹ�, ����ʧ��
 * -2 - ����ʧ�� (��������ķ��񲻴���)
 * -3 - ����ʧ�� (���ؾ���)
 * -4 - ��������
 */
DLL_IMPORT int xsvc_call(const char *xsvc_server, const char *xsvc_name,
                         unsigned char *data, int *len, int timeout = 60,
                         int xsvc_pid = 0, char *xsvc_cid = NULL);

/*
 *�첽���÷���(�ͻ���API)
 *����:
 *  xsvc_server[in]  - Ŀ���������
 *  xsvc_name[in]    - Ŀ�������
 *  data[in]         - ������������
 *  len[in]          - ���ݳ���
 *  xsvc_pid[in]     - Ŀ�������(Ŀ�������̵��ӽ���pid��Ĭ��ֵ:0)
 *                     xsvc_pid==0ʱ����Ŀ�������̵������ӽ��̴���
 *  xsvc_cid[in]     - �����߱�ʶ(������[XSVC_DATA]����Ŀ��������)
 *����ֵ:
 *  >0 - ����icd(������ӦID), ���ṩxsvc_getreply()�����첽��ȡ������Ӧ����.
 *  -1 - ʧ��
 */
DLL_IMPORT int xsvc_acall(const char *xsvc_server, const char *xsvc_name,
                          unsigned char *data, int len,
                          int xsvc_pid = 0, char *xsvc_cid = NULL);

/*
 *��ѯ������Ӧ����(�ͻ���API)
 *����:
 *  xsvc_server[in]  - Ŀ���������ʶ
 *  xsvc_icd[in]     - ������ӦID(xsvc_acall�ķ���ֵ)
 *  data[out]        - ������Ӧ����
 *  len[out]         - ���ݳ���
 *����ֵ:
 *  true  - �ɹ�
 *  false - ʧ��
 */
DLL_IMPORT bool xsvc_getreply(const char *xsvc_server, const int xsvc_icd,
                              unsigned char *data, int *len);

/*
 *�첽����ת������(�ͻ���API)
 *����:
 *  xsvc_server[in] - Ŀ���������ʶ
 *  xsvc_name[in]   - Ŀ�������
 *  xsvc_data[in]   - Դ����
 *����ֵ:
 *  >=0 - �ɹ�(forward_icd)
 *   <0 - ʧ��(-1:������/-2:�������ʧ��/-3:Ŀ����񲻴���)
 */
DLL_IMPORT int  xsvc_forward(const char *xsvc_server, const char *xsvc_name, XSVC_DATA *xsvc_data);
//----------------------------------------------------------------------------


#endif  // _xservice_H_
