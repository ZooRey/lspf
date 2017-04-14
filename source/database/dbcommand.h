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
    /// @brief ��ʼ��
    void init();

    /// @brief ��ȡStatement����
	Statement *getStatement();

    /// @brief ִ�в������ɾ��
	int executeUpdate(const string &_strSql);

    /// @brief ��ѯ���
    /// @return ���ؽ����ResultSet*
	ResultSet *executeQuery(const string &_strSql);

    /// @brief �����Ƿ��Զ��ύ
    void setAutoCommit(bool auto_commit);

    /// @brief �����ύ
	void commit();

    /// @brief ����ع�
	void rollBack();

    /// @brief ������ֹ
	void terminate();
	//ResultSet *GetResultSet();

    /// @brief �رս����
	void closeResultSet();

    /// @brief �رս����
	void closeResultSet(ResultSet *rs);
private:
    int m_index;
    bool bAutoCommit;

	Statement *m_statement;
	ResultSet *m_result_set;
    Connection *m_connection;
    StatelessConnectionPool *m_connPool;

};

//��������ָ������
typedef boost::shared_ptr<DBCommand>  DBCommandPtr;

#endif
