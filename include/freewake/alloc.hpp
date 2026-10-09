#pragma once

#include <vector>
#include <memory>
#include <array>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <utility>

inline double my_memory_allocated = 0.0;
inline double my_memory_deleted = 0.0;
#define GLOBAL_VAR_FOR_ALLOCATION my_memory_allocated
#define GLOBAL_VAR_FOR_DELETION my_memory_deleted

inline bool is_zero(double x, double eps = 1.0e-8) {
    return std::abs(x) < eps;
}

inline double TotalMemoryAllocated()  // in bytes
{
    return GLOBAL_VAR_FOR_ALLOCATION;
}

inline double TotalMemoryDeleted()   // in bytes
{
    return GLOBAL_VAR_FOR_DELETION;
}

inline double TotalMemoryConsumed()  // in bytes
{
    return (GLOBAL_VAR_FOR_ALLOCATION - GLOBAL_VAR_FOR_DELETION);
}

// ===========================================================================
// Modern RAII Multi-Dimensional Dynamic Containers
// ===========================================================================

template <typename T>
class Array2D {
private:
    std::vector<T> data_;
    std::vector<T*> rows_;
    size_t m_ = 0;
    size_t n_ = 0;

    void rebuild_rows() {
        rows_.resize(m_);
        for (size_t i = 0; i < m_; ++i) {
            rows_[i] = data_.empty() ? nullptr : &data_[i * n_];
        }
    }

public:
    Array2D() = default;
    Array2D(size_t m, size_t n, const T& val = T{}) {
        allocate(m, n, val);
    }

    void allocate(size_t m, size_t n, const T& val = T{}) {
        m_ = m;
        n_ = n;
        data_.assign(m * n, val);
        rebuild_rows();
    }

    void clear() {
        data_.clear();
        rows_.clear();
        m_ = 0;
        n_ = 0;
    }

    T** data() noexcept { return rows_.empty() ? nullptr : rows_.data(); }
    const T* const* data() const noexcept { return rows_.empty() ? nullptr : rows_.data(); }

    T* operator[](size_t i) noexcept { return rows_[i]; }
    const T* operator[](size_t i) const noexcept { return rows_[i]; }

    size_t rows() const noexcept { return m_; }
    size_t cols() const noexcept { return n_; }
    bool empty() const noexcept { return data_.empty(); }
};

template <typename T>
class Array3D {
private:
    std::vector<T> data_;
    std::vector<T*> rows_;
    std::vector<T**> slices_;
    size_t d1_ = 0, d2_ = 0, d3_ = 0;

    void rebuild_pointers() {
        rows_.resize(d1_ * d2_);
        slices_.resize(d1_);
        for (size_t i = 0; i < d1_; ++i) {
            slices_[i] = rows_.empty() ? nullptr : &rows_[i * d2_];
            for (size_t j = 0; j < d2_; ++j) {
                rows_[i * d2_ + j] = data_.empty() ? nullptr : &data_[(i * d2_ + j) * d3_];
            }
        }
    }

public:
    Array3D() = default;
    Array3D(size_t d1, size_t d2, size_t d3, const T& val = T{}) {
        allocate(d1, d2, d3, val);
    }

    void allocate(size_t d1, size_t d2, size_t d3, const T& val = T{}) {
        d1_ = d1; d2_ = d2; d3_ = d3;
        data_.assign(d1 * d2 * d3, val);
        rebuild_pointers();
    }

    void clear() {
        data_.clear();
        rows_.clear();
        slices_.clear();
        d1_ = d2_ = d3_ = 0;
    }

    T*** data() noexcept { return slices_.empty() ? nullptr : slices_.data(); }

    T** operator[](size_t i) noexcept { return slices_[i]; }
    const T* const* operator[](size_t i) const noexcept { return slices_[i]; }

    size_t dim1() const noexcept { return d1_; }
    size_t dim2() const noexcept { return d2_; }
    size_t dim3() const noexcept { return d3_; }
    bool empty() const noexcept { return data_.empty(); }
};

// ===========================================================================
// Modernized Smart-Pointer Dynamic Memory Allocations
// ===========================================================================

template <class Etype>
inline void ALLOC1D(Etype **ptr, int m = 1)
{
    auto u = std::make_unique<Etype[]>(m);
    GLOBAL_VAR_FOR_ALLOCATION += 1.0 * m * sizeof(Etype);
    *ptr = u.release();
}

template <class Etype>
inline void MY_MALLOC(Etype **ptr, int n = 1)
{
    ALLOC1D(ptr, n);
}

