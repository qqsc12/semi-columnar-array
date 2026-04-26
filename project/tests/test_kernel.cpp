#include <iostream>
#include <cstdint>
#include "ColumnarArray.hpp"

int main() {
    std::cout << "--- Week 1 Test: Memory Alignment Check ---" << std::endl;

    // 建立一個存 double (B1500 常用的 8-byte 浮點數) 的列式陣列
    ColumnarArray<double> my_data(100);

    // 取得記憶體地址並轉成整數進行數學運算
    uintptr_t addr = reinterpret_cast<uintptr_t>(my_data.data());

    std::cout << "Memory address: 0x" << std::hex << addr << std::dec << std::endl;

    if (addr % 64 == 0) {
        std::cout << "[SUCCESS] Address is 64-byte aligned!" << std::endl;
    } else {
        std::cout << "[FAILED] Address is not aligned. Remainder: " << (addr % 64) << std::endl;
        return 1;
    }

    return 0;
}
