cmake_minimum_required(VERSION 3.10)
project(MiniNginx)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 包含头文件目录
include_directories(include)

# 添加可执行文件
add_executable(MiniNginx src/main.cpp)

# 如果有其他源文件，可以继续添加，例如：
# target_sources(MiniNginx PRIVATE src/other.cpp)

# 设置编译器标志（可选）
target_compile_options(MiniNginx PRIVATE -Wall -Wextra)

# 安装目标（可选，后续可能需要）
# install(TARGETS MiniNginx DESTINATION bin)