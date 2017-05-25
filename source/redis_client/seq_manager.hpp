#include "redisclient.hpp"
#include "json/json.h"
#include "log/log.h"

class CSequenceManager
{
public:
    CSequenceManager(std::string prex="sequenceKeyPrex"):m_keyPrex(prex)
    {

    }
    ~CSequenceManager()
    {

    }


    bool Init(vector<pair<string, int>>& hostVec, int nTimeout, int nConnNum, bool bIfCluster)
    {
        return m_redisCli->Initialize(hostVec, nTimeout, nConnNum, bIfCluster);
    }

    bool Init(boost::shared_ptr<CRedisClient> redisCli)
    {
        m_redisCli = redisCli;
        return true;
    }

    int GetSequence(const std::string& request, std::string& response)
    {
        try
        {
            Json::Reader reader(Json::Features::strictMode());
            Json::Value value;

            if (!reader.parse(request, value))
            {
                PLOG_INFO("message Json parse failed, message=%s", request.c_str());
                return -1;
            }
            
            std::string Sequence_name = value["Sequence_name"].asString();
            std::string Sequence_min = value["Sequence_min"].asString();
            std::string Sequence_max = value["Sequence_max"].asString();
            std::string Sequence_step = value["Sequence_step"].asString();


            PLOG_INFO("Get Sequence from redis, request=%s", request.c_str());
            int64_t seqMin = atol(Sequence_min.c_str());
            int64_t seqMax = atol(Sequence_max.c_str());
            long step = atol(Sequence_step.c_str());
            long iSeq;
            char szTmp[30];
            std::string seqKey = m_keyPrex+Sequence_name;
            int iRet = m_redisCli->Incrby(seqKey, step, &iSeq);
            if(iRet != RC_SUCCESS)
            {
                if(iRet == RC_OBJ_NOT_EXIST)//如果结点不存在，则创建
                {
                    PLOG_INFO("Redis key is not exist, key=%s", seqKey.c_str());
                    int iRet1 = (m_redisCli->Setnx(seqKey, Sequence_min));
                    if(iRet1 == RC_SUCCESS)
                    {
                        iSeq = seqMin;
                    }
                    else if(iRet1 == RC_OBJ_EXIST)//如果有多线程或者多进程同时进行Setnx就可能会返回结果已经存在
                    {
                        m_redisCli->Incrby(seqKey, step, &iSeq);
                    }
                    else
                    {
                        PLOG_INFO("Redis Setnx failed,key=%s, result=%d", seqKey.c_str(), iRet1);
                        return -1;
                    } 
                }
                else//其他错误，返回失败
                {
                    PLOG_INFO("Redis Incrby failed,key=%s, result=%d", seqKey.c_str(), iRet);
                    return -1;
                }
            }
            else
            {
                if(iSeq > seqMax)//如果多线程或者多进程访问的时候，可能会有问题
                {
                    PLOG_INFO("Redis key value exceed, key=%s, value=%d, reset to %d", seqKey.c_str(), iSeq, seqMin);
                    m_redisCli->Set(seqKey, Sequence_min);
                    iSeq = seqMin;
                }
            }
            Json::FastWriter writer;
            Json::Value valueRep;

            sprintf(szTmp, "%ld", iSeq);

            valueRep["trans_id"] = trans_id;
            valueRep["Sequence"] = std::string(szTmp);

            response = writer.write(valueRep);
            PLOG_INFO("Get Sequence from redis, request=%s", request.c_str());
        }
        catch(const std::exception& e)
        {
            return -1;
        }
        return 0;       
    }

private:
    boost::shared_ptr<CRedisClient> m_redisCli;
    std::string m_keyPrex;
};