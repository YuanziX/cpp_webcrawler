A high-performance multi-threaded web crawler written in C++ that crawls websites in parallel for maximum speed.

## Building

```bash
mkdir build && cd build
cmake ..
make
```

## Usage

```bash
./webcrawler <url> <depth>
```

**Examples:**

```bash
# Crawl example.com with depth 2
./webcrawler https://example.com 2

# Crawl a news site with depth 1
./webcrawler https://news.ycombinator.com 1
```

**Parameters:**

- `url`: The starting URL (must include http:// or https://)
- `depth`: How many levels deep to crawl (0 = only the starting page)
