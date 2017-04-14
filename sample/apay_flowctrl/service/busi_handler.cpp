#include "busi_handler.h"

BusiHandler::BusiHandler()
{

}
BusiHandler::~BusiHandler()
{

}

void BusiHandler::method(const std::string &strInput, std::string &strOutput)
{
    int rc = 0;
    rc = parseJson(strInput);
    if (rc == 0)
    {
        //rc = verifySign();
        if (rc == 0)
        {
            rc = process();
        }
    }

    rc = makeJson(strOutput);
}

