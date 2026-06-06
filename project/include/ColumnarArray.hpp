#ifndef COLUMNAR_ARRAY_HPP
#define COLUMNAR_ARRAY_HPP

#include <cstdlib>
#include <stdexcept>
#include <new>

template <typename T>
class ColumnarArray {
public:
    ColumnarArray(size_t size) : m_size(size), m_data(nullptr) {
        if (size == 0) return;
        size_t total_bytes = size * sizeof(T);
        size_t padding = (total_bytes % 64 == 0) ? 0 : (64 - (total_bytes % 64));
        size_t alloc_size = total_bytes + padding;

        void* raw_ptr = std::aligned_alloc(64, alloc_size);
        if (!raw_ptr) throw std::bad_alloc();
        m_data = static_cast<T*>(raw_ptr);

        for (size_t i = 0; i < m_size; ++i) {
            new (&m_data[i]) T(); 
        }
    }

    ~ColumnarArray() {
        if (m_data) {
            std::free(m_data);
            m_data = nullptr;
        }
    }

    // ✨ Unit 11 精神：禁用複製與移動，防止跨界 Double Free 記憶體崩潰
    ColumnarArray(const ColumnarArray&) = delete;
    ColumnarArray& operator=(const ColumnarArray&) = delete;
    ColumnarArray(ColumnarArray&&) = delete;
    ColumnarArray& operator=(ColumnarArray&&) = delete;

    T* data() { return m_data; }
    const T* data() const { return m_data; }
    size_t size() const { return m_size; }
    T& operator[](size_t i) { return m_data[i]; }
    const T& operator[](size_t i) const { return m_data[i]; }

private:
    T* m_data;
    size_t m_size;
};

#endif
