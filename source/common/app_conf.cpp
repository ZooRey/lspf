#include <cstdio>
#include <cstdlib>
#include <iostream>

#include <boost/foreach.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "string_utility.h"
#include "app_conf.h"

AppConf* AppConf::instance_ = NULL;

AppConf* AppConf::Instance()
{
    if (instance_ == NULL){
        instance_ =  new AppConf();
    }
    return instance_;
}

bool AppConf::LoadConf(const std::string &filename)
{
    if (access(filename.c_str(), 0) == -1)
    {
        return false;
    }

    boost::property_tree::ptree pt;

    if (StringUtility::EndsWith(filename, ".ini"))
    {
        boost::property_tree::ini_parser::read_ini(filename, pt);
    }
    else if (StringUtility::EndsWith(filename, ".xml"))
    {
        boost::property_tree::read_xml(filename, pt);
    }
    else
    {
        return false;
    }

    BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt)
    {
        SECTION_CONF_MAP section_map;
        BOOST_FOREACH(boost::property_tree::ptree::value_type &vAttr, v.second)
        {
            section_map.insert(make_pair(vAttr.first, vAttr.second.data()));
        }

        m_conf_items.insert(make_pair(v.first, section_map));
    }

    if (m_conf_items.empty())
    {
        return false;
    }

    return true;
}

int AppConf::GetConfInt(const std::string &section, const std::string &name)
{
    BOOST_AUTO(iter, m_conf_items.find(section));

    if (iter != m_conf_items.end())
    {
        BOOST_AUTO(it, iter->second.find(name));

        if (it != iter->second.end())
        {
            return atoi(it->second.c_str());
        }
    }

    return -1;
}

std::string AppConf::GetConfStr(const std::string &section, const std::string &name)
{
    BOOST_AUTO(iter, m_conf_items.find(section));

    if (iter != m_conf_items.end())
    {
        BOOST_AUTO(it, iter->second.find(name));

        if (it != iter->second.end())
        {
            return it->second;
        }
    }

    return "";
}

