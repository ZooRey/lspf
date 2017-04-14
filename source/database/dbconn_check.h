#ifndef DBCONN_CHECK_H
#define DBCONN_CHECK_H

#include "common/thread.h"
#include <vector>
#include <string>

class DBConnCheckThread : public Thread
{
public:
    static DBConnCheckThread* Instance();

    static void Init(const int pool_size_);

    virtual void Run();

private:
    DBConnCheckThread();

    virtual ~DBConnCheckThread();

private:
    static int pool_size;
    static DBConnCheckThread* instance_;
};

#endif
