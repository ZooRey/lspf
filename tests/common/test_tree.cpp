#include <iostream>  
#include <string>  
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>  
#include <boost/property_tree/ini_parser.hpp>
#include <boost/foreach.hpp> 

#include "common/string_utility.h"

using namespace std; 
using namespace boost;
using namespace boost::property_tree;

void printTree(ptree root)
{
 BOOST_FOREACH(ptree::value_type &v1, root)
 {
	std::cout<<"key:["<<v1.first<<"]"<<std::endl;
	BOOST_FOREACH(ptree::value_type &vAttr, v1.second)
   {
	cout<<"attr:"<<vAttr.first<<"="<<vAttr.second.data()<<"\n";
   }
   std::cout<<std::endl;
   /*
  //如果当前节点为属性，打印全部属性信息
  if(v1.first == "<xmlattr>")
  {
	   BOOST_FOREACH(ptree::value_type &vAttr, v1.second)
	   {
		cout<<"attr:"<<vAttr.first<<"="<<vAttr.second.data()<<"  ";
	   }
	   
	std::cout<<std::endl;
  }
  else
  {
	   //如果没有子节点，则打印键值对
	   if (v1.second.empty())
	   {
		std::cout<<"key-val:"<<v1.first<<"="<<v1.second.data()<<std::endl;
	   }
	   else 
	   {
		//打印当前节点的名称
		std::cout<<v1.first<<"new node*******************"<<std::endl;
		printTree(v1.second);
		if(v1.second().size() == 0x01)
		 std::cout<<"======="<<root.get<std::string>(v1.first)<<std::endl;
	   }
  }
  */
 }
}
int main(int argc, char** argv)
{  
 if(argc < 0x02)
 {
  std::cout<<" invalid param ......"<<std::endl;
  return -1;
 }

 std::string fname = std::string(argv[0x01]);
 std::cout<<"parse file "<<fname<<std::endl;
 boost::property_tree::ptree pt;  
    if (StringUtility::EndsWith(fname, ".ini"))
    {
        boost::property_tree::ini_parser::read_ini(fname, pt);
    }
    else if (StringUtility::EndsWith(fname, ".xml"))
    {
        boost::property_tree::read_xml(fname, pt);
    }
	cout << pt.get<int>("section.number") << endl;
	cout << pt.get<std::string>("section.string") << endl;
 printTree(pt);
 return 0;
}  