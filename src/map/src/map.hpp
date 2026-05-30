/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <cfloat>
#include <cstddef>
#include <functional>

#include "exceptions.hpp"
#include "utility.hpp"

namespace sjtu {

template <class Key, class T, class Compare = std::less<Key> >
class map {
   public:
    typedef pair<const Key, T> value_type;

   private:
    enum Color { RED = 0, BLACK = 1 };
    struct Node {
        value_type data;
        Node *fa;
        Node *l;
        Node *r;
        Color col;
        Node(value_type data)
            : data(data), fa(nullptr), l(nullptr), r(nullptr), col(Color::RED) {
        }
        Node *clone() {
            Node *pos = new Node(data);
            pos->col = col;
            if (l) {
                pos->l = l->clone();
                pos->l->fa = pos;
            }
            if (r) {
                pos->r = r->clone();
                pos->r->fa = pos;
            }
            return pos;
        }
        void clear() {
            if (l) {
                l->clear();
            }
            if (r) {
                r->clear();
            }
            delete this;
        }
        void switchcolor() {
            if (col == Color::RED) {
                col = Color::BLACK;
            } else {
                col = Color::RED;
            }
        }
    };
    int sz;
    Node *rt;
    Compare comp;
    void LL(Node *pos) {
        Node *tmp = pos->l;
        pos->l = tmp->r;
        if (tmp->r != nullptr) {
            tmp->r->fa = pos;
        }
        tmp->fa = pos->fa;
        if (pos->fa == nullptr) {
            rt = tmp;
        } else if (pos == pos->fa->l) {
            tmp->fa->l = tmp;
        } else {
            tmp->fa->r = tmp;
        }
        tmp->r = pos;
        pos->fa = tmp;
    }
    void RR(Node *pos) {
        Node *tmp = pos->r;
        pos->r = tmp->l;
        if (tmp->l != nullptr) {
            tmp->l->fa = pos;
        }
        tmp->fa = pos->fa;
        if (pos->fa == nullptr) {
            rt = tmp;
        } else if (pos == pos->fa->l) {
            tmp->fa->l = tmp;
        } else {
            tmp->fa->r = tmp;
        }
        tmp->l = pos;
        pos->fa = tmp;
    }
    void LR(Node *pos) {
        RR(pos->l);
        LL(pos);
    }
    void RL(Node *pos) {
        LL(pos->r);
        RR(pos);
    }
    void InsertAdjust(Node *gfa, Node *fa, Node *pos) {
        if (fa->col == Color::BLACK) {
            return;
        } else if (fa == rt) {
            rt->col = Color::BLACK;
            return;
        } else if (gfa->l == fa && fa->l == pos) {
            LL(gfa);
            gfa->switchcolor();
            fa->switchcolor();
        } else if (gfa->r == fa && fa->r == pos) {
            RR(gfa);
            gfa->switchcolor();
            fa->switchcolor();
        } else if (gfa->l == fa && fa->r == pos) {
            LR(gfa);
            pos->switchcolor();
            gfa->switchcolor();
        } else {
            RL(gfa);
            pos->switchcolor();
            gfa->switchcolor();
        }
    }
    void EraseAdjust(Node *&fa, Node *&now, Node *&t, const Key &key) {
        if (now->col == Color::RED) {
            return;
        }
        if (now == rt && now->l != nullptr && now->r != nullptr &&
            now->r->col == now->l->col) {
            now->col = Color::RED;
            now->l->col = Color::BLACK;
            now->r->col = Color::BLACK;
            return;
        }
        int cnt_now = 0;
        cnt_now += (now->l == nullptr || now->l->col == Color::BLACK);
        cnt_now += (now->r == nullptr || now->r->col == Color::BLACK);
        // Case 1: now with two black sons
        if (cnt_now == 2) {
            if (t == nullptr) {
                now->switchcolor();
                fa->switchcolor();
                return;
            }
            int cnt_t = 0;
            cnt_t += (t->l == nullptr || t->l->col == Color::BLACK);
            cnt_t += (t->r == nullptr || t->r->col == Color::BLACK);
            if (cnt_t == 2) {
                now->switchcolor();
                fa->switchcolor();
                t->switchcolor();
            } else if (fa->r == t && t->r != nullptr &&
                       t->r->col == Color::RED) {
                RR(fa);
                now->switchcolor();
                fa->switchcolor();
                t->switchcolor();
                t->r->switchcolor();
            } else if (fa->l == t && t->l != nullptr &&
                       t->l->col == Color::RED) {
                LL(fa);
                now->switchcolor();
                fa->switchcolor();
                t->switchcolor();
                t->l->switchcolor();
            } else if (fa->r == t) {
                RL(fa);
                now->switchcolor();
                fa->switchcolor();
            } else {
                LR(fa);
                now->switchcolor();
                fa->switchcolor();
            }
        } else if (!comp(now->data.first, key) && !comp(key, now->data.first)) {
            // Case 2: now is the node to be deleted
            if (now->l != nullptr && now->r != nullptr) {
                if (now->l->col == Color::RED && now->r->col == Color::BLACK) {
                    now->switchcolor();
                    now->l->switchcolor();
                    LL(now);
                }
            } else if (now->l != nullptr) {
                now->switchcolor();
                now->l->switchcolor();
                LL(now);
            } else if (now->r != nullptr) {
                now->switchcolor();
                now->r->switchcolor();
                RR(now);
            }
        } else {  // Case3: now is not the node to be deleted
            fa = now;
            now = comp(key, now->data.first) ? now->l : now->r;
            t = (now == fa->l) ? fa->r : fa->l;
            if (now->col == Color::BLACK) {
                (fa->l == now) ? RR(fa) : LL(fa);
                fa->switchcolor();
                if (t != nullptr) {
                    t->switchcolor();
                }
                t = (now == fa->l) ? fa->r : fa->l;
                EraseAdjust(fa, now, t, key);
            }
        }
    }
    void swapNodes(Node *a, Node *b) {
        if (a == b) return;
        bool a_is_fa_of_b = (a->l == b || a->r == b);
        bool b_is_fa_of_a = (b->l == a || b->r == a);
        Node *a_fa = a->fa, *a_l = a->l, *a_r = a->r;
        Node *b_fa = b->fa, *b_l = b->l, *b_r = b->r;
        Color a_col = a->col, b_col = b->col;
        if (a_is_fa_of_b) {
            bool b_is_l = (a->l == b);
            b->fa = a_fa;
            if (a_fa == nullptr) {
                rt = b;
            } else if (a_fa->l == a) {
                a_fa->l = b;
            } else {
                a_fa->r = b;
            }
            if (b_l != nullptr) {
                b_l->fa = a;
            }
            if (b_r != nullptr) {
                b_r->fa = a;
            }
            a->l = b_l;
            a->r = b_r;
            a->fa = b;
            if (b_is_l) {
                b->l = a;
                b->r = a_r;
            } else {
                b->r = a;
                b->l = a_l;
            }
            if (b->r != nullptr) {
                b->r->fa = b;
            }
            if (b->l != nullptr) {
                b->l->fa = b;
            }
        } else if (b_is_fa_of_a) {
            swapNodes(b, a);
            return;
        } else {
            bool a_is_l = (a->fa != nullptr && a->fa->l == a);
            bool b_is_l = (b->fa != nullptr && b->fa->l == b);
            if (a_fa == nullptr) {
                rt = b;
            } else if (a_is_l) {
                a_fa->l = b;
            } else {
                a_fa->r = b;
            }
            if (b_fa == nullptr) {
                rt = a;
            } else if (b_is_l) {
                b_fa->l = a;
            } else {
                b_fa->r = a;
            }
            a->l = b_l;
            a->r = b_r;
            b->l = a_l;
            b->r = a_r;
            if (a->l != nullptr) {
                a->l->fa = a;
            }
            if (a->r != nullptr) {
                a->r->fa = a;
            }
            if (b->l != nullptr) {
                b->l->fa = b;
            }
            if (b->r != nullptr) {
                b->r->fa = b;
            }
            a->fa = b_fa;
            b->fa = a_fa;
        }
        a->col = b_col;
        b->col = a_col;
    }

