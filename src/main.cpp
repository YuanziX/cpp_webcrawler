#include "crawler.h"
#include <iostream>
#include <string>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <url> <depth>\n";
        std::cerr << "Example: " << argv[0] << " https://example.com 2\n";
        return 1;
    }

    std::string url = argv[1];
    int depth = std::stoi(argv[2]);

    if (depth < 0)
    {
        std::cerr << "Depth must be non-negative\n";
        return 1;
    }

    std::cout << "Starting crawl of " << url << " with depth " << depth << "\n";
    std::cout << "================================================\n\n";

    Crawler crawler(8);
    crawler.crawl(url, depth);

    std::cout << "\n================================================\n";
    std::cout << "Crawl complete!\n";
    std::cout << "Total URLs crawled: " << crawler.getCrawledCount() << "\n";

    return 0;
}