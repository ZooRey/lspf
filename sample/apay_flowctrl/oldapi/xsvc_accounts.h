/**
 *Description:
 *  服务标识定义.
 *Author:
 *   24291878@qq.com
 *Copyright:
 *   Copyright(C) 2013.04
 *History:
 *TODO:
**/

#ifndef _xsvcs_accounts_H
#define _xsvcs_accounts_H

#define BANKCODE_MPAY           "100"                //MPAY渠道(创新)
#define BANKCODE_UNIONPAY       "101"                //深圳银联全渠道
#define BANKCODE_YIBAO          "023"                //易宝支付
#define BANKCODE_YIBAO_QK       "023"                //易宝快捷支付
#define BANKCODE_MINSHWEB          "088"             //民生银行网关

#define BANKCODE_BACKFILE      	"1001"               //回盘文件对账渠道
#define PART_PAYMENT_OK     (char *)"{\"result\":\"part_pay_true\"}"    //部分付款成功

#define BALANCE_LACK_ERR	(char *)"{\"result\":\"balance_lack\"}"     //代付可用余额不足

//费率服务
#define FEE_APPS            (char *)"FEE_SVC_SERVER"            //费率服务名
#define FEE_TRANS           (char *)"FEE_SVC_TRANS"             //费率服务


//消费账务
#define ACC_CONSUMPTION        (char *)"ACC_CONSUMPTION"            //消费账务系统名
#define ACC_CONSUMPTION_SVC    (char *)"ACC_CONSUMPTION_SVC"        //消费账务系统服务

//账务
#define ACC_FINANCE         (char *)"ACC_FINANCE_SERVER"        //账务系统名
#define ACC_ACCOUNTS        (char *)"ACC_SVC_ACCOUNTS"          //账务系统服务

#define ACC_DATA            (char *)"ACC_DATA"                  //账务系统名
#define ACC_DATA_SVC        (char *)"ACC_DATA_SVC"              //账务系统服务

//账务
#define ACC_RECON           (char *)"ACC_RECON"                 //账务系统名
#define ACC_RECON_LIST      (char *)"ACC_RECON_LIST"            //账务系统服务


//生成对账单
#define ACC_ACCCOUNTLIST          (char *)"ACC_ACCCOUNTLIST"        //生成对账单服务名
#define ACC_ACCCOUNTLIST_SVC      (char *)"ACC_ACCCOUNTLIST_SVC"    //生成对账单服务

//对外代付接口
#define ACC_PAYMENT               (char *)"ACC_PAYMENT"             //代付
#define ACC_PAYMENT_SVC           (char *)"ACC_PAYMENT_SVC"         //付款接口
#define ACC_PAYMENTQUERY_SVC      (char *)"ACC_PAYMENTQUERY_SVC"    //查询接口


//付款通道内部web调用
#define ACC_PAYCHANNEL           (char *)"ACC_PAYCHANNEL"           //内部调用的付款通道服务
#define ACC_PAYCHANNEL_SVC       (char *)"ACC_PAYCHANNEL_SVC"
#define ACC_PAYCHANNELQUERY_SVC  (char *)"ACC_PAYCHANNELQUERY_SVC"

//民生实时代付款平台
#define NAC_CMBCPAYMENT          (char *)"NAC_CMBCPAYMENT"          //民生实时代付款服务名
#define NAC_CMBCPAYMENT_SVC      (char *)"NAC_CMBCPAYMENT_SVC"      //民生实时代付款服务
#define NAC_CMBCPAYMENTQUERY_SVC (char *)"NAC_CMBCPAYMENTQUERY_SVC" //民生实时代付款查询服务
#define NAC_CMBCBALANCEQRY_SVC (char *)"NAC_CMBCBALANCEQRY_SVC" //民生对公余额查询服务
#define NAC_CMBCPARTNEQRY_SVC (char *)"NAC_CMBCPARTNEQRY_SVC" //民生对公余额查询服务

//民生对账服务
#define ACC_CMBCCHECK         (char *)"ACC_CMBCCHECK"         //民生银行对账服务名
#define ACC_CMBCCHECK_SVC     (char*)"ACC_CMBCCHECK_SVC"      //民生银行对账服务
//民生银行实名认证
#define NAC_CMBCCERTIFICATE          (char *)"NAC_CMBCCERTIFICATE"          //民生实名认证服务名
#define NAC_CMBCCERTIFICATE_SVC      (char *)"NAC_CMBCCERTIFICATE_SVC"      //民生银行实名认证服务
#define NAC_CMBCCERTIFICATEQUERY_SVC (char *)"NAC_CMBCCERTIFICATEQUERY_SVC" //民生银行实名认证查询服务

