#ifndef SEMI_COLUMNAR_ENGINE_HPP
#define SEMI_COLUMNAR_ENGINE_HPP

#include <vector>
#include <cmath>
#include "ColumnarArray.hpp"

// 橫向存取的代理把手 (Proxy Handle)
class RowHandle {
public:
    RowHandle(size_t index, ColumnarArray<double>& vg, ColumnarArray<double>& id)
        : m_index(index), m_vg(vg), m_id(id) {}

    double& vg() { return m_vg[m_index]; }
    double& id() { return m_id[m_index]; }

private:
    size_t m_index;
    ColumnarArray<double>& m_vg;
    ColumnarArray<double>& m_id;
};

// Week 2 SoA 大管家主引擎 (修正版，完美對齊 pybind11 格式)
class SemiColumnarEngine {
public:
    SemiColumnarEngine(size_t size) : m_size(size), m_vg(size), m_id(size) {}

    RowHandle row(size_t i) {
        return RowHandle(i, m_vg, m_id);
    }

    size_t size() { return m_size; }
    ColumnarArray<double>& v_g() { return m_vg; }
    ColumnarArray<double>& i_d() { return m_id; }

    // 移除末尾的 const，完美相容 ColumnarArray 的原始指針存取
    double calculate_mean_id() {
        if (m_size == 0) return 0.0;
        double sum = 0.0;
        for (size_t i = 0; i < m_size; ++i) {
            sum += m_id[i]; 
        }
        return sum / m_size;
    }

    // 移除末尾的 const，確保統計核穩定執行
    double calculate_variance_id() {
        if (m_size == 0) return 0.0;
        double mean = calculate_mean_id();
        double sum_sq_diff = 0.0;
        for (size_t i = 0; i < m_size; ++i) {
            double diff = m_id[i] - mean;
            sum_sq_diff += diff * diff;
        }
        return sum_sq_diff / m_size;
    }

private:
    size_t m_size;
    ColumnarArray<double> m_vg; 
    ColumnarArray<double> m_id; 
};

#endif
