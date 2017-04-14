#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE example

#include <string>
#include <boost/test/unit_test.hpp>

#include "common/app_conf.h"

BOOST_AUTO_TEST_SUITE(conf_test)

BOOST_AUTO_TEST_CASE(my_test1)
{
    int number;
    std::string strcontext;
	AppConf* appconf = AppConf::Instance();

    BOOST_CHECK(appconf->LoadConf("./test.ini"));

	BOOST_CHECK(appconf->GetConfStr("section", "string") == "abcd");

	BOOST_CHECK(appconf->GetConfInt("section", "number") == 1234);

    number = appconf->GetConfInt("section", "number");
    if (number < 0)
    {
        BOOST_ERROR("get number error!");
    }

    BOOST_CHECK_MESSAGE(false, "section:number=" << number);

    strcontext = appconf->GetConfStr("section", "string");
    if (strcontext.empty())
    {
        BOOST_ERROR("get string error!");
    }

    BOOST_CHECK_MESSAGE(false, "section:string=" << strcontext);

    //BOOST_CHECK_MESSAGE(appconf->GetConfInt("section", "number") > 0, "add(..) result: " << add(2, 2));

    //BOOST_CHECK_MESSAGE(appconf->GetConfStr("section", "string") == 4, "add(..) result: " << add(2, 2));

	/*
    BOOST_REQUIRE(add(2, 2) == 4);

    if(add(2, 2) != 4)

        BOOST_ERROR("oops!");

    if(add(2, 2) != 4)

        BOOST_FAIL("oops!");

    if(add(2, 2) != 4)

        throw "oops!";

    BOOST_CHECK_MESSAGE(add(2, 2) == 4, "add(..) result: " << add(2, 2));

    BOOST_CHECK_EQUAL(add(2, 2), 4);
	*/
}

BOOST_AUTO_TEST_CASE(my_test2)
{
    int number;
    std::string strcontext;
	AppConf* appconf = AppConf::Instance();

    BOOST_CHECK(appconf->LoadConf("./test.xml"));

	BOOST_CHECK(appconf->GetConfStr("section", "string") == "abcd");

	BOOST_CHECK(appconf->GetConfInt("section", "number") == 1234);

    if (appconf->GetConfInt("section", "number") < 0)
    {
        BOOST_ERROR("get number error!");
    }

    BOOST_CHECK_EQUAL(appconf->GetConfInt("section1", "number"), 12345);

    BOOST_CHECK_MESSAGE(appconf->GetConfStr("section1", "string") == "abcdefgh", "section1:string=" << number);

}

BOOST_AUTO_TEST_SUITE_END()