//信联征信实名认证(卡认证)
#define NAC_XLZXCERTIFICATE          (char *)"NAC_XLZXCERTIFICATE"          //信联征信认证服务名
#define NAC_XLZXCERTIFICATE_SVC      (char *)"NAC_XLZXCERTIFICATE_SVC"      //信联征信实名认证服务
#define NAC_XLZXCERTIFICATEQUERY_SVC (char *)"NAC_XLZXCERTIFICATEQUERY_SVC" //信联征信实名认证查询服务

//信联征信身份认证
#define NAC_XLZXIDENTITY          (char *)"NAC_XLZXIDENTITY"          //信联征信身份认证服务名
#define NAC_XLZXIDENTITY_SVC      (char *)"NAC_XLZXIDENTITY_SVC"      //信联征信身份认证服务
#define NAC_XLZXIDENTITYQUERY_SVC (char *)"NAC_XLZXIDENTITYQUERY_SVC" //信联征信身份认证查询服务

//民生银行代扣
#define NAC_CMBCWITHHOLD          (char *)"NAC_CMBCWITHHOLD"
#define NAC_CMBCWITHHOLD_SVC      (char *)"NAC_CMBCWITHHOLD_SVC"
#define NAC_CMBCWITHHOLDQUERY_SVC (char *)"NAC_CMBCWITHHOLDQUERY_SVC"
//中信实时代付款
#define NAC_CITICPAYMENT          	(char *)"NAC_CITICPAYMENT"          	//中信实时代付款服务名
#define NAC_CITICPAYMENT_SVC      	(char *)"NAC_CITICPAYMENT_SVC"      	//中信实时代付款服务
#define NAC_CITICPAYMENTQUERY_SVC 	(char *)"NAC_CITICPAYMENTQUERY_SVC" 	//中信实时代付款查询服务
#define NAC_CITICPAY_AUDIT_OPT_SVC  (char *)"NAC_CITICPAY_AUDIT_OPT_SVC" 	//中信批付执行服务
#define NAC_CITICPAYMENTFILEDWN_SVC	(char *)"NAC_CITICPAYMENTFILEDWN_SVC"	//中信对账文件下载

#define NAC_TJ_UPAY                 (char *)"NAC_TJ_UPAY"               //天津银联代付服务名
#define NAC_TJ_UPAY_PAY_SVC         (char *)"NAC_TJ_UPAY_PAY_SVC"       //天津银联实时代付交易请求
#define NAC_TJ_UPAY_PAYQRY_SVC      (char *)"NAC_TJ_UPAY_PAYQRY_SVC"    //天津银联实时代付代付查询
#define NAC_TJ_UPAY_DOWNLOAD_SVC    (char *)"NAC_TJ_UPAY_DOWNLOAD_SVC"  //天津银联代付对账单下载
#define NAC_TJ_UPAY_NOTIFY_SVC      (char *)"NAC_TJ_UPAY_NOTIFY_SVC"  //天津银联代付对账单下载


#define ACCP_PAYRESULT	    (char *)"ACCP_PAYRESULT"	//中信对账文件下载
#define ACCP_PAYRESULT_UPDATE_SVC	    (char *)"ACCP_PAYRESULT_UPDATE_SVC"	//中信对账文件下载

//信用卡还款
#define ACC_CREDITCARDREPAY         (char *)"ACC_CREDITCARDREPAY"       //信用卡还款服务名
#define ACC_CREDITCARDREPAY_SVC     (char *)"ACC_CREDITCARDREPAY_SVC"   //信用卡还款申请服务
#define ACC_CRDREPAYRESULT_SVC      (char *)"ACC_CRDREPAYRESULT_SVC"    //信用卡还款结果查询
//计算D+0手续费与加收手续费
#define ACC_CALADDFEE       (char *)"ACC_CALADDFEE"         //计算D+0手续费与加收手续费服务名
#define ACC_CALADDFEE_SVC   (char *)"ACC_CALADDFEE_SVC"     //计算D+0手续费与加收手续费服务
//自动充值
#define ACC_AUTORECHARGE       (char *)"ACC_AUTORECHARGE"         //自动充值服务名
#define ACC_AUTORECHARGE_SVC   (char *)"ACC_AUTORECHARGE_SVC"     //自动充值服务

