#include "url_parser.h"
#include <regex>
#include <algorithm>

std::vector<std::string> UrlParser::extractLinks(const std::string &html, const std::string &baseUrl)
{
    std::vector<std::string> links;

    // match href attributes
    std::regex linkRegex(R"(href\s*=\s*[\"']([^\"']+)[\"'])", std::regex::icase);

    auto begin = std::sregex_iterator(html.begin(), html.end(), linkRegex);
    auto end = std::sregex_iterator();

    for (std::sregex_iterator i = begin; i != end; ++i)
    {
        std::smatch match = *i;
        std::string url = match[1].str();

        // normalize the URL
        std::string normalized = normalizeUrl(url, baseUrl);

        if (isValidUrl(normalized))
        {
            links.push_back(normalized);
        }
    }

    return links;
}

std::string UrlParser::normalizeUrl(const std::string &url, const std::string &baseUrl)
{
    // if it has prefix, return it
    if (url.find("http://") == 0 || url.find("https://") == 0)
    {
        return url;
    }

    // skip non-http protocols
    if (url.find("javascript:") == 0 || url.find("mailto:") == 0 ||
        url.find("tel:") == 0 || url.find("#") == 0)
    {
        return "";
    }

    // handle protocol-relative URLs
    if (url.find("//") == 0)
    {
        size_t pos = baseUrl.find("://");
        if (pos != std::string::npos)
        {
            return baseUrl.substr(0, pos) + ":" + url;
        }
        return "https:" + url;
    }

    // get base domain
    std::string base = getDomain(baseUrl);

    // handle absolute paths
    if (url[0] == '/')
    {
        return base + url;
    }

    // Handle relative paths
    size_t lastSlash = baseUrl.find_last_of('/');
    if (lastSlash != std::string::npos && lastSlash > 8)
    { // After http://
        return baseUrl.substr(0, lastSlash + 1) + url;
    }

    return base + "/" + url;
}

std::string UrlParser::getDomain(const std::string &url)
{
    std::regex domainRegex(R"((https?://[^/]+))");
    std::smatch match;

    if (std::regex_search(url, match, domainRegex))
    {
        return match[1].str();
    }

    return "";
}

bool UrlParser::isValidUrl(const std::string &url)
{
    if (url.empty())
    {
        return false;
    }

    // must start with http:// or https://
    if (url.find("http://") != 0 && url.find("https://") != 0)
    {
        return false;
    }

    // should have a domain
    if (url.length() < 10)
    { // Minimum: http://a.b
        return false;
    }

    // exclude these resources from crawling
    std::vector<std::string> excludeExt = {
        ".jpg", ".jpeg", ".png", ".gif", ".pdf", ".zip",
        ".mp4", ".mp3", ".avi", ".css", ".js", ".ico"};

    std::string lowerUrl = url;
    std::transform(lowerUrl.begin(), lowerUrl.end(), lowerUrl.begin(), ::tolower);

    for (const auto &ext : excludeExt)
    {
        if (lowerUrl.find(ext) != std::string::npos)
        {
            return false;
        }
    }

    return true;
}