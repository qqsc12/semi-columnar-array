#include <iostream>
#include <cstdint>
#include <cassert>
#include "ColumnarArray.hpp"
#include "SemiColumnarEngine.hpp"

int main() {
    std::cout << "=========================================" << std::endl;
    std::cout << "⚙️ NSD Term Project: Semi-Columnar Engine Test" << std::endl;
    std::cout << "=========================================" << std::endl;

    // 1. 驗證 Week 1: 記憶體對齊
    ColumnarArray<double> alignment_check(100);
    uintptr_t addr = reinterpret_cast<uintptr_t>(alignment_check.data());
    std::cout << "[Week 1] Memory Address: 0x" << std::hex << addr << std::dec << std::endl;
    assert(addr % 64 == 0);
    std::cout << "[SUCCESS] 64-byte alignment verified!" << std::endl;

    // 2. 驗證 Week 2: SoA 大管家與數學核心精度
    size_t samples = 4;
    SemiColumnarEngine engine(samples);

    // 模擬 Keysight B1500 塞入電性數據 (利用 RowHandle 橫向塞入)
    // 假設我們注入一組電流數據 Id: [1.0, 2.0, 3.0, 4.0] (單位: mA)
    engine.row(0).id() = 1.0;
    engine.row(1).id() = 2.0;
    engine.row(2).id() = 3.0;
    engine.row(3).id() = 4.0;

    // 驗證平均值計算核 (Mean)
    // Expected: (1+2+3+4)/4 = 2.5
    double mean_id = engine.calculate_mean_id();
    std::cout << "[Week 2] Calculated Mean Id: " << mean_id << " mA" << std::endl;
    assert(std::abs(mean_id - 2.5) < 1e-6);

    // 驗證方差計算核 (Variance)
    // Expected: ((1-2.5)^2 + (2-2.5)^2 + (3-2.5)^2 + (4-2.5)^2) / 4 = 1.25
    double var_id = engine.calculate_variance_id();
    std::cout << "[Week 2] Calculated Variance Id: " << var_id << std::endl;
    assert(std::abs(var_id - 1.25) < 1e-6);

    std::cout << "[SUCCESS] Math kernels (Mean/Variance) accuracy verified!" << std::endl;
    std::cout << "\n🎉 All tests passed successfully!" << std::endl;

    return 0;
}