//深银联结算中心代付
#define NAC_SSC             (char *)"NAC_SSC"                   //代付
#define NAC_SSC_SVC         (char *)"NAC_SSC_SVC"               //付款接口
#define NAC_SSC_SVCQRY_SVC  (char *)"NAC_SSC_SVCQRY_SVC"        //付款接口

#define  SSC_SINGLEPAY      (char*)"11"                         //单笔代付
#define  SSC_SINGLEACP      (char*)"12"                         //单笔代收
#define  SSC_BATCH_PAY      (char*)"21"                         //批量代付
#define  SSC_BATCH_ACP      (char*)"22"                         //批量代收
#define  SSC_BIZTQRY_S      (char*)"31"                         //单笔查询
#define  SSC_BIZTQRY_B      (char*)"32"                         //批量查询

//其他
#define ACC_PAYABC          (char *)"ACC_PAYABC"
#define ACC_PAYABC_SVC      (char*)"ACC_PAYABC_SVC"             //临时服务


#define ACC_APPS            (char *)"ACC_SVC_SERVER"            //ACCOUNTING 系统服务集
#define ACC_RECORD          (char *)"ACC_SVC_RECORD"            //ACCOUNT 会计传票服务
//------------------------------------------------------------------------------
#define ACC_SVR             (char *)"ACC_BALANCE_SERVER"        //ACCOUNT 定时服务集
#define ACC_BALANCE         (char *)"ACC_SVC_BALANCE"           //ACCOUNT 会计余额计算服务

//------------------------------------------------------------------------------
#define UTILS_IPDBQUERY_SVR (char *)"UTILS_IPDBQUERY_SERVER"    //IPDBQuery服务
#define UTILS_IPDBQUERY     (char *)"UTILS_IPDBQUERY"           //IPDBQuery服务

//风控
#define RISK_CONTROL        (char *)"RISK_CONTROL"              //风控系统服务名
#define RISK_CONTROL_SVC    (char *)"RISK_CONTROL_SVC"          //风控系统服务

//打开是为了编译通过，hsb
#define RISK_CTRL_APPS      (char *)"RISK_CTRL_SERVER"          //风控服务
#define RISK_CTRL_REALTIME  (char *)"RISK_CTRL_REALTIME"        //实时风控


//对账
#define ACC_STATEMENT       (char *)"ACC_STATEMENT"
#define ACC_STATEMENT_SVC   (char *)"ACC_STATEMENT_SVC"         //对账实时服务

//支付宝对账
#define ACC_STATEMENT_ALIPAY       (char *)"ACC_STATEMENT_ALIPAY"
#define ACC_STATEMENT_ALIPAY_SVC   (char *)"ACC_STATEMENT_ALIPAY_SVC"  //对账实时服务

//mpay对账
#define ACC_STATEMENT_MPAY       (char *)"ACC_STATEMENT_MPAY"
#define ACC_STATEMENT_MPAY_SVC   (char *)"ACC_STATEMENT_MPAY_SVC"  //对账实时服务

//微信对账
#define ACC_WEIXINCHECK       (char *)"ACC_WEIXINCHECK"
#define ACC_WEIXINCHECK_SVC   (char *)"ACC_WEIXINCHECK_SVC"  //对账实时服务

//清分清算
#define ACC_CLEAR           (char *)"ACC_CLEAR"                 //清分服务名
#define ACC_CLEAR_SVC       (char *)"ACC_CLEAR_SVC"             //清分实时服务
#define ACC_CLEAR_EXPORT_SVC (char *)"ACC_CLEAR_EXPORT_SVC"     //导出代付文件实时服务


#define ACC_CLEAR_FCGI      (char *)"ACC_CLEAR_FCGI"
#define ACC_CLEAR_FCGI_SVC  (char *)"ACC_CLEAR_FCGI_SVC"

//账务查询
#define ACC_QUERY           (char *)"ACC_QUERY"                 //账户查询服务名
#define ACC_QUERY_SVC       (char *)"ACC_QUERY_SVC"             //账户查询服务

//提现日志
#define ACC_WITHDRAW        (char *)"ACC_WITHDRAW"              //提现服务名
#define ACC_WITHDRAW_SVC    (char *)"ACC_WITHDRAW_SVC"          //提现服务

//卡认证
#define ACC_CARDCERT        (char *)"ACC_CARDCERT"              //卡认证服务名
#define ACC_CARDCERT_SVC    (char *)"ACC_CARDCERT_SVC"          //卡认证服务

