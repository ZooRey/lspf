/**
 *Description:
 *  �����ʶ����.
 *Author:
 *   24291878@qq.com
 *Copyright:
 *   Copyright(C) 2013.04
 *History:
 *TODO:
**/

#ifndef _xsvcs_accounts_H
#define _xsvcs_accounts_H

#define BANKCODE_MPAY           "100"                //MPAY����(����)
#define BANKCODE_UNIONPAY       "101"                //��������ȫ����
#define BANKCODE_YIBAO          "023"                //�ױ�֧��
#define BANKCODE_YIBAO_QK       "023"                //�ױ����֧��
#define BANKCODE_MINSHWEB          "088"             //������������

#define BANKCODE_BACKFILE      	"1001"               //�����ļ���������
#define PART_PAYMENT_OK     (char *)"{\"result\":\"part_pay_true\"}"    //���ָ���ɹ�

#define BALANCE_LACK_ERR	(char *)"{\"result\":\"balance_lack\"}"     //������������

//���ʷ���
#define FEE_APPS            (char *)"FEE_SVC_SERVER"            //���ʷ�����
#define FEE_TRANS           (char *)"FEE_SVC_TRANS"             //���ʷ���


//��������
#define ACC_CONSUMPTION        (char *)"ACC_CONSUMPTION"            //��������ϵͳ��
#define ACC_CONSUMPTION_SVC    (char *)"ACC_CONSUMPTION_SVC"        //��������ϵͳ����

//����
#define ACC_FINANCE         (char *)"ACC_FINANCE_SERVER"        //����ϵͳ��
#define ACC_ACCOUNTS        (char *)"ACC_SVC_ACCOUNTS"          //����ϵͳ����

#define ACC_DATA            (char *)"ACC_DATA"                  //����ϵͳ��
#define ACC_DATA_SVC        (char *)"ACC_DATA_SVC"              //����ϵͳ����

//����
#define ACC_RECON           (char *)"ACC_RECON"                 //����ϵͳ��
#define ACC_RECON_LIST      (char *)"ACC_RECON_LIST"            //����ϵͳ����


//���ɶ��˵�
#define ACC_ACCCOUNTLIST          (char *)"ACC_ACCCOUNTLIST"        //���ɶ��˵�������
#define ACC_ACCCOUNTLIST_SVC      (char *)"ACC_ACCCOUNTLIST_SVC"    //���ɶ��˵�����

//��������ӿ�
#define ACC_PAYMENT               (char *)"ACC_PAYMENT"             //����
#define ACC_PAYMENT_SVC           (char *)"ACC_PAYMENT_SVC"         //����ӿ�
#define ACC_PAYMENTQUERY_SVC      (char *)"ACC_PAYMENTQUERY_SVC"    //��ѯ�ӿ�


//����ͨ���ڲ�web����
#define ACC_PAYCHANNEL           (char *)"ACC_PAYCHANNEL"           //�ڲ����õĸ���ͨ������
#define ACC_PAYCHANNEL_SVC       (char *)"ACC_PAYCHANNEL_SVC"
#define ACC_PAYCHANNELQUERY_SVC  (char *)"ACC_PAYCHANNELQUERY_SVC"

//����ʵʱ������ƽ̨
#define NAC_CMBCPAYMENT          (char *)"NAC_CMBCPAYMENT"          //����ʵʱ�����������
#define NAC_CMBCPAYMENT_SVC      (char *)"NAC_CMBCPAYMENT_SVC"      //����ʵʱ���������
#define NAC_CMBCPAYMENTQUERY_SVC (char *)"NAC_CMBCPAYMENTQUERY_SVC" //����ʵʱ�������ѯ����
#define NAC_CMBCBALANCEQRY_SVC (char *)"NAC_CMBCBALANCEQRY_SVC" //�����Թ�����ѯ����
#define NAC_CMBCPARTNEQRY_SVC (char *)"NAC_CMBCPARTNEQRY_SVC" //�����Թ�����ѯ����

//�������˷���
#define ACC_CMBCCHECK         (char *)"ACC_CMBCCHECK"         //�������ж��˷�����
#define ACC_CMBCCHECK_SVC     (char*)"ACC_CMBCCHECK_SVC"      //�������ж��˷���
//��������ʵ����֤
#define NAC_CMBCCERTIFICATE          (char *)"NAC_CMBCCERTIFICATE"          //����ʵ����֤������
#define NAC_CMBCCERTIFICATE_SVC      (char *)"NAC_CMBCCERTIFICATE_SVC"      //��������ʵ����֤����
#define NAC_CMBCCERTIFICATEQUERY_SVC (char *)"NAC_CMBCCERTIFICATEQUERY_SVC" //��������ʵ����֤��ѯ����

