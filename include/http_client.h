#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <string>

class HttpClient
{
public:
    HttpClient();
    std::string fetch(const std::string &url);

private:
    static size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp);
};

#endif