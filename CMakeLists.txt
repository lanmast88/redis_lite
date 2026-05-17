cmake_minimum_required(VERSION 3.10)
project(RedisLite LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -O2)

set(SOURCES
    src/main.cpp
    src/server.cpp
    src/client_handler.cpp
    src/storage.cpp
    src/transaction.cpp
    src/command_parser.cpp
)

add_executable(redis_lite ${SOURCES})

target_include_directories(redis_lite PRIVATE include)

find_package(Threads REQUIRED)
target_link_libraries(redis_lite PRIVATE Threads::Threads)