   public:
    class const_iterator;
    class iterator {
        friend class map<Key, T, Compare>;
        friend class const_iterator;

       private:
        const map<Key, T, Compare> *container;
        Node *pos;

       public:
        iterator() : container(nullptr), pos(nullptr) {
        }
        iterator(const map<Key, T, Compare> *container, Node *pos)
            : container(container), pos(pos) {
        }
        iterator(const iterator &other) {
            container = other.container;
            pos = other.pos;
        }
        iterator operator++(int) {
            iterator ans(*this);
            if (pos == nullptr) {
                throw invalid_iterator();
            } else if (pos->r != nullptr) {
                pos = pos->r;
                while (pos->l != nullptr) {
                    pos = pos->l;
                }
            } else {
                Node *fa = pos->fa;
                while (fa != nullptr && fa->r == pos) {
                    pos = fa;
                    fa = pos->fa;
                }
                pos = fa;
            }
            return ans;
        }
        iterator &operator++() {
            if (pos == nullptr) {
                throw invalid_iterator();
            } else if (pos->r != nullptr) {
                pos = pos->r;
                while (pos->l != nullptr) {
                    pos = pos->l;
                }
            } else {
                Node *fa = pos->fa;
                while (fa != nullptr && fa->r == pos) {
                    pos = fa;
                    fa = pos->fa;
                }
                pos = fa;
            }
            return (*this);
        }
        iterator operator--(int) {
            iterator ans(*this);
            if (pos == nullptr) {
                if (container->rt == nullptr) {
                    throw invalid_iterator();
                }
                pos = container->rt;
                while (pos->r != nullptr) {
                    pos = pos->r;
                }
            } else if (pos->l != nullptr) {
                pos = pos->l;
                while (pos->r != nullptr) {
                    pos = pos->r;
                }
            } else {
                Node *fa = pos->fa;
                while (fa != nullptr && fa->l == pos) {
                    pos = fa;
                    fa = pos->fa;
                }
                pos = fa;
            }
            return ans;
        }
        iterator &operator--() {
            if (pos == nullptr) {
                if (container->rt == nullptr) {
                    throw invalid_iterator();
                }
                pos = container->rt;
                while (pos->r != nullptr) {
                    pos = pos->r;
                }
            } else if (pos->l != nullptr) {
                pos = pos->l;
                while (pos->r != nullptr) {
                    pos = pos->r;
                }
            } else {
                Node *fa = pos->fa;
                while (fa != nullptr && fa->l == pos) {
                    pos = fa;
                    fa = pos->fa;
                }
                pos = fa;
            }
            return (*this);
        }
        bool operator==(const iterator &rhs) const {
            return (pos == rhs.pos && container == rhs.container);
        }
        bool operator==(const const_iterator &rhs) const {
            return (pos == rhs.pos && container == rhs.container);
        }
        bool operator!=(const iterator &rhs) const {
            return !((*this) == rhs);
        }
        bool operator!=(const const_iterator &rhs) const {
            return !((*this) == rhs);
        }
        value_type &operator*() const {
            if (pos == nullptr) {
                throw invalid_iterator();
            }
            return pos->data;
        }
        value_type *operator->() const noexcept {
            return &(pos->data);
        }
    };
    class const_iterator {
        friend class map<Key, T, Compare>;
        friend class iterator;

