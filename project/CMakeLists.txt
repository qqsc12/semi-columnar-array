cmake_minimum_required(VERSION 3.10)
project(SemiDataEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 告訴編譯器去哪裡找 .hpp 檔案
include_directories(include)

# 建立執行檔
add_executable(test_kernel tests/test_kernel.cpp)
