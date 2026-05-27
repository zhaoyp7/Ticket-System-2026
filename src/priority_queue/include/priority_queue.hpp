#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cmath>       // in case you need it
#include <cstddef>     // for size_t
#include <functional>  // for std::less

#include "exceptions.hpp"

namespace sjtu {

/**
 * @brief A container automatically sorting its contents, similar to
 * std::priority_queue but with extra functionalities.
 *
 * The extra functionalities are:
 * - Merge two priority queues into one (with good time complexity).
 * - Clear all elements in the queue.
 * - Limited exception safety for some operations (e.g. push, pop, top, merge)
 * when the comparator throws exceptions from `Compare` only.
 *
 * This @priority_queue does not support passing an underlying container as a
 * template parameter. Also, it does not support passing a comparator object as
 * a constructor argument.
 *
 */
template <class T, class Compare = std::less<T>>
class priority_queue {
   private:
    struct Node {
        T data;
        Node* son;
        Node* sibling;
        int deg;
        Node(T value) : son(nullptr), sibling(nullptr), deg(0), data(value) {
        }
        Node* clone() {
            Node* pos = new Node(data);
            pos->deg = deg;
            if (son) {
                pos->son = son->clone();
            }
            if (sibling) {
                pos->sibling = sibling->clone();
            }
            return pos;
        }
        void clear() {
            if (son) {
                son->clear();
            }
            if (sibling) {
                sibling->clear();
            }
            delete this;
        }
    };
    Node *head, *max_pos, *max_pre;
    int sz;
    Compare comp;
    void TryMergeList(Node* a, Node* b, bool flag = 0) {
        if (a == nullptr || b == nullptr) {
            return;
        }
        if (a->deg > b->deg) {
            std::swap(a, b);
        }
        int now_deg = a->deg;
        T now_data = a->data;
        a = a->sibling;
        while (a != nullptr && b != nullptr) {
            if (a->deg > b->deg) {
                std::swap(a, b);
            }
            if (a->deg == b->deg) {
                now_data = comp(a->data, b->data) ? b->data : a->data;
                now_deg = a->deg + 1;
                a = a->sibling;
                b = b->sibling;
            } else if (now_deg == a->deg) {
                if (comp(now_data, a->data)) {
                    now_data = a->data;
                }
                now_deg++;
                a = a->sibling;
            } else if (now_deg < a->deg) {
                now_deg = a->deg;
                now_data = a->data;
                a = a->sibling;
            }
        }
        if (a == nullptr) {
            a = b;
        }
        while (a != nullptr) {
            if (now_deg < a->deg) {
                if (flag) {
                    comp(now_data, a->data);
                    now_deg = a->deg;
                    now_data = a->data;
                } else {
                    return;
                }
            } else if (now_deg == a->deg) {
                if (comp(now_data, a->data)) {
                    now_data = a->data;
                }
                now_deg++;
                a = a->sibling;
            }
        }
    }
    Node* MergeTree(Node* a, Node* b) {
        if (a == nullptr) {
            return b;
        } else if (b == nullptr) {
            return a;
        }
        if (comp(a->data, b->data)) {
            std::swap(a, b);
        }
        b->sibling = a->son;
        a->son = b;
        a->deg++;
        return a;
    }
    Node* MergeList(Node* a, Node* b) {
        if (a == nullptr) {
            return b;
        } else if (b == nullptr) {
            return a;
        }
        if (a->deg > b->deg) {
            std::swap(a, b);
        }
        Node *head = a, *now = a, *pre = nullptr;
        a = a->sibling;
        now->sibling = nullptr;
        while (a != nullptr && b != nullptr) {
            if (a->deg > b->deg) {
                std::swap(a, b);
            }
            if (a->deg == b->deg) {
                Node *nexa = a->sibling, *nexb = b->sibling;
                a->sibling = b->sibling = nullptr;
                now->sibling = MergeTree(a, b);
                pre = now;
                now = now->sibling;
                a = nexa;
                b = nexb;
            } else if (now->deg == a->deg) {
                Node* nex = a->sibling;
                a->sibling = nullptr;
                if (head == now) {
                    head = now = MergeTree(now, a);
                } else {
                    now = MergeTree(now, a);
                    pre->sibling = now;
                }
                now->sibling = nullptr;
                a = nex;
            } else if (now->deg < a->deg) {
                now->sibling = a;
                pre = now;
                a = a->sibling;
                now = now->sibling;
                now->sibling = nullptr;
            }
        }
        if (a == nullptr) {
            a = b;
        }
        while (a != nullptr) {
            if (now->deg < a->deg) {
                now->sibling = a;
                return head;
            } else if (now->deg == a->deg) {
                Node* nex = a->sibling;
                a->sibling = nullptr;
                if (head == now) {
                    head = now = MergeTree(now, a);
                } else {
                    now = MergeTree(now, a);
                    pre->sibling = now;
                }
                now->sibling = nullptr;
                a = nex;
            }
        }
        return head;
    }
    void UpdateTop() {
        max_pos = max_pre = nullptr;
        if (head == nullptr) {
            return;
        }
        Node* pos = head;
        max_pos = head;
        while (pos->sibling) {
            if (comp(max_pos->data, pos->sibling->data)) {
                max_pos = pos->sibling;
                max_pre = pos;
            }
            pos = pos->sibling;
        }
    }

