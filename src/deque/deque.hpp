#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include <cstddef>
#include <optional>

#include "exceptions.hpp"

namespace sjtu {

template <class T>
class deque {
   public:
    class const_iterator;
    class iterator {
        friend class const_iterator;

       private:
        T *pos;
        int chunk_number;
        const deque<T> *container;

       public:
        iterator() : pos(nullptr), container(nullptr), chunk_number(0){};
        iterator(T *pos, const deque<T> *container, int chunk_number)
            : pos(pos), container(container), chunk_number(chunk_number) {
        }
        iterator(const iterator &rhs)
            : pos(rhs.pos),
              container(rhs.container),
              chunk_number(rhs.chunk_number) {
        }
        iterator(const const_iterator &rhs)
            : pos(rhs.pos),
              container(rhs.container),
              chunk_number(rhs.chunk_number) {
        }
        iterator operator+(const int &n) const {
            if (n < 0) {
                return iterator((*this) - (-n));
            }
            int num = pos - container->q[chunk_number];
            iterator ans(*this);
            if (num + n < chunk_size) {
                ans.pos += n;
                return ans;
            }
            int tmp = n - (chunk_size - num);
            // ans.chunk_number += 1 + (tmp >> chunk_len);
            ans.chunk_number += 1 + (tmp >> chunk_len);
            ans.pos =
                ans.container->q[ans.chunk_number] + (tmp & full_chunk_num);
            return ans;
        }
        iterator operator-(const int &n) const {
            if (n < 0) {
                return iterator((*this) + (-n));
            }
            int num = pos - container->q[chunk_number];
            iterator ans(*this);
            if (num - n >= 0) {
                ans.pos -= n;
                return ans;
            }
            int tmp = n - num - 1;
            ans.chunk_number -= 1 + (tmp >> chunk_len);
            ans.pos = ans.container->q[ans.chunk_number] + chunk_size -
                      (tmp & full_chunk_num) - 1;
            return ans;
        }
        int operator-(const iterator &rhs) const {
            if (container != rhs.container) {
                throw invalid_iterator();
            }
            int pos1 = pos - container->q[chunk_number];
            pos1 += (chunk_number - 1) * chunk_size;
            int pos2 = rhs.pos - rhs.container->q[rhs.chunk_number];
            pos2 += (rhs.chunk_number - 1) * chunk_size;
            return pos1 - pos2;
        }
        iterator operator+=(const int &n) {
            (*this) = (*this) + n;
            return (*this);
        }
        iterator operator-=(const int &n) {
            (*this) = (*this) - n;
            return (*this);
        }
        iterator operator++(int) {
            iterator ans(*this);
            int num = pos - container->q[chunk_number];
            if (num + 1 < chunk_size) {
                pos++;
            } else {
                chunk_number++;
                pos = container->q[chunk_number];
            }
            return ans;
        }
        iterator &operator++() {
            int num = pos - container->q[chunk_number];
            if (num + 1 < chunk_size) {
                pos++;
            } else {
                chunk_number++;
                pos = container->q[chunk_number];
            }
            return (*this);
        }
        iterator operator--(int) {
            iterator ans(*this);
            int num = pos - container->q[chunk_number];
            if (num > 0) {
                pos--;
            } else {
                chunk_number--;
                pos = container->q[chunk_number] + chunk_size - 1;
            }
            return ans;
        }
        iterator &operator--() {
            int num = pos - container->q[chunk_number];
            if (num > 0) {
                pos--;
            } else {
                chunk_number--;
                pos = container->q[chunk_number] + chunk_size - 1;
            }
            return (*this);
        }
        T &operator*() const {
            int ind = *this - this->container->cbegin();
            if (ind < 0 || ind >= this->container->size()) {
                throw invalid_iterator();
            }
            return (*pos);
        }
        T *operator->() const noexcept {
            int ind = *this - this->container->cbegin();
            if (ind < 0 || ind >= this->container->size()) {
                throw invalid_iterator();
            }
            return pos;
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
        friend class iterator;

       private:
        T *pos;
        int chunk_number;
        const deque<T> *container;

       public:
        const_iterator() : pos(nullptr), container(nullptr), chunk_number(0){};
        const_iterator(T *pos, const deque<T> *container, int chunk_number)
            : pos(pos), container(container), chunk_number(chunk_number) {
        }
        const_iterator(const const_iterator &rhs)
            : pos(rhs.pos),
              container(rhs.container),
              chunk_number(rhs.chunk_number) {
        }
        const_iterator(const iterator &rhs)
            : pos(rhs.pos),
              container(rhs.container),
              chunk_number(rhs.chunk_number) {
        }
        const_iterator operator+(const int &n) const {
            if (n < 0) {
                return const_iterator((*this) - (-n));
            }
            int num = pos - container->q[chunk_number];
            const_iterator ans(*this);
            if (num + n < chunk_size) {
                ans.pos += n;
                return ans;
            }
            int tmp = n - (chunk_size - num);
            ans.chunk_number += 1 + (tmp >> chunk_len);
            ans.pos =
                ans.container->q[ans.chunk_number] + (tmp & full_chunk_num);
            return ans;
        }
        const_iterator operator-(const int &n) const {
            if (n < 0) {
                return const_iterator((*this) + (-n));
            }
            int num = pos - container->q[chunk_number];
            const_iterator ans(*this);
            if (num - n >= 0) {
                ans.pos -= n;
                return ans;
            }
            int tmp = n - num - 1;
            ans.chunk_number -= 1 + (tmp >> chunk_len);
            ans.pos = ans.container->q[ans.chunk_number] + chunk_size -
                      (tmp & full_chunk_num) - 1;
            return ans;
        }
        int operator-(const const_iterator &rhs) const {
            if (container != rhs.container) {
                throw invalid_iterator();
            }
            int pos1 = pos - container->q[chunk_number];
            pos1 += (chunk_number - 1) * chunk_size;
            int pos2 = rhs.pos - rhs.container->q[rhs.chunk_number];
            pos2 += (rhs.chunk_number - 1) * chunk_size;
            return pos1 - pos2;
        }
        const_iterator operator+=(const int &n) {
            (*this) = (*this) + n;
            return (*this);
        }
        const_iterator operator-=(const int &n) {
            (*this) = (*this) - n;
            return (*this);
        }
        const_iterator operator++(int) {
            const_iterator ans(*this);
            int num = pos - container->q[chunk_number];
            if (num + 1 < chunk_size) {
                pos++;
            } else {
                chunk_number++;
                pos = container->q[chunk_number];
            }
            return ans;
        }
        const_iterator &operator++() {
            int num = pos - container->q[chunk_number];
            if (num + 1 < chunk_size) {
                pos++;
            } else {
                chunk_number++;
                pos = container->q[chunk_number];
            }
            return (*this);
        }
        const_iterator operator--(int) {
            const_iterator ans(*this);
            int num = pos - container->q[chunk_number];
            if (num > 0) {
                pos--;
            } else {
                chunk_number--;
                pos = container->q[chunk_number] + chunk_size - 1;
            }
            return ans;
        }
        const_iterator &operator--() {
            int num = pos - container->q[chunk_number];
            if (num > 0) {
                pos--;
            } else {
                chunk_number--;
                pos = container->q[chunk_number] + chunk_size - 1;
            }
            return (*this);
        }
        const T &operator*() const {
            int ind = *this - this->container->cbegin();
            if (ind < 0 || ind >= this->container->size()) {
                throw invalid_iterator();
            }
            return (*pos);
        }
        const T *operator->() const noexcept {
            int ind = *this - this->container->cbegin();
            if (ind < 0 || ind >= this->container->size()) {
                throw invalid_iterator();
            }
            return pos;
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
    static const int chunk_size = 16;
    static const int chunk_len = 4;
    static const int full_chunk_num = 15;
    int head, tail, len, sz;
    int head_pos, tail_pos;
    T **q;
    void DoubleSpace() {
        T **tmp = q;
        len *= 2;
        q = new T *[len];
        for (int i = 0; i < len; i++) {
            q[i] = static_cast<T *>(operator new[](chunk_size * sizeof(T)));
        }
        int empty_chunk = len - (tail - head + 1);
        int delta = empty_chunk / 2 - head;
        for (int i = head; i <= tail; i++) {
            int st = (i == head ? head_pos : 0);
            int ed = (i == tail ? tail_pos : chunk_size);
            for (int j = st; j < ed; j++) {
                new (&q[i + delta][j]) T(tmp[i][j]);
                tmp[i][j].~T();
            }
        }
        head += delta;
        tail += delta;
        for (int i = 0; i < len / 2; i++) {
            operator delete[](tmp[i]);
        }
        delete[] tmp;
    }

   public:
    deque() : head(1), tail(1), len(4), head_pos(1), tail_pos(1), sz(0) {
        q = new T *[len];
        for (int i = 0; i < len; i++) {
            q[i] = static_cast<T *>(operator new[](chunk_size * sizeof(T)));
        }
    }
    deque(const deque &other)
        : head(other.head),
          tail(other.tail),
          len(other.len),
          head_pos(other.head_pos),
          tail_pos(other.tail_pos),
          sz(other.sz) {
        q = new T *[len];
        for (int i = 0; i < len; i++) {
            q[i] = static_cast<T *>(operator new[](chunk_size * sizeof(T)));
        }
        for (int i = head; i <= tail; i++) {
            int st = (i == head ? head_pos : 0);
            int ed = (i == tail ? tail_pos : chunk_size);
            for (int j = st; j < ed; j++) {
                new (&q[i][j]) T(other.q[i][j]);
            }
        }
    }
    ~deque() {
        for (int i = head; i <= tail; i++) {
            int st = (i == head ? head_pos : 0);
            int ed = (i == tail ? tail_pos : chunk_size);
            for (int j = st; j < ed; j++) {
                q[i][j].~T();
            }
        }
        for (int i = 0; i < len; i++) {
            operator delete[](q[i]);
        }
        delete[] q;
    }
    deque &operator=(const deque &other) {
        if (&other == this) {
            return (*this);
        }
        for (int i = head; i <= tail; i++) {
            int st = (i == head ? head_pos : 0);
            int ed = (i == tail ? tail_pos : chunk_size);
            for (int j = st; j < ed; j++) {
                q[i][j].~T();
            }
        }
        for (int i = 0; i < len; i++) {
            operator delete[](q[i]);
        }
        delete[] q;
        head = other.head;
        tail = other.tail;
        head_pos = other.head_pos;
        tail_pos = other.tail_pos;
        len = other.len;
        sz = other.sz;
        q = new T *[len];
        for (int i = 0; i < len; i++) {
            q[i] = static_cast<T *>(operator new[](chunk_size * sizeof(T)));
        }
        for (int i = head; i <= tail; i++) {
            int st = (i == head ? head_pos : 0);
            int ed = (i == tail ? tail_pos : chunk_size);
            for (int j = st; j < ed; j++) {
                new (&q[i][j]) T(other.q[i][j]);
            }
        }
        return (*this);
    }
    T &at(const size_t &pos) {
        if (pos < 0 || pos >= sz) {
            throw index_out_of_bound();
        }
        if (head == tail) {
            return q[head][head_pos + pos];
        }
        int head_size = chunk_size - head_pos;
        if (pos < head_size) {
            return q[head][head_pos + pos];
        }
        int tmp = pos - head_size;
        return q[head + 1 + (tmp >> chunk_len)][(tmp & full_chunk_num)];
    }
    const T &at(const size_t &pos) const {
        if (pos < 0 || pos >= sz) {
            throw index_out_of_bound();
        }
        if (head == tail) {
            return q[head][head_pos + pos];
        }
        int head_size = chunk_size - head_pos;
        if (pos < head_size) {
            return q[head][head_pos + pos];
        }
        int tmp = pos - head_size;
        // return q[head + 1 + (tmp >> chunk_len)][(tmp & full_chunk_num)];
        return q[head + 1 + (tmp >> chunk_len)][(tmp & full_chunk_num)];
    }
    T &operator[](const size_t &pos) {
        if (pos < 0 || pos >= sz) {
            throw index_out_of_bound();
        }
        if (head == tail) {
            return q[head][head_pos + pos];
        }
        int head_size = chunk_size - head_pos;
        if (pos < head_size) {
            return q[head][head_pos + pos];
        }
        int tmp = pos - head_size;
        return q[head + 1 + (tmp >> chunk_len)][(tmp & full_chunk_num)];
    }
    const T &operator[](const size_t &pos) const {
        if (pos < 0 || pos >= sz) {
            throw index_out_of_bound();
        }
        if (head == tail) {
            return q[head][head_pos + pos];
        }
        int head_size = chunk_size - head_pos;
        if (pos < head_size) {
            return q[head][head_pos + pos];
        }
        int tmp = pos - head_size;
        return q[head + 1 + (tmp >> chunk_len)][(tmp & full_chunk_num)];
    }
    const T &front() const {
        if (sz == 0) {
            throw container_is_empty();
        }
        return q[head][head_pos];
    }
    const T &back() const {
        if (sz == 0) {
            throw container_is_empty();
        }
        return q[tail][tail_pos - 1];
    }
    iterator begin() {
        iterator ans(q[head] + head_pos, this, head);
        return ans;
    }
    const_iterator cbegin() const {
        const_iterator ans(q[head] + head_pos, this, head);
        return ans;
    }
    iterator end() {
        if (tail_pos == chunk_size) {
            return iterator(q[tail + 1], this, tail + 1);
        }
        iterator ans(q[tail] + tail_pos, this, tail);
        return ans;
    }
    const_iterator cend() const {
        if (tail_pos == chunk_size) {
            return const_iterator(q[tail + 1], this, tail + 1);
        }
        const_iterator ans(q[tail] + tail_pos, this, tail);
        return ans;
    }
    bool empty() const {
        return (sz == 0);
    }
    size_t size() const {
        return sz;
    }
    void clear() {
        for (int i = head; i <= tail; i++) {
            int st = (i == head ? head_pos : 0);
            int ed = (i == tail ? tail_pos : chunk_size);
            for (int j = st; j < ed; j++) {
                q[i][j].~T();
            }
        }
        sz = 0;
        head = tail = 1;
        head_pos = tail_pos = 1;
    }
    iterator insert(iterator pos, const T &value) {
        sz++;
        int ind = pos - begin();
        if (ind * 2 >= sz) {
            tail_pos++;
            if (tail_pos == chunk_size + 1) {
                tail_pos = 1;
                tail++;
            }
            for (int i = sz - 1; i > ind; i--) {
                new (&at(i)) T(at(i - 1));
                at(i - 1).~T();
            }
            new (&at(ind)) T(value);
        } else {
            head_pos--;
            if (head_pos < 0) {
                head--;
                head_pos += chunk_size;
            }
            for (int i = 1; i <= ind; i++) {
                new (&at(i - 1)) T(at(i));
                at(i).~T();
            }
            new (&at(ind)) T(value);
        }
        if (head == 0 && head_pos == 0) {
            DoubleSpace();
        } else if (tail == len - 1 && tail_pos >= chunk_size - 1) {
            DoubleSpace();
        }
        iterator ans(begin() + ind);
        return ans;
    }
    iterator erase(iterator pos) {
        int ind = pos - begin();
        if (ind * 2 >= sz) {
            for (int i = ind; i + 1 < sz; i++) {
                at(i).~T();
                new (&at(i)) T(at(i + 1));
            }
            at(sz - 1).~T();
            tail_pos--;
            if (tail_pos <= 0) {
                tail--;
                tail_pos += chunk_size;
            }
        } else {
            for (int i = ind; i; i--) {
                at(i).~T();
                new (&at(i)) T(at(i - 1));
            }
            at(0).~T();
            head_pos++;
            if (head_pos >= chunk_size) {
                head++;
                head_pos -= chunk_size;
            }
        }
        sz--;
        iterator ans(begin() + ind);
        return ans;
    }
    void push_back(const T &value) {
        tail_pos++;
        if (tail_pos == chunk_size + 1) {
            tail_pos = 1;
            tail++;
        }
        new (&q[tail][tail_pos - 1]) T(value);
        sz++;
        if (tail == len - 1 && tail_pos >= chunk_size - 1) {
            DoubleSpace();
        }
    }
    void pop_back() {
        if (sz == 0) {
            throw container_is_empty();
        }
        q[tail][tail_pos - 1].~T();
        sz--;
        tail_pos--;
        if (tail_pos == 0) {
            tail--;
            tail_pos += chunk_size;
        }
    }
    void push_front(const T &value) {
        head_pos--;
        if (head_pos < 0) {
            head--;
            head_pos += chunk_size;
        }
        new (&q[head][head_pos]) T(value);
        sz++;
        if (head == 0 && head_pos == 0) {
            DoubleSpace();
        }
    }
    void pop_front() {
        if (sz == 0) {
            throw container_is_empty();
        }
        q[head][head_pos].~T();
        sz--;
        head_pos++;
        if (head_pos == chunk_size) {
            head++;
            head_pos -= chunk_size;
        }
    }
};

}  // namespace sjtu

#endif
