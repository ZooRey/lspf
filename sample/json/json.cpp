#include <iostream>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <cstdio>
#include <unistd.h>
#include <json/json.h>

using namespace std;

//JSON解包
void parseJson()
{
    //std::string strValue = "{\"key1\":\"value1\",\"array\".[{\"key2\":\"value2\"},{\"key2\":\"value3\"},{\"key2\":\"value4\"}]}";

    std::string strValue =
    "\"version\":\"1.0.0\",\"request_id\":\"254903\",\"organize_code\":\"001440332334700\",\"sign\":\"sc8M3qR+C7vWlonc0FPlj4JhSYG3hmtUtCI0NvxDrOi3S2H9ZA/uTi1D1Zj6gvbUFwykoOB66jIwzKltB0eTKo5bNLuZP7mMdH1b81kdDW1HSH9Eidy1SCHRuzNR6gyf0emWjOqdWsjGrDa5aTRkYT07dNN2VAaIFemwFdlrSG9CMhxo563j3/F6lwoHg4Cmem7737yPf+KM7mVWyh27eT4aodYL0tTUHufVzQeNulbRgD5ICsS135DQMmgEFIGKSvuSpO48Toa5geA/wpsafpVqmVwmEC1Z0YwUwOTmHwLCpiCFZcW8UyE4ojVV5wmp5DSgMuuPuEcHbOORbMND/w==\"";
    Json::Reader reader(Json::Features::strictMode());
    Json::Value value;

    if (reader.parse(strValue, value))
    {
		std::string out = value["key1"].asString();
		std::cout << out << std::endl;

		std::string outnull = value["outnull"].asString();
		std::cout << "outnull= "<< outnull << std::endl;

		const Json::Value arrayObj = value["array"];

		std::cout << "array size=" << arrayObj.size() << " : ";

		for (unsigned int i=0; i<arrayObj.size(); i++)
		{
			out = arrayObj[i]["key2"].asString();
			std::cout << out << " ";
		}

		std::cout << std::endl;
    }else{
        std::cout << "parse error" << std::endl;
    }
}

//Json组包
void makeJson()
{
    Json::Value root;
    Json::Value arrayObj;
    Json::Value item;
    for (int i=0; i<10; i++)
    {
		item["key"] = i;
		arrayObj.append(item);
    }

    root["key1"] = "value1";
    root["key2"] = "value2";
    root["keyInt"] = 100;
    root["array"] = arrayObj;
    root.toStyledString();
    std::string out = root.toStyledString();
    std::cout << out << std::endl;

}

//删除json对象
void deleteJson()
{
    std::string strContent = "{\"key\":\"1\",\"name\":\"test\"}";

    Json::Reader reader;

    Json::Value value;

    if (reader.parse(strContent, value))
    {
        Json::Value root=value;

        root.removeMember("key");

        printf("%s \n",root.toStyledString().c_str());
	}
}

int main(int argc, char* argv[])
{
	std::cout << "parseJson:" << std::endl;
    parseJson();

	std::cout << "\nmakeJson:" << std::endl;
	makeJson();

	std::cout << "\ndeleteJson:" << std::endl;
	deleteJson();

	return 0;
}