template <class Etype>
inline void ALLOC2D(Etype ***ptr, int m, int n)
{
    auto rows = std::make_unique<Etype*[]>(m);
    auto data = std::make_unique<Etype[]>(m * n);
    for (int i = 0; i < m; ++i) {
        rows[i] = &data[i * n];
    }
    GLOBAL_VAR_FOR_ALLOCATION += 1.0 * (m * sizeof(Etype*) + m * n * sizeof(Etype));
    data.release();
    *ptr = rows.release();
}

template <class Etype>
inline void ALLOC3D(Etype ****ptr, int m, int n, int o)
{
    auto slices = std::make_unique<Etype**[]>(m);
    auto rows = std::make_unique<Etype*[]>(m * n);
    auto data = std::make_unique<Etype[]>(m * n * o);
    for (int i = 0; i < m; ++i) {
        slices[i] = &rows[i * n];
        for (int j = 0; j < n; ++j) {
            rows[i * n + j] = &data[(i * n + j) * o];
        }
    }
    GLOBAL_VAR_FOR_ALLOCATION += 1.0 * (m * sizeof(Etype**) + m * n * sizeof(Etype*) + m * n * o * sizeof(Etype));
    data.release();
    rows.release();
    *ptr = slices.release();
}

template <class Etype>
inline void ALLOC4D(Etype *****ptr, int m, int n, int o, int p)
{
    auto vols = std::make_unique<Etype***[]>(m);
    auto slices = std::make_unique<Etype**[]>(m * n);
    auto rows = std::make_unique<Etype*[]>(m * n * o);
    auto data = std::make_unique<Etype[]>(m * n * o * p);
    for (int i = 0; i < m; ++i) {
        vols[i] = &slices[i * n];
        for (int j = 0; j < n; ++j) {
            slices[i * n + j] = &rows[(i * n + j) * o];
            for (int k = 0; k < o; ++k) {
                rows[(i * n + j) * o + k] = &data[((i * n + j) * o + k) * p];
            }
        }
    }
    GLOBAL_VAR_FOR_ALLOCATION += 1.0 * (m * sizeof(Etype***) + m * n * sizeof(Etype**) + m * n * o * sizeof(Etype*) + m * n * o * p * sizeof(Etype));
    data.release();
    rows.release();
    slices.release();
    *ptr = vols.release();
}

template <class Etype>
inline void FREE1D(Etype **ptr, int m = 1)
{
    if (!ptr || !*ptr) return;
    std::unique_ptr<Etype[]> u(*ptr);
    *ptr = nullptr;
    GLOBAL_VAR_FOR_DELETION += 1.0 * m * sizeof(Etype);
}

template <class Etype>
inline void FREE2D(Etype ***ptr, int m, int n)
{
    if (!ptr || !*ptr) return;
    Etype **rows = *ptr;
    if (rows && rows[0]) {
        std::unique_ptr<Etype[]> data(rows[0]);
    }
    std::unique_ptr<Etype*[]> r(rows);
    *ptr = nullptr;
    GLOBAL_VAR_FOR_DELETION += 1.0 * (m * sizeof(Etype*) + m * n * sizeof(Etype));
}

template <class Etype>
inline void FREE3D(Etype ****ptr, int m, int n, int o)
{
    if (!ptr || !*ptr) return;
    Etype ***slices = *ptr;
    if (slices && slices[0] && slices[0][0]) {
        std::unique_ptr<Etype[]> data(slices[0][0]);
        std::unique_ptr<Etype*[]> rows(slices[0]);
    }
    std::unique_ptr<Etype**[]> s(slices);
    *ptr = nullptr;
    GLOBAL_VAR_FOR_DELETION += 1.0 * (m * sizeof(Etype**) + m * n * sizeof(Etype*) + m * n * o * sizeof(Etype));
}

template <class Etype>
inline void FREE4D(Etype *****ptr, int m, int n, int o, int p)
{
    if (!ptr || !*ptr) return;
    Etype ****vols = *ptr;
    if (vols && vols[0] && vols[0][0] && vols[0][0][0]) {
        std::unique_ptr<Etype[]> data(vols[0][0][0]);
        std::unique_ptr<Etype*[]> rows(vols[0][0]);
        std::unique_ptr<Etype**[]> slices(vols[0]);
    }
    std::unique_ptr<Etype***[]> v(vols);
    *ptr = nullptr;
    GLOBAL_VAR_FOR_DELETION += 1.0 * (m * sizeof(Etype***) + m * n * sizeof(Etype**) + m * n * o * sizeof(Etype*) + m * n * o * p * sizeof(Etype));
}

