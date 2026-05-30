#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include <climits>
#include <cstddef>
#include <new>

#include "exceptions.hpp"

namespace sjtu {
template <typename T>
class vector {
   public:
   using value_type = T;

    class const_iterator;
    class iterator {
       public:
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T *;
        using reference = T &;
        using iterator_category = std::output_iterator_tag;

       private:
        pointer pos;
        const vector<T> *container;

       public:
        iterator() : pos(nullptr), container(nullptr) {
        }
        iterator(pointer pos, const vector<T> *container)
            : pos(pos), container(container) {
        }
        iterator(const iterator &rhs) : pos(rhs.pos), container(rhs.container) {
        }
        iterator operator+(const int &n) const {
            iterator ans(*this);
            ans.pos += n;
            return ans;
        }
        iterator operator-(const int &n) const {
            iterator ans(*this);
            ans.pos -= n;
            return ans;
        }
        int operator-(const iterator &rhs) const {
            if (container != rhs.container) {
                throw invalid_iterator();
            }
            if (rhs.pos > pos) {
                return rhs.pos - pos;
            } else {
                return pos - rhs.pos;
            }
        }
        iterator &operator+=(const int &n) {
            pos += n;
            return (*this);
        }
        iterator &operator-=(const int &n) {
            pos -= n;
            return (*this);
        }
        iterator operator++(int) {
            iterator ans(*this);
            pos++;
            return ans;
        }
        iterator &operator++() {
            pos++;
            return (*this);
        }
        iterator operator--(int) {
            iterator ans(*this);
            pos--;
            return ans;
        }
        iterator &operator--() {
            --pos;
            return (*this);
        }
        T &operator*() const {
            return *pos;
        }
        bool operator==(const iterator &rhs) const {
            return (pos == rhs.pos);
        }
        bool operator==(const const_iterator &rhs) const {
            return (pos == rhs.pos);
        }
        bool operator!=(const iterator &rhs) const {
            return (pos != rhs.pos);
        }
        bool operator!=(const const_iterator &rhs) const {
            return (pos != rhs.pos);
        }
    };
    class const_iterator {
       public:
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T *;
        using reference = T &;
        using iterator_category = std::output_iterator_tag;

       private:
        pointer pos;
        const vector<T> *container;

       public:
        const_iterator() : pos(nullptr), container(nullptr){};
        const_iterator(pointer pos, const vector<T> *container)
            : pos(pos), container(container){};

        const_iterator(const const_iterator &rhs) {
            pos = rhs.pos;
            container = rhs.container;
        }
        const_iterator operator+(const int &n) const {
            const_iterator ans(*this);
            ans.pos += n;
            return ans;
        }
        const_iterator operator-(const int &n) const {
            const_iterator ans(*this);
            ans.pos -= n;
            return ans;
        }
        int operator-(const const_iterator &rhs) const {
            if (container != rhs.container) {
                throw invalid_iterator();
            }
            return pos - rhs.pos;
        }
        const_iterator &operator+=(const int &n) {
            pos += n;
            return (*this);
        }
        const_iterator &operator-=(const int &n) {
            pos -= n;
            return (*this);
        }
        const_iterator operator++(int) {
            const_iterator ans(*this);
            pos++;
            return ans;
        }
        const_iterator &operator++() {
            pos++;
            return (*this);
        }
        const_iterator operator--(int) {
            const_iterator ans(*this);
            pos--;
            return ans;
        }
        const_iterator &operator--() {
            --pos;
            return (*this);
        }
        T &operator*() const {
            return *pos;
        }
        bool operator==(const iterator &rhs) const {
            return (pos == rhs.pos);
        }
        bool operator==(const const_iterator &rhs) const {
            return (pos == rhs.pos);
        }
        bool operator!=(const iterator &rhs) const {
            return (pos != rhs.pos);
        }
        bool operator!=(const const_iterator &rhs) const {
            return (pos != rhs.pos);
        }
    };

   private:
    size_t len, sz;
    T *val;
    void DoubleSpace() {
        T *tmp = val;
        len *= 2;
        val = static_cast<T *>(operator new[](len * sizeof(T)));
        for (int i = 0; i < (int)sz; i++) {
            new (&val[i]) T(tmp[i]);
            tmp[i].~T();
        }
        operator delete[](tmp);
    }

