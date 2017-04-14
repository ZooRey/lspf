#ifndef __DATA_SIGN_H__
#define __DATA_SIGN_H__

#include <map>
#include <string>

class DataSign
{
public:
    //nMethod 0 RSA, 1 md
    DataSign(const int nMethod = 0);
    ~DataSign();

    static void InitEncKey(const std::string &merch_no);

    void addItem(const std::string &strKey, const std::string &strValue);

    void SetEncKey(const std::string &strEncKey);
    bool GetEncKey(const std::string &merch_no);

    bool verify(const std::string &strSign);
    std::string genSign();

private:
    std::string getSignData();

private:
    int m_nMethod;
    std::string m_strEncKey;
    std::map<std::string, std::string> m_mapParam;

    static std::map<std::string, std::string> rsa_key_list;
};

#endif // __DATA_SIGN_H__
