cmake_minimum_required(VERSION 3.10)
project(SemiDataEngine)

# 強制要求 C++17 標準，這是 pybind11 與對齊分配的基礎
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 告訴編譯器去哪裡找我們的 C++ 核心標頭檔 (.hpp)
include_directories(include)

# =========================================================================
# ⚙️ Week 3 核心升級：尋找並引進本地的 pybind11 套件
# =========================================================================
find_package(pybind11 REQUIRED)

# 1. 保留原本的 C++ 測試（Week 1 & 2 的核心硬體與數學驗證）
add_executable(test_kernel tests/test_kernel.cpp)

# 2. 建立 Python 專屬的動態連結庫模組 (_semi_columnar_engine)
# 這裡會把 src/wrapper.cpp 編譯成 Python 可以直接 import 的套件！
pybind11_add_module(_semi_columnar_engine src/wrapper.cpp)
