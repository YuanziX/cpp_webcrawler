#include "http_client.h"
#include <curl/curl.h>
#include <iostream>
#include <thread>
#include <chrono>

HttpClient::HttpClient()
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

size_t HttpClient::writeCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t totalSize = size * nmemb;
    std::string *str = static_cast<std::string *>(userp);
    str->append(static_cast<char *>(contents), totalSize);
    return totalSize;
}

std::string HttpClient::fetch(const std::string &url)
{
    const int maxRetries = 2;
    for (int attempt = 0; attempt <= maxRetries; ++attempt)
    {
        CURL *curl = curl_easy_init();
        if (!curl)
        {
            return "";
        }

        std::string response;

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "ParallelWebCrawler/1.0");
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

        CURLcode res = curl_easy_perform(curl);

        if (res == CURLE_OK)
        {
            curl_easy_cleanup(curl);
            return response;
        }

        if (attempt < maxRetries)
        {
            std::cerr << "Attempt " << (attempt + 1) << " failed for " << url
                      << ", retrying... (" << curl_easy_strerror(res) << ")" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        else
        {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
    }

    return "";
}