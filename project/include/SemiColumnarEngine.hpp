#ifndef SEMI_COLUMNAR_ENGINE_HPP
#define SEMI_COLUMNAR_ENGINE_HPP

#include "ColumnarArray.hpp"
#include <vector>
#include <string>
#include <cmath>

// =============================================================================
// ✨ Unit 11 概念實作：Row-accessor 橫向存取把手 (Proxy Handle)
// 因為資料是直著排 (SoA)，使用者想看「某一筆量測的所有參數」時，我們給他一個輕量把手
// =============================================================================
class RowHandle {
public:
    RowHandle(double* vg_ptr, double* id_ptr, size_t idx) 
        : m_vg_ptr(vg_ptr), m_id_ptr(id_ptr), m_idx(idx) {}

    double vg() const { return m_vg_ptr[m_idx]; }
    double& vg() { return m_vg_ptr[m_idx]; }

    double id() const { return m_id_ptr[m_idx]; }
    double& id() { return m_id_ptr[m_idx]; }

private:
    double* m_vg_ptr;
    double* m_id_ptr;
    size_t m_idx;
};

// =============================================================================
// SemiColumnarEngine: 掌管整張 B1500 電性表的大管家 (SoA 結構)
// =============================================================================
class SemiColumnarEngine {
public:
    SemiColumnarEngine(size_t num_samples) 
        : m_num_samples(num_samples),
          m_vg(num_samples), 
          m_id(num_samples) {}

    // 取得特定的 Row 把手
    RowHandle row(size_t idx) {
        if (idx >= m_num_samples) throw std::out_of_range("Index out of bound");
        return RowHandle(m_vg.data(), m_id.data(), idx);
    }

    // =========================================================================
    // 📈 Week 2 數學計算核 (Math Kernels)
    // 利用直著排 (SoA) 的記憶體連續性，全速前進計算，對快取極度友善！
    // =========================================================================
    
    // 計算平均值 (Mean)
    double calculate_mean_id() const {
        if (m_num_samples == 0) return 0.0;
        double sum = 0.0;
        const double* id_ptr = m_id.data();
        for (size_t i = 0; i < m_num_samples; ++i) {
            sum += id_ptr[i]; // 連續記憶體存取，快取命中率 100%!
        }
        return sum / m_num_samples;
    }

    // 計算方差/變異數 (Variance)
    double calculate_variance_id() const {
        if (m_num_samples <= 1) return 0.0;
        double mean = calculate_mean_id();
        double sum_sq_diff = 0.0;
        const double* id_ptr = m_id.data();
        for (size_t i = 0; i < m_num_samples; ++i) {
            double diff = id_ptr[i] - mean;
            sum_sq_diff += diff * diff;
        }
        return sum_sq_diff / m_num_samples;
    }

    size_t num_samples() const { return m_num_samples; }

private:
    size_t m_num_samples;
    ColumnarArray<double> m_vg; // 閘極電壓列
    ColumnarArray<double> m_id; // 汲極電流列
};

#endif
