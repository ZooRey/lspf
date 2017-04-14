#include "dao_channel.h"
#include <iostream>
#include <string>
#include "log/log.h"
#include "database/db_oracle.h"

DAOChannel::DAOChannel(const int nId):m_db_cmd(new DBCommand(nId))
{

}
DAOChannel::DAOChannel(DBCommandPtr db_cmd):m_db_cmd(db_cmd)
{

}

DAOChannel::~DAOChannel()
{
}

//获取路由规则ID
bool DAOChannel::GetRRuleId(std::vector<std::string> &vecRRuleId)
{
	std::string sql;
	std::string rule_id;
    try{
		Statement *statement = m_db_cmd->getStatement();

        sql.assign("select rut_rul_id from ch_chl_rut where trunc(start_dt)<= trunc(sysdate) and  trunc(end_dt)>= trunc(sysdate) order by prio_val desc");

		statement->setSQL(sql);
        ResultSet *rs = statement->executeQuery();
		while (!rs->next()){
			rule_id = rs->getString(1);
            vecRRuleId.push_back(rule_id);
		}

        ///必须管理结果集，否则内存泄漏
        statement->closeResultSet(rs);

		if (vecRRuleId.empty()){
            PLOG_ERROR("no record :%s\n", sql.c_str());
			return false;
		}

    }catch(SQLException &sqlExcp){
		PLOG_ERROR("SQLException %d:%s. sql=[%s]\n", sqlExcp.getErrorCode(), sqlExcp.getMessage().c_str(), sql.c_str());
        return false;
	}catch(DBException &dbExcp){
		PLOG_ERROR("DBException :%s.\n", dbExcp.what());
        return false;
	}

    return true;
}

//获取渠道ID对应的最大单笔支付金额
bool DAOChannel::GetMaxSingleAmt(const std::string &channel_id, std::string &channel_name, unsigned long &max_amt)
{
	std::string sql;
    try{
        sql = std::string("select CHL_NAM, SGL_MAX_AMT from CH_CHL where CHL_ID='") + channel_id + "'";

        ResultSet *rs = m_db_cmd->executeQuery(sql);

		if (!rs->next())
		{
			PLOG_ERROR("GetChannelInfo:%s\n", sql.c_str());
			return false;
		}

        channel_name = rs->getString(1);
        Number num = rs->getNumber(2);

        max_amt = (unsigned long )num;

    }catch(SQLException &sqlExcp){
		PLOG_ERROR("SQLException %d:%s. sql=[%s]\n", sqlExcp.getErrorCode(), sqlExcp.getMessage().c_str(), sql.c_str());
        return false;
	}catch(DBException &dbExcp){
		PLOG_ERROR("DBException :%s.\n", dbExcp.what());
        return false;
	}

    return true;

}

bool DAOChannel::GetWorkDayFlag(const std::string &pay_date, std::string &flag)
{
	std::string sql;
    try{
		Statement *statement = m_db_cmd->getStatement();

        sql.assign("select decode(t.property, '2', 'N', 'Y') isworkday from acc_user.t_holiday t where t.holiday=:1");

		statement->setSQL(sql);

		statement->setString(1, pay_date);

        ResultSet *pRs = statement->executeQuery();

		if (!pRs->next()){
			///必须管理结果集，否则内存泄漏
            statement->closeResultSet(pRs);
			PLOG_INFO("GetWorkDayFlag error: pay_date=%s", pay_date.c_str());
			flag = "Y";
			return true;
		}

        flag = pRs->getString(1);
        ///必须管理结果集，否则内存泄漏
        statement->closeResultSet(pRs);

    }catch(SQLException &sqlExcp){
		PLOG_ERROR("SQLException %d:%s. sql=[%s]\n", sqlExcp.getErrorCode(), sqlExcp.getMessage().c_str(), sql.c_str());
        return false;
	}catch(DBException &dbExcp){
		PLOG_ERROR("DBException :%s.\n", dbExcp.what());
        return false;
	}

    return true;
}

