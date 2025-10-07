cmake_minimum_required(VERSION 3.10)
project(MiniNginx)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置编译器标志
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# 包含头文件目录
include_directories(include)

# 直接列出所有源文件
set(SOURCES
    src/main.cpp
    src/net/Server.cpp
    src/http/Request.cpp
    src/http/Response.cpp
    src/handlers/StaticHandler.cpp
    src/handlers/ProxyHandler.cpp
)

# 添加可执行文件
add_executable(MiniNginx ${SOURCES})

# 设置输出目录
set_target_properties(MiniNginx PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)