   public:
    priority_queue()
        : head(nullptr), sz(0), max_pos(nullptr), max_pre(nullptr) {
    }
    priority_queue(const priority_queue& other) : head(nullptr) {
        sz = other.sz;
        if (other.head != nullptr) {
            head = other.head->clone();
        }
        UpdateTop();
    }
    ~priority_queue() {
        if (head != nullptr) {
            head->clear();
        }
    }
    priority_queue& operator=(const priority_queue& other) {
        if (this == &other) return (*this);
        if (head != nullptr) {
            head->clear();
        }
        sz = other.sz;
        head = nullptr;
        if (other.head != nullptr) {
            head = other.head->clone();
        }
        UpdateTop();
        return (*this);
    }
    void push(const T& value) {
        Node* pos = new Node(value);
        try {
            TryMergeList(head, pos, 1);
            head = MergeList(head, pos);
            sz++;
            UpdateTop();
        } catch (...) {
            delete pos;
            throw;
        }
    }
    const T& top() const {
        if (sz == 0) {
            throw container_is_empty();
        }
        return max_pos->data;
    }
    void pop() {
        if (sz == 0) {
            throw container_is_empty();
        }
        Node* cur = max_pos;
        Node *other = cur->son, *pre = nullptr;
        if (other == nullptr) {
            sz--;
            if (head == cur) {
                head = cur->sibling;
            } else {
                max_pre->sibling = max_pre->sibling->sibling;
            }
            delete cur;
            UpdateTop();
            return;
        }
        try {
            while (other->sibling) {
                Node* next = other->sibling;
                other->sibling = pre;
                pre = other;
                other = next;
            }
            other->sibling = pre;
            sz--;
            if (head == cur) {
                head = cur->sibling;
            } else {
                max_pre->sibling = max_pre->sibling->sibling;
            }
            TryMergeList(head, other, 1);
            head = MergeList(head, other);
            delete cur;
            UpdateTop();
        } catch (...) {
            if (max_pre == nullptr) {
                head = cur;
            } else {
                max_pre->sibling = cur;
            }
            sz++;
            pre = nullptr;
            while (other->sibling) {
                Node* next = other->sibling;
                other->sibling = pre;
                pre = other;
                other = next;
            }
            other->sibling = pre;
            throw;
        }
    }
    size_t size() const {
        return sz;
    }
    bool empty() const {
        return (sz == 0);
    }
    void clear() {
        if (head != nullptr) {
            head->clear();
        }
        head = nullptr;
        max_pos = nullptr;
        max_pre = nullptr;
        sz = 0;
    }
    void merge(priority_queue& other) {
        if (this == &other) {
            return;
        }
        TryMergeList(head, other.head, 1);
        sz += other.sz;
        head = MergeList(head, other.head);
        other.head = nullptr;
        other.sz = 0;
        UpdateTop();
    }
};

}  // namespace sjtu

#endif