bool DAOChannel::GetChannelId(const std::string &pay_date, const std::string &work_day_flag, const int &amount, std::string &channel_id)
{
    m_vChlInfo.clear();

	std::string strSql;

    try{
        Statement *statement = m_db_cmd->getStatement();

        strSql = std::string("select t2.chl_id, t2.wt_val \
                        from acc_user.ch_chl_rut t1 \
                        join acc_user.ch_wt_rut t2 on t1.rut_rul_id = t2.chl_rut_rul_id \
                        where to_date(:1, 'YYYYMMDD') between t1.start_dt and t1.end_dt \
                         and work_day_flag = :2");


        statement->setSQL(strSql);
        statement->setString(1, pay_date);
        statement->setString(2, work_day_flag);

        ResultSet *rs = statement->executeQuery();
		while (rs->next())
		{
            __STR_CHANNEL_INFO chlInfo;
            chlInfo.strChanellId = rs->getString(1);
            chlInfo.strWtVal = rs->getString(2);
            m_vChlInfo.push_back(chlInfo);
		}

        ///必须管理结果集，否则内存泄漏
        statement->closeResultSet(rs);

    }catch(SQLException &sqlExcp){
		PLOG_ERROR("SQLException %d:%s. sql=[%s]\n", sqlExcp.getErrorCode(), sqlExcp.getMessage().c_str(), strSql.c_str());
        return false;
	}catch(DBException &dbExcp){
		PLOG_ERROR("DBException :%s.\n", dbExcp.what());
        return false;
	}

    //当为空情况，去默认值

    int iTotalWeight = 0;
    // 先找到总权重
    vector<__STR_CHANNEL_INFO>::const_iterator it;
    for(it = m_vChlInfo.begin(); it != m_vChlInfo.end(); it++)
    {
        iTotalWeight = iTotalWeight + atoi(it->strWtVal.c_str());
    }

    if (iTotalWeight > 0){
         // 产生权重值
        srand((unsigned)time(NULL));
        int iRand = rand()%iTotalWeight;

        int iTmpWeight = 0;
        // 计算权重
        for(it = m_vChlInfo.begin(); it != m_vChlInfo.end(); it++)
        {
            // 根据总权重计算出当前该往哪个渠道发消息。
            iTmpWeight = iTmpWeight + atoi(it->strWtVal.c_str());
            // 系统生成的权重值和总的权重发生对比，小于这个值，即可认为找到对应渠道
            if(iRand < iTmpWeight)
            {
                channel_id = it->strChanellId;
                break;
            }
        }
    }


    if (channel_id.empty() && m_vChlInfo.size() > 0){
        channel_id = m_vChlInfo.begin()->strChanellId;
    }

    if (channel_id.empty()){
        return false;
    }

    return true;
}

bool DAOChannel::GetChannelId(const int &amount, std::string &channel_id, std::string &channel_name)
{
	std::string sql;
    try{
        sql = std::string("select CHL_ID, CHL_NAM "
                          "from CH_CHL where CHL_STS_CD='2' and SGL_MAX_AMT>:1 "
                          "and CHL_START_DT <= trunc(sysdate) and CHL_STOP_DT >= trunc(sysdate) "
                          "order by SGL_MAX_AMT asc");

        Statement *statement = m_db_cmd->getStatement();
        statement->setSQL(sql);
        statement->setInt(1, amount);

        ResultSet *rs = statement->executeQuery();

        if (!rs->next()){
            ///必须管理结果集，否则内存泄漏
            statement->closeResultSet(rs);
            PLOG_ERROR("GetChannelInfo:%s\n", sql.c_str());
            return false;
        }

        channel_id = rs->getString(1);
        channel_name = rs->getString(2);

        ///必须管理结果集，否则内存泄漏
        statement->closeResultSet(rs);

    }catch(SQLException &sqlExcp){
		PLOG_ERROR("SQLException %d:%s. sql=[%s]\n", sqlExcp.getErrorCode(), sqlExcp.getMessage().c_str(), sql.c_str());
        return false;
	}catch(DBException &dbExcp){
		PLOG_ERROR("DBException :%s.\n", dbExcp.what());
        return false;
	}

    return true;

}