//��������ʵ����֤(����֤)
#define NAC_XLZXCERTIFICATE          (char *)"NAC_XLZXCERTIFICATE"          //����������֤������
#define NAC_XLZXCERTIFICATE_SVC      (char *)"NAC_XLZXCERTIFICATE_SVC"      //��������ʵ����֤����
#define NAC_XLZXCERTIFICATEQUERY_SVC (char *)"NAC_XLZXCERTIFICATEQUERY_SVC" //��������ʵ����֤��ѯ����

//�������������֤
#define NAC_XLZXIDENTITY          (char *)"NAC_XLZXIDENTITY"          //�������������֤������
#define NAC_XLZXIDENTITY_SVC      (char *)"NAC_XLZXIDENTITY_SVC"      //�������������֤����
#define NAC_XLZXIDENTITYQUERY_SVC (char *)"NAC_XLZXIDENTITYQUERY_SVC" //�������������֤��ѯ����

//�������д���
#define NAC_CMBCWITHHOLD          (char *)"NAC_CMBCWITHHOLD"
#define NAC_CMBCWITHHOLD_SVC      (char *)"NAC_CMBCWITHHOLD_SVC"
#define NAC_CMBCWITHHOLDQUERY_SVC (char *)"NAC_CMBCWITHHOLDQUERY_SVC"
//����ʵʱ������
#define NAC_CITICPAYMENT          	(char *)"NAC_CITICPAYMENT"          	//����ʵʱ�����������
#define NAC_CITICPAYMENT_SVC      	(char *)"NAC_CITICPAYMENT_SVC"      	//����ʵʱ���������
#define NAC_CITICPAYMENTQUERY_SVC 	(char *)"NAC_CITICPAYMENTQUERY_SVC" 	//����ʵʱ�������ѯ����
#define NAC_CITICPAY_AUDIT_OPT_SVC  (char *)"NAC_CITICPAY_AUDIT_OPT_SVC" 	//��������ִ�з���
#define NAC_CITICPAYMENTFILEDWN_SVC	(char *)"NAC_CITICPAYMENTFILEDWN_SVC"	//���Ŷ����ļ�����

#define NAC_TJ_UPAY                 (char *)"NAC_TJ_UPAY"               //�����������������
#define NAC_TJ_UPAY_PAY_SVC         (char *)"NAC_TJ_UPAY_PAY_SVC"       //�������ʵʱ������������
#define NAC_TJ_UPAY_PAYQRY_SVC      (char *)"NAC_TJ_UPAY_PAYQRY_SVC"    //�������ʵʱ����������ѯ
#define NAC_TJ_UPAY_DOWNLOAD_SVC    (char *)"NAC_TJ_UPAY_DOWNLOAD_SVC"  //��������������˵�����
#define NAC_TJ_UPAY_NOTIFY_SVC      (char *)"NAC_TJ_UPAY_NOTIFY_SVC"  //��������������˵�����


#define ACCP_PAYRESULT	    (char *)"ACCP_PAYRESULT"	//���Ŷ����ļ�����
#define ACCP_PAYRESULT_UPDATE_SVC	    (char *)"ACCP_PAYRESULT_UPDATE_SVC"	//���Ŷ����ļ�����

//���ÿ�����
#define ACC_CREDITCARDREPAY         (char *)"ACC_CREDITCARDREPAY"       //���ÿ����������
#define ACC_CREDITCARDREPAY_SVC     (char *)"ACC_CREDITCARDREPAY_SVC"   //���ÿ������������
#define ACC_CRDREPAYRESULT_SVC      (char *)"ACC_CRDREPAYRESULT_SVC"    //���ÿ���������ѯ
//����D+0�����������������
#define ACC_CALADDFEE       (char *)"ACC_CALADDFEE"         //����D+0����������������ѷ�����
#define ACC_CALADDFEE_SVC   (char *)"ACC_CALADDFEE_SVC"     //����D+0����������������ѷ���
//�Զ���ֵ
#define ACC_AUTORECHARGE       (char *)"ACC_AUTORECHARGE"         //�Զ���ֵ������
#define ACC_AUTORECHARGE_SVC   (char *)"ACC_AUTORECHARGE_SVC"     //�Զ���ֵ����

