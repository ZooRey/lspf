#define BOOST_TEST_DYN_LINK 
#define BOOST_TEST_MODULE example
//#define BOOST_AUTO_TEST_MAIN

#include <boost/test/unit_test.hpp>
#include <boost/thread/thread.hpp>
#include <unistd.h>
#include <iostream>
#include "common/thread_local.h"

class Sample{
public:
	Sample(){ std::cout << "Sample():" << &id_ << std::endl;}
	~Sample(){ std::cout << "~Sample():" << &id_ << std::endl;}
	
private:
	int id_;
};

class Sample2{
public:
	Sample2(){ std::cout << "Sample2():" << &id_ << std::endl;}
	~Sample2(){ std::cout << "~Sample2():" << &id_ << std::endl;}
	
private:
	int id_;
};

void ThreadFunc(){
	Sample *sample_1 = ThreadLocal<Sample>::Instance();
	Sample *sample_2 = ThreadLocal<Sample>::Instance();
	Sample2 *sample_3 = ThreadLocal<Sample2>::Instance();
	Sample2 *sample_4 = ThreadLocal<Sample2>::Instance();
	sleep(1);
}


BOOST_AUTO_TEST_SUITE(minimal_test)

BOOST_AUTO_TEST_CASE(thread_test)
{
	boost::thread t1(&ThreadFunc);
	boost::thread t2(&ThreadFunc);
	
	while( std::cin.get() != 'q') {}
	/*
    BOOST_CHECK(add(2, 2) == 4);

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


BOOST_AUTO_TEST_SUITE_END()