//T+1提现民生代付
#define ACC_WDCMBC          (char *)"ACC_WDCMBC"                //提现民生代付服务名
#define ACC_WDCMBC_SVC      (char *)"ACC_WDCMBC_SVC"            //提现民生代付服务

//D+0提现民生代付
#define ACC_PAYREALTIME          (char *)"ACC_PAYREALTIME"                //D+0提现民生代付服务名
#define ACC_PAYREALTIME_SVC      (char *)"ACC_PAYREALTIME_SVC"            //D+0提现民生代付服务


//稽核审核服务
#define ACC_AUDIT      (char *)"ACC_AUDIT"                  //稽核审核
#define ACC_AUDIT_SVC   (char *)"ACC_AUDIT_SVC"             //稽核审核服务


//清算查询服务
#define ACC_CLEARQUERY          (char *)"ACC_CLEARQUERY"            //清算查询服务
#define ACC_CLEARQUERY_SVC      (char *)"ACC_CLEARQUERY_SVC"             //清算查询服务名


//日终扎帐：T+1应收应付冲消
#define ACC_DAILY           (char *)"ACC_DAILY"                 //日终扎帐服务名
#define ACC_DAILY_BILL      (char *)"ACC_DAILY_BILL"            //T+1应收应付冲消服务

//对账
#define ACC_CHECKOUT        (char *)"ACC_CHECKOUT"              //对账服务名
#define ACC_CHECK_ACCOUNT   (char *)"ACC_CHECK_ACCOUNT"         //对账服务


#define ACC_PAY             (char *)"ACC_PAY"                   //代付服务名
#define ACC_PAY_SVC         (char *)"ACC_PAY_SVC"               //代付服务


#define ACC_AGENCY_PAY_CTL      (char*)"ACC_AGENCY_PAY_CTL"        //代付稽查服务名
#define ACC_AGENT_PAY_CTL_SVC   (char*)"ACC_AGENT_PAY_CTL_SVC"     //代付稽查服务


//生成对账文件服务
#define ACC_TRANS           (char *)"ACC_TRANS"                 //生成对账文件
#define ACC_TRANS_SVC       (char *)"ACC_TRANS_SVC"             //生成对账文件服务


//清算文件接口
#define ACC_CLEARFILE       (char *)"ACC_CLEARFILE"             //清算文件接口
#define ACC_CLEARFILE_SVC   (char *)"ACC_CLEARFILE_SVC"         //清算文件接口服务


//付款回盘文件对账
#define ACC_RESPONCKECK     (char *)"ACC_RESPONCKECK"           //付款回盘文件对账服务名
#define ACC_RESPON_SVC      (char *)"ACC_RESPON_SVC"            //付款回盘文件对账服务


//代付清算文件生成
#define ACC_CLEARCONVER     (char *)"ACC_CLEARCONVER"           //代付清算服务名
#define ACC_YL_SAVE         (char *)"ACC_CLEARCONVER_SVC"       //银嘉代付清算服务(将银嘉代付数据保存入嘉联平台)
#define ACC_CLEARCONVER_SVC (char *)"ACC_CLEARCONVER_SVC"       //代付文件转换服务(将代付机构的清算文件转换成嘉联的清算文件)


//(COMA)机构一般交易流水文件处理
#define ACC_COMA            (char *)"ACC_COMA"                  //服务名
#define ACC_COMA_SVR        (char *)"ACC_COMA_SVR"              //服务

//易宝对账
#define ACC_RECON_YIBAO      (char*)"ACC_RECON_YIBAO"            //易宝对账服务名
#define ACC_RECONYIBAO_SVC   (char*)"ACC_RECONYIBAO_SVC"         //易宝对账服务

//民生银行网关对账
#define ACC_RECON_MINSH      (char*)"ACC_RECON_MINSH"            //民生银行网关对账服务名
#define ACC_RECONMINSH_SVC   (char*)"ACC_RECONMINSH_SVC"         //民生银行网关对账服务


#define ACC_COUPONS         (char*)"ACC_COUPONS"                //优惠劵购机服务名
#define ACC_COUPONS_SVC     (char*)"ACC_COUPONS_SVC"            //优惠劵购机服务

#define ACC_FOUND           (char*)"ACC_FOUND"                  //资金归集服务名
#define FOUND_COMESDOWN     (char*)"FOUND_COMESDOWN"            //资金归集服务

#define ACC_UNFREEZE_SVR    (char *)"ACC_UNFREEZE_SVR"          //解冻服务集
#define ACC_UNFREEZE        (char *)"ACC_UNFREEZE"              //解冻服务


#endif // _gobal_xsvcs_H
