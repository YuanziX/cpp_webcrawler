cmake_minimum_required(VERSION 3.10)
project(WebCrawler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(CURL REQUIRED)
find_package(Threads REQUIRED)

# Add executable
add_executable(webcrawler
    src/main.cpp
    src/crawler.cpp
    src/http_client.cpp
    src/url_parser.cpp
)

# Include directories
target_include_directories(webcrawler PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CURL_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(webcrawler
    ${CURL_LIBRARIES}
    Threads::Threads
)

# Compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(webcrawler PRIVATE -Wall -Wextra -pedantic)
endif()