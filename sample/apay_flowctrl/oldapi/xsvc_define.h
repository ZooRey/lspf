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

#ifndef _XSVCS_PAYMENT_H
#define _XSVCS_PAYMENT_H

//账务
#define ACCP_WDSOURCE         (char *)"ACCP_WDSOURCE"        //账务系统名
#define ACCP_WDSOURCE_SVC        (char *)"ACCP_WDSOURCE_SVC"          //账务系统服务

#define ACCP_PAYMENT               (char *)"ACCP_PAYMENT"             //代付
#define ACCP_PAYMENT_SVC           (char *)"ACCP_PAYMENT_SVC"         //付款接口
#define ACCP_PAYMENT_QUERY_SVC      (char *)"ACCP_PAYMENT_QUERY_SVC"    //查询接口

//风控
#define ACCP_RISKCTRL        	   (char *)"ACCP_RISKCTRL"              //风控系统服务名
#define ACCP_RISKCTRL_SVC          (char *)"ACCP_RISKCTRL_SVC"          //风控系统服务

#define ACCP_CHANNEL               (char *)"ACCP_CHANNEL"             //渠道路由服务
#define CHL_GETCODE_SVC           (char *)"CHL_GETCODE_SVC"         //获取渠道代码接口
#define CHL_ROUTE_SVC      (char *)"CHL_ROUTE_SVC"    //渠道路由接口

//账务
#define ACCP_FINANCE         (char *)"ACCP_FINANCE"        //账务系统名
#define ACCP_ACCOUNT        (char *)"ACCP_ACCOUNT_SVC"          //账务系统服务


//交易结果
#define ACCP_PAYRESULT         (char *)"ACCP_PAYRESULT"        //交易结果
#define ACCP_PAYRESULT_UPDATE_SVC        (char *)"ACCP_PAYRESULT_UPDATE_SVC"          //交易结果更新服务

#endif // _gobal_xsvcs_H
