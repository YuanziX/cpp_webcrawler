#ifndef CRAWLER_H
#define CRAWLER_H

#include <string>
#include <unordered_set>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>
#include <atomic>

struct CrawlTask
{
    std::string url;
    int depth;

    CrawlTask(const std::string &u, int d) : url(u), depth(d) {}
};

class Crawler
{
public:
    explicit Crawler(size_t numThreads = 4);
    ~Crawler();

    void crawl(const std::string &startUrl, int maxDepth);
    size_t getCrawledCount() const;

private:
    void workerThread();
    void addTask(const std::string &url, int depth);
    bool shouldCrawl(const std::string &url);
    void processUrl(const std::string &url, int currentDepth, int maxDepth);

    std::vector<std::thread> workers_;
    std::queue<CrawlTask> taskQueue_;
    std::unordered_set<std::string> visited_;

    std::mutex queueMutex_;
    std::mutex visitedMutex_;
    std::condition_variable condition_;

    std::atomic<bool> stop_;
    std::atomic<int> activeWorkers_;
    std::atomic<size_t> crawledCount_;
    int maxDepth_;
};

#endif