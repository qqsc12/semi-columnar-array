#ifndef COLUMNAR_ARRAY_HPP
#define COLUMNAR_ARRAY_HPP

#include <cstdlib>
#include <stdexcept>
#include <iostream>

template <typename T>
class ColumnarArray {
public:
    // 建構子：申請對齊 64-byte 的記憶體
    ColumnarArray(size_t size) : m_size(size) {
        size_t total_bytes = size * sizeof(T);
        
        // 為了確保對齊安全，我們把總申請量補足到 64 的倍數
        size_t padding = (total_bytes % 64 == 0) ? 0 : (64 - (total_bytes % 64));
        size_t alloc_size = total_bytes + padding;

        // 使用 Unit 7 學到的對齊申請函數
        m_data = static_cast<T*>(std::aligned_alloc(64, alloc_size));

        if (!m_data) {
            throw std::bad_alloc();
        }
    }

    // 解構子：手動釋放 Heap 記憶體
    ~ColumnarArray() {
        if (m_data) std::free(m_data);
    }

    // 取得資料指標與大小
    T* data() { return m_data; }
    size_t size() const { return m_size; }

    // 存取資料的 operator (像陣列一樣用法)
    T& operator[](size_t i) { return m_data[i]; }

private:
    T* m_data;
    size_t m_size;
};

#endif
