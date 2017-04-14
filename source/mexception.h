#ifndef LSPF_MEXCEPTION_H__
#define LSPF_MEXCEPTION_H__

#include <string>
#include <exception>

//自定义异常类
class MException : public std::exception {
public:
	MException(): message_() {}

	MException(const std::string& message): message_(message){}

	virtual ~MException() throw() {}

	virtual const char *what() const throw() {
		if (message_.empty()) {
			return "Default Exception.";
		} else {
			return message_.c_str();
		}
	}

protected:
	std::string message_;
};

#endif
