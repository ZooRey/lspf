#ifndef __MDB_CONN_POOL_H__
#define __MDB_CONN_POOL_H__

#include <string>
using namespace std;

#include <occi.h>
using namespace oracle::occi;
class Mutex;

class DBConnPool
{
public:
    DBConnPool(const std::string db, const std::string user, const std::string passwd, ConnectionPool *connectionPool) :
        m_db(db), m_user(user), m_passwd(passwd),m_connectionPool(connectionPool) {}

    ~DBConnPool(){}

    void setConnectionPool(ConnectionPool *connectionPool){
        m_connectionPool = connectionPool;
    }

    Connection *getConnection(){
        return m_connectionPool->createConnection(m_user, m_passwd);
    }

    ConnectionPool *getConnectionPool(){
        return m_connectionPool;
    }

private:
    std::string m_db;
    std::string m_user;
    std::string m_passwd;

    ConnectionPool *m_connectionPool;
};

class DBConnManage
{
protected:
    static DBConnManage *m_manage;

    DBConnManage(){};

public:
	~DBConnManage();

	static DBConnManage *instance(){
		if (m_manage == NULL){
			m_manage = new DBConnManage;
		}

		return m_manage;
	}

    /// @brief ��ʼ������
    void init();

    /// @brief �������ݿ����ӳ�
    /// @param dbname���ݿ���
    /// @param user ���ݿ��û���
    /// @param passwd���ݿ��û�����
    /// @param minConn ��С������
    /// @param maxConn ���������
    /// @param inrConn ������
    /// @return true �ɹ���false ʧ��
	bool creatConnectionPool(const string  &dbname, const string &user, const string &passwd,
                              const int &minConn, const int &maxConn, const int &inrConn = 1);

    /// @brief ��ȡ���ݿ�����
    /// @param index ��������
    /// @return DBConnection����
	DBConnPool *getDBConnPool(const unsigned int index=0);

    /// @brief ��ֹ������Դ
	void fini();

private:

	Environment * m_env;
	std::vector<DBConnPool *> m_vecPool;
};

#endif
