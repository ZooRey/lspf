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
    DBConnPool(const std::string db, const std::string user, const std::string passwd, StatelessConnectionPool *connectionPool) :
        m_db(db), m_user(user), m_passwd(passwd),m_connectionPool(connectionPool) {}

    ~DBConnPool(){}

    void setConnectionPool(StatelessConnectionPool *connectionPool){
        m_connectionPool = connectionPool;
    }

    Connection *getConnection(){
        return m_connectionPool->getConnection();
    }

    StatelessConnectionPool *getConnectionPool(){
        return m_connectionPool;
    }

private:
    std::string m_db;
    std::string m_user;
    std::string m_passwd;

    StatelessConnectionPool *m_connectionPool;
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

    /// @brief 初始化环境
    void init();

    /// @brief 创建数据库连接池
    /// @param dbname数据库名
    /// @param user 数据库用户名
    /// @param passwd数据库用户密码
    /// @param minConn 最小连接数
    /// @param maxConn 最大连接数
    /// @param inrConn 增长数
    /// @return true 成功，false 失败
	bool creatConnectionPool(const string  &dbname, const string &user, const string &passwd,
                              const int &minConn, const int &maxConn, const int &inrConn = 3);

    /// @brief 获取数据库连接
    /// @param index 配置索引
    /// @return DBConnection连接
	DBConnPool *getDBConnPool(const unsigned int index=0);

    /// @brief 启动检测连接线程
    void CheckConnection(const int timeout);

    /// @brief 终止回收资源
	void fini();

private:

	Environment * m_env;
	std::vector<DBConnPool *> m_vecPool;
};

#endif
