#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE example
#include "object_pool.h"

#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/test/unit_test.hpp>

class ObjectA{
public:
	ObjectA(){ std::cout << "ObjectA()" << std::endl;}
	~ObjectA(){ std::cout << "~ObjectA()" << std::endl;}

	int add(int i, int j)
	{
		return i + j;
	}
};



BOOST_AUTO_TEST_SUITE(minimal_test)

BOOST_AUTO_TEST_CASE(my_test)
{
	ObjectPool<ObjectA> pool(10);

	boost::shared_ptr<ObjectA> p = pool.GetObject();

	boost::shared_ptr<ObjectA> p2(pool.GetObject());

	BOOST_CHECK(p->add(2, 2) == 4);
	BOOST_CHECK(p2->add(3, 5) == 8);

	pool.ReturnObject(p2);
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