//�������������Ĵ���
#define NAC_SSC             (char *)"NAC_SSC"                   //����
#define NAC_SSC_SVC         (char *)"NAC_SSC_SVC"               //����ӿ�
#define NAC_SSC_SVCQRY_SVC  (char *)"NAC_SSC_SVCQRY_SVC"        //����ӿ�

#define  SSC_SINGLEPAY      (char*)"11"                         //���ʴ���
#define  SSC_SINGLEACP      (char*)"12"                         //���ʴ���
#define  SSC_BATCH_PAY      (char*)"21"                         //��������
#define  SSC_BATCH_ACP      (char*)"22"                         //��������
#define  SSC_BIZTQRY_S      (char*)"31"                         //���ʲ�ѯ
#define  SSC_BIZTQRY_B      (char*)"32"                         //������ѯ

//����
#define ACC_PAYABC          (char *)"ACC_PAYABC"
#define ACC_PAYABC_SVC      (char*)"ACC_PAYABC_SVC"             //��ʱ����


#define ACC_APPS            (char *)"ACC_SVC_SERVER"            //ACCOUNTING ϵͳ����
#define ACC_RECORD          (char *)"ACC_SVC_RECORD"            //ACCOUNT ��ƴ�Ʊ����
//------------------------------------------------------------------------------
#define ACC_SVR             (char *)"ACC_BALANCE_SERVER"        //ACCOUNT ��ʱ����
#define ACC_BALANCE         (char *)"ACC_SVC_BALANCE"           //ACCOUNT ������������

//------------------------------------------------------------------------------
#define UTILS_IPDBQUERY_SVR (char *)"UTILS_IPDBQUERY_SERVER"    //IPDBQuery����
#define UTILS_IPDBQUERY     (char *)"UTILS_IPDBQUERY"           //IPDBQuery����

//���
#define RISK_CONTROL        (char *)"RISK_CONTROL"              //���ϵͳ������
#define RISK_CONTROL_SVC    (char *)"RISK_CONTROL_SVC"          //���ϵͳ����

//����Ϊ�˱���ͨ����hsb
#define RISK_CTRL_APPS      (char *)"RISK_CTRL_SERVER"          //��ط���
#define RISK_CTRL_REALTIME  (char *)"RISK_CTRL_REALTIME"        //ʵʱ���


//����
#define ACC_STATEMENT       (char *)"ACC_STATEMENT"
#define ACC_STATEMENT_SVC   (char *)"ACC_STATEMENT_SVC"         //����ʵʱ����

//֧��������
#define ACC_STATEMENT_ALIPAY       (char *)"ACC_STATEMENT_ALIPAY"
#define ACC_STATEMENT_ALIPAY_SVC   (char *)"ACC_STATEMENT_ALIPAY_SVC"  //����ʵʱ����

//mpay����
#define ACC_STATEMENT_MPAY       (char *)"ACC_STATEMENT_MPAY"
#define ACC_STATEMENT_MPAY_SVC   (char *)"ACC_STATEMENT_MPAY_SVC"  //����ʵʱ����

//΢�Ŷ���
#define ACC_WEIXINCHECK       (char *)"ACC_WEIXINCHECK"
#define ACC_WEIXINCHECK_SVC   (char *)"ACC_WEIXINCHECK_SVC"  //����ʵʱ����

//�������
#define ACC_CLEAR           (char *)"ACC_CLEAR"                 //��ַ�����
#define ACC_CLEAR_SVC       (char *)"ACC_CLEAR_SVC"             //���ʵʱ����
#define ACC_CLEAR_EXPORT_SVC (char *)"ACC_CLEAR_EXPORT_SVC"     //���������ļ�ʵʱ����


#define ACC_CLEAR_FCGI      (char *)"ACC_CLEAR_FCGI"
#define ACC_CLEAR_FCGI_SVC  (char *)"ACC_CLEAR_FCGI_SVC"

//�����ѯ
#define ACC_QUERY           (char *)"ACC_QUERY"                 //�˻���ѯ������
#define ACC_QUERY_SVC       (char *)"ACC_QUERY_SVC"             //�˻���ѯ����

//������־
#define ACC_WITHDRAW        (char *)"ACC_WITHDRAW"              //���ַ�����
#define ACC_WITHDRAW_SVC    (char *)"ACC_WITHDRAW_SVC"          //���ַ���

//����֤
#define ACC_CARDCERT        (char *)"ACC_CARDCERT"              //����֤������
#define ACC_CARDCERT_SVC    (char *)"ACC_CARDCERT_SVC"          //����֤����

