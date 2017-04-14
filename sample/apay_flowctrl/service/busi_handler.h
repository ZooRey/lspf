#ifndef __BUSI_HANDER__
#define __BUSI_HANDER__

#include <string>

class BusiHandler
{
public:
    BusiHandler();
    virtual ~BusiHandler();
	void method(const std::string &strInput, std::string &strOutput);

protected:
	virtual int process() = 0;
	virtual int parseJson(const std::string &strInput) = 0;
	virtual int makeJson(std::string &strOutput) = 0;
	virtual int verifySign() = 0;
private:

};

#endif
