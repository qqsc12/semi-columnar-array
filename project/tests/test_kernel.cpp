#include <iostream>
#include <cstdint>
#include <cmath>
#include "ColumnarArray.hpp"
#include "SemiColumnarEngine.hpp"

int main() {
    std::cout << "=========================================" << std::endl;
    std::cout << "⚙️ NSD Term Project: Semi-Columnar Engine Test" << std::endl;
    std::cout << "=========================================" << std::endl;

    // =========================================================================
    // ✅ 驗證 Week 1: 核心記憶體 64-byte 對齊測試
    // =========================================================================
    ColumnarArray<double> alignment_check(100);
    uintptr_t addr = reinterpret_cast<uintptr_t>(alignment_check.data());
    std::cout << "[Week 1] Memory Address: 0x" << std::hex << addr << std::dec << std::endl;
    
    // ✨ 修正點 1：放棄脆弱的 assert，改用明確的運行時 if 條件檢查
    if (addr % 64 == 0) {
        std::cout << "[SUCCESS] Address is 64-byte aligned!" << std::endl;
    } else {
        std::cerr << "[ERROR] Memory Alignment Failed! Address is not 64-byte aligned." << std::endl;
        return 1; // 發生硬體錯誤，強迫終止並回傳非零狀態碼
    }

    // =========================================================================
    // ✅ 驗證 Week 2: SoA 大管家與數學核心精度測試
    // =========================================================================
    size_t samples = 4;
    SemiColumnarEngine engine(samples);

    // 模擬 Keysight B1500 注入一組電流數據 Id: [1.0, 2.0, 3.0, 4.0] (mA)
    engine.row(0).id() = 1.0;
    engine.row(1).id() = 2.0;
    engine.row(2).id() = 3.0;
    engine.row(3).id() = 4.0;

    // 驗證平均值計算核 (Mean)
    // 理論值: 2.5
    double mean_id = engine.calculate_mean_id();
    std::cout << "[Week 2] Calculated Mean Id: " << mean_id << " mA" << std::endl;
    
    // ✨ 修正點 2：改用明確的絕對誤差閾值檢查，確保 Release Mode 依然具備安全監控功能
    if (std::abs(mean_id - 2.5) >= 1e-6) {
        std::cerr << "[ERROR] Mean Id calculation kernel accuracy validation failed!" << std::endl;
        return 1;
    }

    // 驗證方差計算核 (Variance)
    // 理論值: 1.25
    double var_id = engine.calculate_variance_id();
    std::cout << "[Week 2] Calculated Variance Id: " << var_id << std::endl;
    
    if (std::abs(var_id - 1.25) >= 1e-6) {
        std::cerr << "[ERROR] Variance Id calculation kernel accuracy validation failed!" << std::endl;
        return 1;
    }

    std::cout << "[SUCCESS] Math kernels (Mean/Variance) accuracy verified!" << std::endl;
    std::cout << "\n🎉 All tests passed successfully under explicit runtime validation!" << std::endl;

    return 0;
}
