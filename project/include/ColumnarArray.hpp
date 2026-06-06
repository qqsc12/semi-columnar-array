#ifndef COLUMNAR_ARRAY_HPP
#define COLUMNAR_ARRAY_HPP

#include <cstddef>
#include <cstdlib>
#include <new>
#include <algorithm>
#include <stdexcept>

template <typename T>
class ColumnarArray {
public:
    // 🌟 預設建構子：允許出生時大小為 0，後面再動態長大
    ColumnarArray() : m_size(0), m_capacity(0), m_raw_data(nullptr) {}

    ColumnarArray(size_t size) : m_size(size), m_capacity(size) {
        if (m_size > 0) {
            size_t byte_size = ((m_size * sizeof(T) + 63) / 64) * 64; // 向上對齊 64 字节的倍數
            m_raw_data = (T*)aligned_alloc(64, byte_size);
            if (!m_raw_data) throw std::bad_alloc();
            for (size_t i = 0; i < m_size; ++i) {
                new (&m_raw_data[i]) T();
            }
        } else {
            m_raw_data = nullptr;
        }
    }

    ~ColumnarArray() {
        clear();
    }

    // 捍衛記憶體所有權，禁止複製
    ColumnarArray(const ColumnarArray&) = delete;
    ColumnarArray& operator=(const ColumnarArray&) = delete;

    // 允許移動（Move）語意，這在動態擴容時極度重要
    ColumnarArray(ColumnarArray&& other) noexcept 
        : m_size(other.m_size), m_capacity(other.m_capacity), m_raw_data(other.m_raw_data) {
        other.m_size = 0;
        other.m_capacity = 0;
        other.m_raw_data = nullptr;
    }

    ColumnarArray& operator=(ColumnarArray&& other) noexcept {
        if (this != &other) {
            clear();
            m_size = other.m_size;
            m_capacity = other.m_capacity;
            m_raw_data = other.m_raw_data;
            other.m_size = 0;
            other.m_capacity = 0;
            other.m_raw_data = nullptr;
        }
        return *this;
    }

    size_t size() const { return m_size; }
    T* data() { return m_raw_data; }
    const T* data() const { return m_raw_data; }

    T& operator[](size_t idx) { return m_raw_data[idx]; }
    const T& operator[](size_t idx) const { return m_raw_data[idx]; }

    // 🌟 老師要求的關鍵 1：尾部追加元件資料，自動處理擴容
    void push_back(const T& value) {
        if (m_size >= m_capacity) {
            size_t new_capacity = m_capacity == 0 ? 16 : m_capacity * 2;
            reserve(new_capacity);
        }
        new (&m_raw_data[m_size]) T(value);
        m_size++;
    }

    // 🌟 老師要求的關鍵 2：動態重塑並重新分配大小（Reshape / Resize）
    void resize(size_t new_size) {
        if (new_size > m_capacity) {
            reserve(new_size);
        }
        if (new_size > m_size) {
            for (size_t i = m_size; i < new_size; ++i) {
                new (&m_raw_data[i]) T();
            }
        } else if (new_size < m_size) {
            for (size_t i = new_size; i < m_size; ++i) {
                m_raw_data[i].~T();
            }
        }
        m_size = new_size;
    }

private:
    size_t m_size;
    size_t m_capacity;
    T* m_raw_data;

    void clear() {
        if (m_raw_data) {
            for (size_t i = 0; i < m_size; ++i) {
                m_raw_data[i].~T();
            }
            std::free(m_raw_data);
            m_raw_data = nullptr;
        }
        m_size = 0;
        m_capacity = 0;
    }

    // 擴充保險箱容量，並保證新保險箱依然是 64-byte 嚴格對齊
    void reserve(size_t new_capacity) {
        if (new_capacity <= m_capacity) return;

        size_t byte_size = ((new_capacity * sizeof(T) + 63) / 64) * 64;
        T* new_data = (T*)aligned_alloc(64, byte_size);
        if (!new_data) throw std::bad_alloc();

        for (size_t i = 0; i < m_size; ++i) {
            new (&new_data[i]) T(std::move(m_raw_data[i]));
            m_raw_data[i].~T();
        }

        std::free(m_raw_data);
        m_raw_data = new_data;
        m_capacity = new_capacity;
    }
};

#endif