       private:
        const map<Key, T, Compare> *container;
        Node *pos;

       public:
        const_iterator() : container(nullptr), pos(nullptr) {
        }
        const_iterator(const map<Key, T, Compare> *container, Node *pos)
            : container(container), pos(pos) {
        }
        const_iterator(const const_iterator &other) {
            container = other.container;
            pos = other.pos;
        }
        const_iterator(const iterator &other) {
            container = other.container;
            pos = other.pos;
        }
        const_iterator operator++(int) {
            const_iterator ans(*this);
            if (pos == nullptr) {
                throw invalid_iterator();
            }
            if (pos->r != nullptr) {
                pos = pos->r;
                while (pos->l != nullptr) {
                    pos = pos->l;
                }
            } else {
                Node *fa = pos->fa;
                while (fa != nullptr && fa->r == pos) {
                    pos = fa;
                    fa = pos->fa;
                }
                pos = fa;
            }
            return ans;
        }
        const_iterator &operator++() {
            if (pos == nullptr) {
                throw invalid_iterator();
            }
            if (pos->r != nullptr) {
                pos = pos->r;
                while (pos->l != nullptr) {
                    pos = pos->l;
                }
            } else {
                Node *fa = pos->fa;
                while (fa != nullptr && fa->r == pos) {
                    pos = fa;
                    fa = pos->fa;
                }
                pos = fa;
            }
            return (*this);
        }
        const_iterator operator--(int) {
            const_iterator ans(*this);
            if (pos == nullptr) {
                if (container->rt == nullptr) {
                    throw invalid_iterator();
                }
                pos = container->rt;
                while (pos->r != nullptr) {
                    pos = pos->r;
                }
            } else if (pos->l != nullptr) {
                pos = pos->l;
                while (pos->r != nullptr) {
                    pos = pos->r;
                }
            } else {
                Node *fa = pos->fa;
                while (fa != nullptr && fa->l == pos) {
                    pos = fa;
                    fa = pos->fa;
                }
                pos = fa;
            }
            return ans;
        }
        const_iterator &operator--() {
            if (pos == nullptr) {
                if (container->rt == nullptr) {
                    throw invalid_iterator();
                }
                pos = container->rt;
                while (pos->r != nullptr) {
                    pos = pos->r;
                }
            } else if (pos->l != nullptr) {
                pos = pos->l;
                while (pos->r != nullptr) {
                    pos = pos->r;
                }
            } else {
                Node *fa = pos->fa;
                while (fa != nullptr && fa->l == pos) {
                    pos = fa;
                    fa = pos->fa;
                }
                pos = fa;
            }
            return (*this);
        }
        bool operator==(const iterator &rhs) const {
            return (pos == rhs.pos && container == rhs.container);
        }
        bool operator==(const const_iterator &rhs) const {
            return (pos == rhs.pos && container == rhs.container);
        }
        bool operator!=(const iterator &rhs) const {
            return !((*this) == rhs);
        }
        bool operator!=(const const_iterator &rhs) const {
            return !((*this) == rhs);
        }
        const value_type &operator*() const {
            if (pos == nullptr) {
                throw invalid_iterator();
            }
            return pos->data;
        }
        const value_type *operator->() const noexcept {
            return &(pos->data);
        }
    };
    map() : sz(0), rt(nullptr) {
    }
    map(const map &other) : rt(nullptr), sz(other.sz) {
        if (other.rt != nullptr) {
            rt = other.rt->clone();
        }
    }
    map &operator=(const map &other) {
        if (this == &other) {
            return (*this);
        }
        if (rt != nullptr) {
            rt->clear();
        }
        sz = other.sz;
        rt = nullptr;
        if (other.rt != nullptr) {
            rt = other.rt->clone();
        }
        return (*this);
    }
    ~map() {
        if (rt != nullptr) {
            rt->clear();
        }
    }
    T &at(const Key &key) {
        iterator ans = find(key);
        if (ans == end()) {
            throw index_out_of_bound();
        }
        return ans->second;
    }
    const T &at(const Key &key) const {
        const_iterator ans = find(key);
        if (ans == cend()) {
            throw index_out_of_bound();
        }
        return ans->second;
    }
    T &operator[](const Key &key) {
        iterator ans = find(key);
        if (ans == end()) {
            ans = insert({key, T()}).first;
        }
        return ans->second;
    }
    const T &operator[](const Key &key) const {
        const_iterator ans = find(key);
        if (ans == cend()) {
            throw index_out_of_bound();
        }
        return ans->second;
    }
    iterator begin() {
        if (rt == nullptr) {
            return iterator(this, rt);
        }
        Node *pos = rt;
        while (pos->l != nullptr) {
            pos = pos->l;
        }
        return iterator(this, pos);
    }
    const_iterator cbegin() const {
        if (rt == nullptr) {
            return const_iterator(this, rt);
        }
        Node *pos = rt;
        while (pos->l != nullptr) {
            pos = pos->l;
        }
        return const_iterator(this, pos);
    }
    iterator end() {
        return iterator(this, nullptr);
    }
    const_iterator cend() const {
        return const_iterator(this, nullptr);
    }
    bool empty() const {
        return (sz == 0);
    }
    size_t size() const {
        return sz;
    }
    void clear() {
        if (rt != nullptr) {
            rt->clear();
        }
        rt = nullptr;
        sz = 0;
    }
    pair<iterator, bool> insert(const value_type &value) {
        if (rt == nullptr) {
            sz++;
            rt = new Node(value);
            iterator ans(this, rt);
            return pair<iterator, bool>(ans, true);
        }
        Node *pos = rt, *fa = rt, *gfa = rt;
        while (pos != nullptr) {
            if (pos->l != nullptr && pos->l->col == Color::RED &&
                pos->r != nullptr && pos->r->col == Color::RED) {
                pos->l->col = Color::BLACK;
                pos->r->col = Color::BLACK;
                pos->col = Color::RED;
                InsertAdjust(gfa, fa, pos);
            }
            if (comp(pos->data.first, value.first)) {
                gfa = fa;
                fa = pos;
                pos = pos->r;
            } else if (comp(value.first, pos->data.first)) {
                gfa = fa;
                fa = pos;
                pos = pos->l;
            } else {
                iterator ans(this, pos);
                return pair<iterator, bool>(ans, false);
            }
        }
        pos = new Node(value);
        pos->fa = fa;
        if (comp(value.first, fa->data.first)) {
            fa->l = pos;
        } else {
            fa->r = pos;
        }
        iterator ans(this, pos);
        InsertAdjust(gfa, fa, pos);
        rt->col = Color::BLACK;
        sz++;
        return pair<iterator, bool>(ans, true);
    }
    void erase(iterator pos) {
        if (pos == end() || pos.container != this) {
            throw invalid_iterator();
        }
        if (rt == pos.pos && rt->l == nullptr && rt->r == nullptr) {
            delete rt;
            rt = nullptr;
            sz--;
            return;
        }
        const Key key = pos.pos->data.first;
        Node *now = rt, *fa = rt, *t = rt;
        while (true) {
            // make now be a RED node
            EraseAdjust(fa, now, t, key);
            if (now == rt) {
                fa = rt;
            } else {
                fa = now->fa;
            }
            if (!comp(now->data.first, key) && !comp(key, now->data.first) &&
                now->l != nullptr && now->r != nullptr) {
                Node *tmp = now->r;
                while (tmp->l != nullptr) {
                    tmp = tmp->l;
                }
                swapNodes(now, tmp);
                fa = tmp;
                now = fa->r;
                t = fa->l;
                continue;
            }
            if (!comp(now->data.first, key) && !comp(key, now->data.first)) {
                if (fa->l == now) {
                    fa->l = nullptr;
                } else {
                    fa->r = nullptr;
                }
                delete now;
                rt->col = Color::BLACK;
                sz--;
                return;
            }
            now = comp(now->data.first, key) ? now->r : now->l;
            fa = now->fa;
            t = (fa->l == now) ? fa->r : fa->l;
        }
    }
    size_t count(const Key &key) const {
        return (find(key) == cend() ? 0 : 1);
    }
    iterator find(const Key &key) {
        Node *pos = rt;
        while (pos != nullptr) {
            if (comp(key, pos->data.first)) {
                pos = pos->l;
            } else if (comp(pos->data.first, key)) {
                pos = pos->r;
            } else {
                return iterator(this, pos);
            }
        }
        return end();
    }
    const_iterator find(const Key &key) const {
        Node *pos = rt;
        while (pos != nullptr) {
            if (comp(key, pos->data.first)) {
                pos = pos->l;
            } else if (comp(pos->data.first, key)) {
                pos = pos->r;
            } else {
                return const_iterator(this, pos);
            }
        }
        return cend();
    }
    void clean() {
        if (rt != nullptr) {
            rt->clear();
        }
    }
};

}  // namespace sjtu

#endif
