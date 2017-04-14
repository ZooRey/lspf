#ifndef ROUTE_DISPATH_H__
#define ROUTE_DISPATH_H__

#include <string>

class RouteDispath
{
public:
    RouteDispath(const std::string &channel_id);
    ~RouteDispath();

public:
    int Query(const std::string &request, std::string &response);
	int Pay(const std::string &request, std::string &response);

private:
    std::string m_channel_id;

};

#endif // __PAY_MENT_H__
