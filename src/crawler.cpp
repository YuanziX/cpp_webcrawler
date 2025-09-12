#include "crawler.h"
#include "http_client.h"
#include "url_parser.h"
#include <iostream>
#include <algorithm>

Crawler::Crawler(size_t numThreads)
    : stop_(false), activeWorkers_(0), crawledCount_(0), maxDepth_(0)
{
    for (size_t i = 0; i < numThreads; ++i)
    {
        workers_.emplace_back(&Crawler::workerThread, this);
    }
}

Crawler::~Crawler()
{
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        stop_ = true;
    }
    condition_.notify_all();

    for (auto &worker : workers_)
    {
        if (worker.joinable())
        {
            worker.join();
        }
    }
}

void Crawler::crawl(const std::string &startUrl, int maxDepth)
{
    addTask(startUrl, 0);

    // store maxdepth for workers
    maxDepth_ = maxDepth;

    // wait for all work to complete
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        std::lock_guard<std::mutex> lock(queueMutex_);
        if (taskQueue_.empty() && activeWorkers_ == 0)
        {
            break;
        }
    }
}

size_t Crawler::getCrawledCount() const
{
    return crawledCount_;
}

void Crawler::workerThread()
{
    while (true)
    {
        CrawlTask task("", -1);

        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            condition_.wait(lock, [this]
                            { return stop_ || !taskQueue_.empty(); });

            if (stop_ && taskQueue_.empty())
            {
                return;
            }

            if (!taskQueue_.empty())
            {
                task = taskQueue_.front();
                taskQueue_.pop();
                activeWorkers_++;
            }
        }

        if (task.depth >= 0)
        {
            processUrl(task.url, task.depth, maxDepth_);
            activeWorkers_--;
        }
    }
}

void Crawler::addTask(const std::string &url, int depth)
{
    if (!shouldCrawl(url))
    {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(visitedMutex_);
        if (visited_.find(url) != visited_.end())
        {
            return;
        }
        visited_.insert(url);
    }

    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        taskQueue_.emplace(url, depth);
    }
    condition_.notify_one();
}

bool Crawler::shouldCrawl(const std::string &url)
{
    // validate url
    return url.find("http://") == 0 || url.find("https://") == 0;
}

void Crawler::processUrl(const std::string &url, int currentDepth, int maxDepth)
{
    std::cout << "[Depth " << currentDepth << "] Crawling: " << url << std::endl;

    HttpClient client;
    std::string html = client.fetch(url);

    if (html.empty())
    {
        std::cerr << "[Error] Failed to fetch: " << url << std::endl;
        return;
    }

    crawledCount_++;

    // crawl till maxdepth
    if (currentDepth < maxDepth_)
    {
        UrlParser parser;
        auto links = parser.extractLinks(html, url);

        for (const auto &link : links)
        {
            addTask(link, currentDepth + 1);
        }
    }
}