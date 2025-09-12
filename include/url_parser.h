#ifndef URL_PARSER_H
#define URL_PARSER_H

#include <string>
#include <vector>

class UrlParser
{
public:
    std::vector<std::string> extractLinks(const std::string &html, const std::string &baseUrl);

private:
    std::string normalizeUrl(const std::string &url, const std::string &baseUrl);
    std::string getDomain(const std::string &url);
    bool isValidUrl(const std::string &url);
};

#endif