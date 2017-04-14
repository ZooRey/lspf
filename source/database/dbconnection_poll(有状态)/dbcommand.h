#ifndef __DB_COMMAND_H__
#define __DB_COMMAND_H__

#include <string>
#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>

using namespace std;

#include <occi.h>
using namespace oracle::occi;
#include "database/dbconn_manage.h"

class DBCommand
{
public:
	DBCommand(const int index = 0);
	~DBCommand();

public:
    /// @brief 初始化
    void init();

    /// @brief 获取Statement对象
	Statement *getStatement();

    /// @brief 执行插入更新删除
	int executeUpdate(const string &_strSql);

    /// @brief 查询结果
    /// @return 返回结果集ResultSet*
	ResultSet *executeQuery(const string &_strSql);

    /// @brief 设置是否自动提交
    void setAutoCommit(bool auto_commit);

    /// @brief 事务提交
	void commit();

    /// @brief 事务回滚
	void rollBack();

    /// @brief 出错终止
	void terminate();
	//ResultSet *GetResultSet();
private:


private:
    int m_index;
    bool bAutoCommit;

	Statement *m_statement;
	//ResultSet *m_pResultSet;
    Connection *m_connection;
    ConnectionPool *m_connPool;

};

//声明智能指针类型
typedef boost::shared_ptr<DBCommand>  DBCommandPtr;

#endif