//T+1������������
#define ACC_WDCMBC          (char *)"ACC_WDCMBC"                //������������������
#define ACC_WDCMBC_SVC      (char *)"ACC_WDCMBC_SVC"            //����������������

//D+0������������
#define ACC_PAYREALTIME          (char *)"ACC_PAYREALTIME"                //D+0������������������
#define ACC_PAYREALTIME_SVC      (char *)"ACC_PAYREALTIME_SVC"            //D+0����������������


//������˷���
#define ACC_AUDIT      (char *)"ACC_AUDIT"                  //�������
#define ACC_AUDIT_SVC   (char *)"ACC_AUDIT_SVC"             //������˷���


//�����ѯ����
#define ACC_CLEARQUERY          (char *)"ACC_CLEARQUERY"            //�����ѯ����
#define ACC_CLEARQUERY_SVC      (char *)"ACC_CLEARQUERY_SVC"             //�����ѯ������


//�������ʣ�T+1Ӧ��Ӧ������
#define ACC_DAILY           (char *)"ACC_DAILY"                 //�������ʷ�����
#define ACC_DAILY_BILL      (char *)"ACC_DAILY_BILL"            //T+1Ӧ��Ӧ����������

//����
#define ACC_CHECKOUT        (char *)"ACC_CHECKOUT"              //���˷�����
#define ACC_CHECK_ACCOUNT   (char *)"ACC_CHECK_ACCOUNT"         //���˷���


#define ACC_PAY             (char *)"ACC_PAY"                   //����������
#define ACC_PAY_SVC         (char *)"ACC_PAY_SVC"               //��������


#define ACC_AGENCY_PAY_CTL      (char*)"ACC_AGENCY_PAY_CTL"        //�������������
#define ACC_AGENT_PAY_CTL_SVC   (char*)"ACC_AGENT_PAY_CTL_SVC"     //�����������


//���ɶ����ļ�����
#define ACC_TRANS           (char *)"ACC_TRANS"                 //���ɶ����ļ�
#define ACC_TRANS_SVC       (char *)"ACC_TRANS_SVC"             //���ɶ����ļ�����


//�����ļ��ӿ�
#define ACC_CLEARFILE       (char *)"ACC_CLEARFILE"             //�����ļ��ӿ�
#define ACC_CLEARFILE_SVC   (char *)"ACC_CLEARFILE_SVC"         //�����ļ��ӿڷ���


//��������ļ�����
#define ACC_RESPONCKECK     (char *)"ACC_RESPONCKECK"           //��������ļ����˷�����
#define ACC_RESPON_SVC      (char *)"ACC_RESPON_SVC"            //��������ļ����˷���


//���������ļ�����
#define ACC_CLEARCONVER     (char *)"ACC_CLEARCONVER"           //�������������
#define ACC_YL_SAVE         (char *)"ACC_CLEARCONVER_SVC"       //���δ����������(�����δ������ݱ��������ƽ̨)
#define ACC_CLEARCONVER_SVC (char *)"ACC_CLEARCONVER_SVC"       //�����ļ�ת������(�����������������ļ�ת���ɼ����������ļ�)


//(COMA)����һ�㽻����ˮ�ļ�����
#define ACC_COMA            (char *)"ACC_COMA"                  //������
#define ACC_COMA_SVR        (char *)"ACC_COMA_SVR"              //����

//�ױ�����
#define ACC_RECON_YIBAO      (char*)"ACC_RECON_YIBAO"            //�ױ����˷�����
#define ACC_RECONYIBAO_SVC   (char*)"ACC_RECONYIBAO_SVC"         //�ױ����˷���

//�����������ض���
#define ACC_RECON_MINSH      (char*)"ACC_RECON_MINSH"            //�����������ض��˷�����
#define ACC_RECONMINSH_SVC   (char*)"ACC_RECONMINSH_SVC"         //�����������ض��˷���


#define ACC_COUPONS         (char*)"ACC_COUPONS"                //�Ż݄�����������
#define ACC_COUPONS_SVC     (char*)"ACC_COUPONS_SVC"            //�Ż݄���������

#define ACC_FOUND           (char*)"ACC_FOUND"                  //�ʽ�鼯������
#define FOUND_COMESDOWN     (char*)"FOUND_COMESDOWN"            //�ʽ�鼯����

#define ACC_UNFREEZE_SVR    (char *)"ACC_UNFREEZE_SVR"          //�ⶳ����
#define ACC_UNFREEZE        (char *)"ACC_UNFREEZE"              //�ⶳ����


#endif // _gobal_xsvcs_H