   public:
    vector() : len(1), sz(0) {
        val = static_cast<T *>(operator new[](len * sizeof(T)));
    }
    vector(const vector &other) : len(other.len), sz(other.sz) {
        val = static_cast<T *>(operator new[](len * sizeof(T)));
        for (int i = 0; i < (int)sz; i++) {
            new (&val[i]) T(other[i]);
        }
    }
    ~vector() {
        for (int i = 0; i < (int)sz; i++) {
            val[i].~T();
        }
        operator delete[](val);
    }
    vector &operator=(const vector &other) {
        if (&other == this) {
            return (*this);
        }
        for (int i = 0; i < (int)sz; i++) {
            val[i].~T();
        }
        operator delete[](val);
        len = other.len;
        sz = other.sz;
        val = static_cast<T *>(operator new[](len * sizeof(T)));
        for (int i = 0; i < (int)sz; i++) {
            new (&val[i]) T(other[i]);
        }
        return (*this);
    }
    T &at(const size_t &pos) {
        if (pos < 0 || pos >= sz) {
            throw index_out_of_bound();
        }
        return val[pos];
    }
    const T &at(const size_t &pos) const {
        if (pos < 0 || pos >= sz) {
            throw index_out_of_bound();
        }
        return val[pos];
    }
    T &operator[](const size_t &pos) {
        if (pos < 0 || pos >= sz) {
            throw index_out_of_bound();
        }
        return val[pos];
    }
    const T &operator[](const size_t &pos) const {
        if (pos < 0 || pos >= sz) {
            throw index_out_of_bound();
        }
        return val[pos];
    }
    const T &front() const {
        if (sz == 0) {
            throw container_is_empty();
        }
        return val[0];
    }
    const T &back() const {
        if (sz == 0) {
            throw container_is_empty();
        }
        return val[sz - 1];
    }
    iterator begin() {
        iterator ans(val, this);
        return ans;
    }
    const_iterator begin() const {
        const_iterator ans(val, this);
        return ans;
    }
    const_iterator cbegin() const {
        const_iterator ans(val, this);
        return ans;
    }
    iterator end() {
        iterator ans(val + sz, this);
        return ans;
    }
    const_iterator end() const {
        const_iterator ans(val + sz, this);
        return ans;
    }
    const_iterator cend() const {
        const_iterator ans(val + sz, this);
        return ans;
    }
    bool empty() const {
        return (sz == 0);
    }
    size_t size() const {
        return sz;
    }
    void clear() {
        for (int i = 0; i < (int)sz; i++) {
            val[i].~T();
        }
        sz = 0;
    }
    iterator insert(iterator pos, const T &value) {
        int ind = pos - begin();
        if (sz == len) {
            DoubleSpace();
        }
        for (int i = sz; i > ind; i--) {
            new (&val[i]) T(val[i - 1]);
            val[i - 1].~T();
        }
        new (&val[ind]) T(value);
        sz++;
        iterator ans(val + ind, this);
        return ans;
    }
    iterator insert(const size_t &ind, const T &value) {
        if (ind > sz) {
            throw index_out_of_bound();
        }
        if (sz == len) {
            DoubleSpace();
        }
        for (int i = sz; i > ind; i--) {
            new (&val[i]) T(val[i - 1]);
            val[i - 1].~T();
        }
        new (&val[ind]) T(value);
        sz++;
        iterator ans(val + ind, this);
        return ans;
    }
    iterator erase(iterator pos) {
        int ind = pos - begin();
        for (int i = ind; i + 1 < sz; i++) {
            val[i].~T();
            new (&val[i]) T(val[i + 1]);
        }
        val[sz - 1].~T();
        sz--;
        return pos;
    }
    iterator erase(const size_t &ind) {
        if (ind >= sz) {
            throw index_out_of_bound();
        }
        for (int i = ind; i + 1 < sz; i++) {
            val[i].~T();
            new (&val[i]) T(val[i + 1]);
        }
        val[sz - 1].~T();
        sz--;
        iterator ans(val + ind, this);
        return ans;
    }
    void push_back(const T &value) {
        if (sz == len) {
            DoubleSpace();
        }
        new (&val[sz++]) T(value);
    }
    void pop_back() {
        if (sz == 0) {
            throw container_is_empty();
        }
        val[sz - 1].~T();
        sz--;
    }
};

}  // namespace sjtu

#endif
