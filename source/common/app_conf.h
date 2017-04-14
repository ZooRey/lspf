#ifndef __APP_CONF_H__
#define __APP_CONF_H__

#include <map>
#include <string>

class AppConf
{
public:
    static AppConf* Instance();

    /// @brief ���������ļ�
    bool LoadConf(const std::string &filename);

    /// @brief ��ȡ�����ļ������Ͳ���
    int GetConfInt(const std::string &section, const std::string &name);

    /// @brief ��ȡ�����ļ����ַ�������
    std::string GetConfStr(const std::string &section, const std::string &name);

private:
    AppConf(){};

private:
    typedef std::map<std::string, std::string> SECTION_CONF_MAP;
    typedef std::map<std::string, SECTION_CONF_MAP> CONFIG_MAP;

    CONFIG_MAP m_conf_items;

	static AppConf* instance_;
};

#endif

