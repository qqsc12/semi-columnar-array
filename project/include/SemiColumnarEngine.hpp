#ifndef SEMI_COLUMNAR_ENGINE_HPP
#define SEMI_COLUMNAR_ENGINE_HPP

#include "ColumnarArray.hpp"
#include <cstddef>
#include <cmath>
#include <algorithm>
#include <vector>

class SemiColumnarEngine {
public:
    SemiColumnarEngine() : m_size(0) {}
    SemiColumnarEngine(size_t size) 
        : m_size(size), m_vg(size), m_vd(size), m_abs_ig(size), m_abs_id(size), m_abs_is(size) {}

    size_t size() const { return m_size; }
    
    void resize(size_t new_size) {
        m_size = new_size;
        m_vg.resize(new_size);
        m_vd.resize(new_size);
        m_abs_ig.resize(new_size);
        m_abs_id.resize(new_size);
        m_abs_is.resize(new_size);
    }
    
    ColumnarArray<double>& v_g() { return m_vg; }
    ColumnarArray<double>& v_d() { return m_vd; }
    ColumnarArray<double>& abs_i_g() { return m_abs_ig; }
    ColumnarArray<double>& abs_i_d() { return m_abs_id; }
    ColumnarArray<double>& abs_i_s() { return m_abs_is; }

    // =========================================================================
    // ⚡ 1. 萃取 On-Off Ratio：支援去回程拆分
    // =========================================================================
    double extract_on_off_ratio(bool is_backward) {
        size_t half = m_size / 2;
        size_t start = is_backward ? half : 0;
        size_t end = is_backward ? m_size : half;
        if (end <= start) return 0.0;

        double max_id = m_abs_id[start];
        double min_id = m_abs_id[start];
        for (size_t i = start + 1; i < end; ++i) {
            if (m_abs_id[i] > max_id) max_id = m_abs_id[i];
            if (m_abs_id[i] < min_id && m_abs_id[i] > 0.0) min_id = m_abs_id[i];
        }
        return max_id / min_id;
    }

    // =========================================================================
    // ⚡ 2. 萃取 Vth（最大 Gm 外推法）：加入 5 點中央差分平滑濾波
    // =========================================================================
    double extract_vth(bool is_backward) {
        size_t half = m_size / 2;
        size_t start = is_backward ? half : 0;
        size_t end = is_backward ? m_size : half;
        if ((end - start) < 5) return 0.0;

        double max_gm = -1.0;
        size_t max_gm_idx = start + 2;

        // 5 點微積分平滑濾波
        for (size_t i = start + 2; i < end - 2; ++i) {
            double dVg = m_vg[i+1] - m_vg[i-1];
            if (dVg == 0.0) continue;
            
            // 5 點平滑轉導計算
            double gm = (-m_abs_id[i+2] + 8.0*m_abs_id[i+1] - 8.0*m_abs_id[i-1] + m_abs_id[i+2]) / (6.0 * dVg);
            gm = std::abs(gm); // 確保不論掃描方向皆為正值

            if (gm > max_gm) {
                max_gm = gm;
                max_gm_idx = i;
            }
        }

        double target_vg = m_vg[max_gm_idx];
        double target_id = m_abs_id[max_gm_idx];
        if (max_gm == 0.0) return 0.0;
        
        return target_vg - (target_id / max_gm);
    }

    // =========================================================================
    // ⚡ 3. 萃取 SS（次臨限擺幅）：嚴格鎖定 10^-10A 到 10^-8A 物理區間，防禦漏電雜訊
    // =========================================================================
    double extract_ss(bool is_backward) {
        size_t half = m_size / 2;
        size_t start = is_backward ? half : 0;
        size_t end = is_backward ? m_size : half;

        double max_slope = -1.0;

        for (size_t i = start + 1; i < end - 1; ++i) {
            double id_curr = m_abs_id[i];
            // 🌟 關鍵防禦細節：只在標準次臨限線性下坡區間 (10^-10 A ~ 10^-8 A) 計算斜率
            if (id_curr < 1e-10 || id_curr > 1e-8) continue;

            double dVg = m_vg[i+1] - m_vg[i-1];
            if (dVg == 0.0) continue;

            double dLogId = std::log10(m_abs_id[i+1]) - std::log10(m_abs_id[i-1]);
            double slope = std::abs(dLogId / dVg);

            if (slope > max_slope) {
                max_slope = slope;
            }
        }

        if (max_slope <= 0.0) return 0.0;
        return (1.0 / max_slope) * 1000.0; // 單位：mV/dec
    }

private:
    size_t m_size;
    ColumnarArray<double> m_vg;
    ColumnarArray<double> m_vd;
    ColumnarArray<double> m_abs_ig;
    ColumnarArray<double> m_abs_id;
    ColumnarArray<double> m_abs_is;
};

#endif
