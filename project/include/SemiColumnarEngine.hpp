#ifndef SEMI_COLUMNAR_ENGINE_HPP
#define SEMI_COLUMNAR_ENGINE_HPP

#include "ColumnarArray.hpp"
#include <cstddef>

class SemiColumnarEngine {
public:
    SemiColumnarEngine(size_t size) 
        : m_size(size), m_vg(size), m_vd(size), m_abs_ig(size), m_abs_id(size), m_abs_is(size) {}

    size_t size() { return m_size; }
    
    ColumnarArray<double>& v_g() { return m_vg; }
    ColumnarArray<double>& v_d() { return m_vd; }
    ColumnarArray<double>& abs_i_g() { return m_abs_ig; }
    ColumnarArray<double>& abs_i_d() { return m_abs_id; }
    ColumnarArray<double>& abs_i_s() { return m_abs_is; }

private:
    size_t m_size;
    ColumnarArray<double> m_vg;
    ColumnarArray<double> m_vd;
    ColumnarArray<double> m_abs_ig;
    ColumnarArray<double> m_abs_id;
    ColumnarArray<double> m_abs_is;
};

#endif
