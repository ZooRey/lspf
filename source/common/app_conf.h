#ifndef __APP_CONF_H__
#define __APP_CONF_H__

#include <map>
#include <string>

class AppConf
{
public:
    static AppConf* Instance();

    /// @brief 加载配置文件
    bool LoadConf(const std::string &filename);

    /// @brief 获取配置文件中整型参数
    int GetConfInt(const std::string &section, const std::string &name);

    /// @brief 获取配置文件中字符串参数
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

