#pragma once
#include "vector/src/vector.hpp"

#include <cmath>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

constexpr int MAX_INDEX_LEN = 64;
constexpr int NODE_SIZE = 4096;
constexpr int CACHE_CAPACITY = 6500;

class LRUCache {
private:
  struct Entry {
    int pos;
    char data[NODE_SIZE];
    bool dirty;
    int lru_pre, lru_nex;
    int hash_nex;
  };
  static constexpr int hash_mod = 16381;
  int capacity;
  int size;
  int lru_head, lru_tail;
  int *hash_table;
  Entry *items;
  std::fstream &file;
  sjtu::vector<int> empty_items;
  int get_hash(int pos) { return (pos / NODE_SIZE) % hash_mod; }
  int find_index(int pos) {
    int idx = hash_table[get_hash(pos)];
    while (idx != -1 && items[idx].pos != pos) {
      idx = items[idx].hash_nex;
    }
    return idx;
  }
  void lru_remove(int idx) {
    int pre = items[idx].lru_pre, nex = items[idx].lru_nex;
    if (pre != -1) {
      items[pre].lru_nex = items[idx].lru_nex;
    } else {
      lru_head = items[idx].lru_nex;
    }
    if (nex != -1) {
      items[nex].lru_pre = items[idx].lru_pre;
    } else {
      lru_tail = items[idx].lru_pre;
    }
  }
  void lru_insert_head(int idx) {
    items[idx].lru_pre = -1;
    items[idx].lru_nex = lru_head;
    if (lru_head != -1) {
      items[lru_head].lru_pre = idx;
    }
    lru_head = idx;
    if (lru_tail == -1) {
      lru_tail = idx;
    }
  }
  void lru_move_head(int idx) {
    lru_remove(idx);
    lru_insert_head(idx);
  }
  void lru_delete_last() {
    if (lru_tail == -1) {
      return;
    }
    int idx = lru_tail, pos = items[idx].pos;
    if (items[idx].dirty) {
      file.seekp(pos);
      file.write(items[idx].data, NODE_SIZE);
    }
    int val = get_hash(pos);
    if (items[hash_table[val]].pos == pos) {
      hash_table[val] = items[hash_table[val]].hash_nex;
    } else {
      int ind = hash_table[val];
      while (items[ind].hash_nex != idx) {
        ind = items[ind].hash_nex;
      }
      items[ind].hash_nex = items[idx].hash_nex;
    }
    lru_remove(idx);
    items[idx].dirty = false;
    items[idx].pos = -1;
    size--;
    empty_items.push_back(idx);
  }
  void hash_insert(int idx) {
    int val = get_hash(items[idx].pos);
    items[idx].hash_nex = hash_table[val];
    hash_table[val] = idx;
  }

public:
  LRUCache(int cap, std::fstream &fs) : file(fs), capacity(cap) {
    size = 0;
    items = new Entry[cap];
    lru_head = lru_tail = -1;
    hash_table = new int[hash_mod];
    for (int i = 0; i < hash_mod; i++) {
      hash_table[i] = -1;
    }
    for (int i = 0; i < capacity; i++) {
      items[i].pos = -1;
      empty_items.push_back(i);
    }
  }
  ~LRUCache() {
    for (int i = 0; i < capacity; i++) {
      if (items[i].pos != -1 && items[i].dirty) {
        file.seekp(items[i].pos);
        file.write(items[i].data, NODE_SIZE);
      }
    }
    delete[] items;
    delete[] hash_table;
  }
  bool get(int pos, char *data) {
    int idx = find_index(pos);
    if (idx == -1) {
      return false;
    } else {
      lru_move_head(idx);
      memcpy(data, items[idx].data, NODE_SIZE);
      return true;
    }
  }
  void put(int pos, const char *data, bool dirty = false) {
    int idx = find_index(pos);
    if (idx != -1) {
      memcpy(items[idx].data, data, NODE_SIZE);
      if (dirty) {
        items[idx].dirty = true;
      }
      lru_move_head(idx);
      return;
    }
    if (size == capacity) {
      lru_delete_last();
    }
    idx = empty_items.back();
    empty_items.pop_back();
    items[idx].dirty = dirty;
    items[idx].pos = pos;
    items[idx].hash_nex = -1;
    memcpy(items[idx].data, data, NODE_SIZE);
    hash_insert(idx);
    lru_insert_head(idx);
    size++;
  }
  void remove(int pos) {
    int idx = find_index(pos);
    if (items[idx].dirty) {
      file.seekp(pos);
      file.write(items[idx].data, NODE_SIZE);
    }
    int val = get_hash(pos);
    if (items[hash_table[val]].pos == pos) {
      hash_table[val] = items[hash_table[val]].hash_nex;
    } else {
      int ind = hash_table[val];
      while (items[ind].hash_nex != idx) {
        ind = items[ind].hash_nex;
      }
      items[ind].hash_nex = items[idx].hash_nex;
    }
    lru_remove(idx);
    items[idx].dirty = false;
    items[idx].pos = -1;
    size--;
    empty_items.push_back(idx);
  }
  void clean() {
    empty_items.clear();
    size = 0;
    items = new Entry[capacity];
    lru_head = lru_tail = -1;
    hash_table = new int[hash_mod];
    for (int i = 0; i < hash_mod; i++) {
      hash_table[i] = -1;
    }
    for (int i = 0; i < capacity; i++) {
      items[i].pos = -1;
      empty_items.push_back(i);
    }

  }
  // TODO :
};

template <typename KeyType, typename ValueType> class BPT {
private:
  static constexpr int CalcLeafOrder() {
    return (NODE_SIZE - 32) / (sizeof(KeyType) + sizeof(ValueType)) - 1;
  }
  static constexpr int CalcInternalOrder() {
    return (NODE_SIZE - 32) /
               (sizeof(KeyType) + sizeof(ValueType) + sizeof(int)) -
           1;
  }
  static constexpr int LEAF_ORDER = CalcLeafOrder();
  static constexpr int INTERNAL_ORDER = CalcInternalOrder();
  struct Node {
    bool is_leaf;
    int size;
    int parent_pos;
  };
  struct InternalNode : Node {
    KeyType keys[INTERNAL_ORDER + 1];
    ValueType values[INTERNAL_ORDER + 1];
    int sons[INTERNAL_ORDER + 2];
    InternalNode() : Node{false, 0, -1} {}
  };
  struct LeafNode : Node {
    KeyType keys[LEAF_ORDER + 1];
    ValueType values[LEAF_ORDER + 1];
    int next_leaf;
    LeafNode() : Node{true, 0, -1}, next_leaf(-1) {}
  };
  static const int info_len = 2;
  std::fstream file_;
  std::string file_name;
  std::string filename;
  int root_pos;
  LRUCache cache;
  int get_root_pos() {
    int tmp;
    file_.seekg(0);
    file_.read(reinterpret_cast<char *>(&tmp), sizeof(int));
    return tmp;
  }
  void update_root_pos(int pos) {
    file_.seekp(0);
    file_.write(reinterpret_cast<char *>(&pos), sizeof(int));
  }
  void read_node(int pos, Node &node) {
    char buf[NODE_SIZE];
    if (!cache.get(pos, buf)) {
      file_.seekg(pos);
      file_.read(buf, NODE_SIZE);
      cache.put(pos, buf, false);
    }
    memcpy(&node, buf, sizeof(Node));
  }
  void read_node(int pos, InternalNode &node) {
    char buf[NODE_SIZE];
    if (!cache.get(pos, buf)) {
      file_.seekg(pos);
      file_.read(buf, NODE_SIZE);
      cache.put(pos, buf, false);
    }
    memcpy(&node, buf, sizeof(InternalNode));
  }
  void read_node(int pos, LeafNode &node) {
    char buf[NODE_SIZE];
    if (!cache.get(pos, buf)) {
      file_.seekg(pos);
      file_.read(buf, NODE_SIZE);
      cache.put(pos, buf, false);
    }
    memcpy(&node, buf, sizeof(LeafNode));
  }
  void write_node(int pos, const InternalNode &node) {
    char buf[NODE_SIZE];
    memcpy(buf, &node, sizeof(InternalNode));
    cache.put(pos, buf, true);
  }
  void write_node(int pos, const LeafNode &node) {
    char buf[NODE_SIZE];
    memcpy(buf, &node, sizeof(LeafNode));
    cache.put(pos, buf, true);
  }
  int append_node(const InternalNode &node) {
    char buf[NODE_SIZE];
    memcpy(buf, &node, sizeof(InternalNode));
    file_.seekp(0, std::ios::end);
    int pos = file_.tellp();
    file_.write(buf, NODE_SIZE);
    cache.put(pos, buf);
    return pos;
  }
  int append_node(const LeafNode &node) {
    char buf[NODE_SIZE] = {0};
    memcpy(buf, &node, sizeof(LeafNode));
    file_.seekp(0, std::ios::end);
    int pos = file_.tellp();
    file_.write(buf, NODE_SIZE);
    cache.put(pos, buf);
    return pos;
  }

  int find_leaf(const KeyType &key, const ValueType &value) {
    int pos = root_pos;
    while (true) {
      InternalNode node;
      read_node(pos, node);
      if (node.is_leaf) {
        return pos;
      } else {
        int l = 0, r = node.size - 1, ans = 0;
        while (l <= r) {
          int mid = (l + r) >> 1;
          if (node.keys[mid] < key) {
            ans = mid + 1, l = mid + 1;
          } else if (node.keys[mid] == key && node.values[mid] <= value) {
            ans = mid + 1, l = mid + 1;
          } else {
            r = mid - 1;
          }
        }
        pos = node.sons[ans];
      }
    }
  }
  void split_leaf(int leaf_pos, LeafNode &leaf) {
    int mid = LEAF_ORDER / 2;
    LeafNode new_leaf;
    new_leaf.parent_pos = leaf.parent_pos;
    new_leaf.size = leaf.size - mid;
    new_leaf.next_leaf = leaf.next_leaf;
    memcpy(new_leaf.keys, leaf.keys + mid, new_leaf.size * sizeof(KeyType));
    memcpy(new_leaf.values, leaf.values + mid,
           new_leaf.size * sizeof(ValueType));
    leaf.next_leaf = append_node(new_leaf);
    leaf.size = mid;
    KeyType up_key = new_leaf.keys[0];
    ValueType up_value = new_leaf.values[0];
    if (leaf.parent_pos == -1) {
      InternalNode rt;
      rt.size = 1;
      rt.keys[0] = up_key;
      rt.values[0] = up_value;
      rt.sons[0] = leaf_pos;
      rt.sons[1] = leaf.next_leaf;
      leaf.parent_pos = append_node(rt);
      new_leaf.parent_pos = root_pos = leaf.parent_pos;
      update_root_pos(root_pos);
      write_node(leaf_pos, leaf);
      write_node(leaf.next_leaf, new_leaf);
    } else {
      write_node(leaf_pos, leaf);
      write_node(leaf.next_leaf, new_leaf);
      insert_internal(up_key, up_value, leaf_pos, leaf.next_leaf,
                      leaf.parent_pos);
    }
  }
  void insert_internal(const KeyType &key, const ValueType &value, int pos_l,
                       int pos_r, int node_pos) {
    InternalNode now;
    read_node(node_pos, now);
    int l = 0, r = now.size - 1, p = 0;
    while (l <= r) {
      int mid = (l + r) >> 1;
      if (now.keys[mid] < key) {
        p = mid + 1, l = mid + 1;
      } else if (now.keys[mid] == key && now.values[mid] < value) {
        p = mid + 1, l = mid + 1;
      } else {
        r = mid - 1;
      }
    }
    memmove(now.keys + p + 1, now.keys + p, (now.size - p) * sizeof(KeyType));
    memmove(now.values + p + 1, now.values + p,
            (now.size - p) * sizeof(ValueType));
    memmove(now.sons + p + 2, now.sons + p + 1, (now.size - p) * sizeof(int));
    now.keys[p] = key;
    now.values[p] = value;
    now.sons[p + 1] = pos_r;
    now.size++;
    if (now.size <= INTERNAL_ORDER) {
      write_node(node_pos, now);
    } else {
      split_internal(node_pos, now);
    }
  }
  void split_internal(int node_pos, InternalNode &now) {
    int mid = INTERNAL_ORDER / 2;
    InternalNode new_node;
    new_node.parent_pos = now.parent_pos;
    new_node.size = now.size - mid - 1;
    int cnt = now.size - mid - 1;
    memcpy(new_node.keys, now.keys + mid + 1, cnt * sizeof(KeyType));
    memcpy(new_node.values, now.values + mid + 1, cnt * sizeof(ValueType));
    memcpy(new_node.sons, now.sons + mid + 1, (cnt + 1) * sizeof(int));
    now.size = mid;
    int new_pos = append_node(new_node);
    for (int i = 0; i <= new_node.size; i++) {
      int son_pos = new_node.sons[i];
      Node header;
      read_node(son_pos, header);
      if (header.is_leaf) {
        LeafNode child;
        read_node(son_pos, child);
        child.parent_pos = new_pos;
        write_node(son_pos, child);
      } else {
        InternalNode child;
        read_node(son_pos, child);
        child.parent_pos = new_pos;
        write_node(son_pos, child);
      }
    }
    KeyType up_key = now.keys[mid];
    ValueType up_value = now.values[mid];
    if (now.parent_pos == -1) {
      InternalNode rt;
      rt.size = 1;
      rt.keys[0] = up_key;
      rt.values[0] = up_value;
      rt.sons[0] = node_pos;
      rt.sons[1] = new_pos;
      root_pos = now.parent_pos = new_node.parent_pos = append_node(rt);
      update_root_pos(root_pos);
      write_node(node_pos, now);
      write_node(new_pos, new_node);
    } else {
      write_node(node_pos, now);
      write_node(new_pos, new_node);
      insert_internal(up_key, up_value, node_pos, new_pos, now.parent_pos);
    }
  }
  void update_ancestor_key(int child_pos, int now_pos, const KeyType &key,
                           const ValueType &value) {
    while (now_pos != -1) {
      InternalNode now;
      read_node(now_pos, now);
      int ind = 0;
      for (int i = 0; i <= now.size; i++) {
        if (now.sons[i] == child_pos) {
          ind = i;
          break;
        }
      }
      if (ind == 0) {
        child_pos = now_pos;
        now_pos = now.parent_pos;
      } else {
        now.keys[ind - 1] = key;
        now.values[ind - 1] = value;
        write_node(now_pos, now);
        break;
      }
    }
  }
  void adjust_leaf(int leaf_pos, LeafNode &leaf) {
    if (leaf.parent_pos == -1) {
      write_node(leaf_pos, leaf);
      return;
    }
    InternalNode parent;
    read_node(leaf.parent_pos, parent);
    int ind = 0;
    for (int i = 0; i <= parent.size; i++) {
      if (parent.sons[i] == leaf_pos) {
        ind = i;
        break;
      }
    }
    if (ind > 0) {
      int left_pos = parent.sons[ind - 1];
      LeafNode left;
      read_node(left_pos, left);
      if (left.size > LEAF_ORDER / 2) {
        memmove(leaf.keys + 1, leaf.keys, leaf.size * sizeof(KeyType));
        memmove(leaf.values + 1, leaf.values, leaf.size * sizeof(ValueType));
        leaf.keys[0] = left.keys[left.size - 1];
        leaf.values[0] = left.values[left.size - 1];
        leaf.size++;
        left.size--;
        parent.keys[ind - 1] = leaf.keys[0];
        parent.values[ind - 1] = leaf.values[0];
        write_node(leaf_pos, leaf);
        write_node(left_pos, left);
        write_node(leaf.parent_pos, parent);
        return;
      }
    } else if (ind < parent.size) {
      int right_pos = parent.sons[ind + 1];
      LeafNode right;
      read_node(right_pos, right);
      if (right.size > LEAF_ORDER / 2) {
        leaf.keys[leaf.size] = right.keys[0];
        leaf.values[leaf.size] = right.values[0];
        memmove(right.keys, right.keys + 1, (right.size - 1) * sizeof(KeyType));
        memmove(right.values, right.values + 1,
                (right.size - 1) * sizeof(ValueType));
        leaf.size++;
        right.size--;
        parent.keys[ind] = right.keys[0];
        parent.values[ind] = right.values[0];
        write_node(leaf_pos, leaf);
        write_node(right_pos, right);
        write_node(leaf.parent_pos, parent);
        return;
      }
    }
    if (ind > 0) {
      // TODO :
      int left_pos = parent.sons[ind - 1];
      LeafNode left;
      read_node(left_pos, left);
      memcpy(left.keys + left.size, leaf.keys, leaf.size * sizeof(KeyType));
      memcpy(left.values + left.size, leaf.values,
             leaf.size * sizeof(ValueType));
      left.size += leaf.size;
      left.next_leaf = leaf.next_leaf;
      write_node(left_pos, left);
      int move_cnt = parent.size - ind;
      memmove(parent.sons + ind, parent.sons + ind + 1, move_cnt * sizeof(int));
      memmove(parent.keys + ind - 1, parent.keys + ind,
              move_cnt * sizeof(KeyType));
      memmove(parent.values + ind - 1, parent.values + ind,
              move_cnt * sizeof(ValueType));
      parent.size--;
      write_node(leaf.parent_pos, parent);
      if (parent.size == 0 && parent.parent_pos == -1) {
        // TODO :
        root_pos = left_pos;
        update_root_pos(root_pos);
        left.parent_pos = -1;
        write_node(left_pos, left);
      } else if (parent.size < INTERNAL_ORDER / 2) {
        adjust_internal(leaf.parent_pos, parent);
      }
    } else {
      // TODO :
      int right_pos = parent.sons[ind + 1];
      LeafNode right;
      read_node(right_pos, right);
      memcpy(leaf.keys + leaf.size, right.keys, right.size * sizeof(KeyType));
      memcpy(leaf.values + leaf.size, right.values,
             right.size * sizeof(ValueType));
      leaf.size += right.size;
      leaf.next_leaf = right.next_leaf;
      write_node(leaf_pos, leaf);
      int move_cnt = parent.size - ind - 1;
      memmove(parent.sons + ind + 1, parent.sons + ind + 2,
              move_cnt * sizeof(int));
      memmove(parent.keys + ind, parent.keys + ind + 1,
              move_cnt * sizeof(KeyType));
      memmove(parent.values + ind, parent.values + ind + 1,
              move_cnt * sizeof(ValueType));
      parent.size--;
      write_node(leaf.parent_pos, parent);
      if (parent.size == 0 && parent.parent_pos == -1) {
        // TODO :
        root_pos = leaf_pos;
        update_root_pos(root_pos);
        leaf.parent_pos = -1;
        write_node(leaf_pos, leaf);
      } else if (parent.size < INTERNAL_ORDER / 2) {
        adjust_internal(leaf.parent_pos, parent);
      }
    }
  }
  void adjust_internal(int now_pos, InternalNode &now) {
    if (now.parent_pos == -1) {
      write_node(now_pos, now);
      return;
    }
    InternalNode parent;
    read_node(now.parent_pos, parent);
    int ind = 0;
    for (int i = 0; i <= parent.size; i++) {
      if (parent.sons[i] == now_pos) {
        ind = i;
        break;
      }
    }
    if (ind > 0) {
      int left_pos = parent.sons[ind - 1];
      InternalNode left;
      read_node(left_pos, left);
      if (left.size > INTERNAL_ORDER / 2) {
        memmove(now.keys + 1, now.keys, now.size * sizeof(KeyType));
        memmove(now.values + 1, now.values, now.size * sizeof(ValueType));
        memmove(now.sons + 1, now.sons, (now.size + 1) * sizeof(int));
        now.sons[0] = left.sons[left.size];
        now.keys[0] = parent.keys[ind - 1];
        now.values[0] = parent.values[ind - 1];
        parent.keys[ind - 1] = left.keys[left.size - 1];
        parent.values[ind - 1] = left.values[left.size - 1];
        now.size++;
        left.size--;
        int son_pos = now.sons[0];
        Node header;
        read_node(son_pos, header);
        if (header.is_leaf) {
          LeafNode child;
          read_node(son_pos, child);
          child.parent_pos = now_pos;
          write_node(son_pos, child);
        } else {
          InternalNode child;
          read_node(son_pos, child);
          child.parent_pos = now_pos;
          write_node(son_pos, child);
        }
        write_node(now_pos, now);
        write_node(left_pos, left);
        write_node(now.parent_pos, parent);
        return;
      }
    } else if (ind < parent.size) {
      int right_pos = parent.sons[ind + 1];
      InternalNode right;
      read_node(right_pos, right);
      if (right.size > INTERNAL_ORDER / 2) {
        now.keys[now.size] = parent.keys[ind];
        now.values[now.size] = parent.values[ind];
        now.sons[now.size + 1] = right.sons[0];
        parent.keys[ind] = right.keys[0];
        parent.values[ind] = right.values[0];
        memmove(right.keys, right.keys + 1, (right.size - 1) * sizeof(KeyType));
        memmove(right.values, right.values + 1,
                (right.size - 1) * sizeof(ValueType));
        memmove(right.sons, right.sons + 1, right.size * sizeof(int));
        right.size--;
        now.size++;
        int son_pos = now.sons[now.size];
        Node header;
        read_node(son_pos, header);
        if (header.is_leaf) {
          LeafNode child;
          read_node(son_pos, child);
          child.parent_pos = now_pos;
          write_node(son_pos, child);
        } else {
          InternalNode child;
          read_node(son_pos, child);
          child.parent_pos = now_pos;
          write_node(son_pos, child);
        }
        write_node(now_pos, now);
        write_node(right_pos, right);
        write_node(now.parent_pos, parent);
        return;
      }
    }
    if (ind > 0) {
      // TODO :
      int left_pos = parent.sons[ind - 1];
      InternalNode left;
      read_node(left_pos, left);
      left.keys[left.size] = parent.keys[ind - 1];
      left.values[left.size] = parent.values[ind - 1];
      left.size++;
      memcpy(left.keys + left.size, now.keys, now.size * sizeof(KeyType));
      memcpy(left.values + left.size, now.values, now.size * sizeof(ValueType));
      memcpy(left.sons + left.size, now.sons, now.size * sizeof(int));
      left.sons[left.size + now.size] = now.sons[now.size];
      for (int i = 0; i <= now.size; i++) {
        int son_pos = now.sons[i];
        Node header;
        read_node(son_pos, header);
        if (header.is_leaf) {
          LeafNode child;
          read_node(son_pos, child);
          child.parent_pos = left_pos;
          write_node(son_pos, child);
        } else {
          InternalNode child;
          read_node(son_pos, child);
          child.parent_pos = left_pos;
          write_node(son_pos, child);
        }
      }
      left.size += now.size;
      write_node(left_pos, left);
      int move_cnt = parent.size - ind;
      memmove(parent.sons + ind, parent.sons + ind + 1, move_cnt * sizeof(int));
      memmove(parent.keys + ind - 1, parent.keys + ind,
              move_cnt * sizeof(KeyType));
      memmove(parent.values + ind - 1, parent.values + ind,
              move_cnt * sizeof(ValueType));
      parent.size--;
      write_node(now.parent_pos, parent);
      if (parent.size == 0 && parent.parent_pos == -1) {
        // TODO :
        root_pos = left_pos;
        update_root_pos(root_pos);
        left.parent_pos = -1;
        write_node(left_pos, left);
      } else if (parent.size < INTERNAL_ORDER / 2) {
        adjust_internal(now.parent_pos, parent);
      }
    } else {
      // TODO :
      int right_pos = parent.sons[ind + 1];
      InternalNode right;
      read_node(right_pos, right);
      now.keys[now.size] = parent.keys[ind];
      now.values[now.size] = parent.values[ind];
      now.size++;
      memcpy(now.keys + now.size, right.keys, right.size * sizeof(KeyType));
      memcpy(now.values + now.size, right.values,
             right.size * sizeof(ValueType));
      memcpy(now.sons + now.size, right.sons, right.size * sizeof(int));
      now.sons[now.size + right.size] = right.sons[right.size];
      for (int i = 0; i <= right.size; i++) {
        int son_pos = right.sons[i];
        Node header;
        read_node(son_pos, header);
        if (header.is_leaf) {
          LeafNode child;
          read_node(son_pos, child);
          child.parent_pos = now_pos;
          write_node(son_pos, child);
        } else {
          InternalNode child;
          read_node(son_pos, child);
          child.parent_pos = now_pos;
          write_node(son_pos, child);
        }
      }
      now.size += right.size;
      write_node(now_pos, now);
      int move_cnt = parent.size - ind - 1;
      memmove(parent.sons + ind + 1, parent.sons + ind + 2,
              move_cnt * sizeof(int));
      memmove(parent.keys + ind, parent.keys + ind + 1,
              move_cnt * sizeof(KeyType));
      memmove(parent.values + ind, parent.values + ind + 1,
              move_cnt * sizeof(ValueType));
      parent.size--;
      write_node(now.parent_pos, parent);
      if (parent.size == 0 && parent.parent_pos == -1) {
        // TODO :
        root_pos = now_pos;
        update_root_pos(root_pos);
        now.parent_pos = -1;
        write_node(now_pos, now);
      } else if (parent.size < INTERNAL_ORDER / 2) {
        adjust_internal(now.parent_pos, parent);
      }
    }
  }

public:
  BPT(const std::string name) : cache(CACHE_CAPACITY, file_) {
    file_name = name;
    std::fstream file__(file_name);
    if (!file__.good()) {
      file_.open(file_name, std::ios::app | std::ios::binary);
      int tmp = 0;
      for (int i = 0; i < info_len; ++i) {
        file_.write(reinterpret_cast<char *>(&tmp), sizeof(int));
      }
      file_.close();
    }
    file_.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
    root_pos = get_root_pos();
    if (root_pos == -1 || root_pos == 0) {
      LeafNode rt;
      root_pos = append_node(rt);
      update_root_pos(root_pos);
    }
  }
  ~BPT() = default;
  void insert(const KeyType &key, const ValueType &value) {
    int leaf_pos = find_leaf(key, value);
    LeafNode leaf;
    read_node(leaf_pos, leaf);
    int l = 0, r = leaf.size - 1, pos = 0;
    while (l <= r) {
      int mid = (l + r) >> 1;
      if (leaf.keys[mid] < key) {
        pos = mid + 1, l = mid + 1;
      } else if (leaf.keys[mid] == key && leaf.values[mid] < value) {
        pos = mid + 1, l = mid + 1;
      } else {
        r = mid - 1;
      }
    }
    if (pos < leaf.size && leaf.keys[pos] == key && leaf.values[pos] == value) {
      return;
    }
    int move_cnt = leaf.size - pos;
    memmove(leaf.keys + pos + 1, leaf.keys + pos, move_cnt * sizeof(KeyType));
    memmove(leaf.values + pos + 1, leaf.values + pos,
            move_cnt * sizeof(ValueType));
    leaf.keys[pos] = key;
    leaf.values[pos] = value;
    leaf.size++;
    if (leaf.size <= LEAF_ORDER) {
      write_node(leaf_pos, leaf);
    } else {
      split_leaf(leaf_pos, leaf);
    }
  }
  sjtu::vector<ValueType> find(const KeyType &key, const ValueType &value) {
    sjtu::vector<ValueType> ans;
    int leaf_pos = find_leaf(key, value);
    LeafNode leaf;
    read_node(leaf_pos, leaf);
    int l = 0, r = leaf.size - 1, pos = 0;
    while (l <= r) {
      int mid = (l + r) >> 1;
      if (leaf.keys[mid] < key) {
        pos = mid + 1, l = mid + 1;
      } else if (leaf.keys[mid] == key && leaf.values[mid] < value) {
        pos = mid + 1, l = mid + 1;
      } else {
        r = mid - 1;
      }
    }
    while (pos < leaf.size && key == leaf.keys[pos]) {
      ans.push_back(leaf.values[pos]);
      ++pos;
    }
    while (pos == leaf.size && leaf.next_leaf != -1) {
      read_node(leaf.next_leaf, leaf);
      pos = 0;
      while (pos < leaf.size && key == leaf.keys[pos]) {
        ans.push_back(leaf.values[pos]);
        ++pos;
      }
    }
    return ans;
  }
  void remove(const KeyType &key, const ValueType &value) {
    int leaf_pos = find_leaf(key, value);
    LeafNode leaf;
    read_node(leaf_pos, leaf);
    int l = 0, r = leaf.size - 1, pos = 0;
    while (l <= r) {
      int mid = (l + r) >> 1;
      if (leaf.keys[mid] < key) {
        pos = mid + 1, l = mid + 1;
      } else if (leaf.keys[mid] == key && leaf.values[mid] < value) {
        pos = mid + 1, l = mid + 1;
      } else {
        r = mid - 1;
      }
    }
    if (pos == leaf.size || leaf.keys[pos] > key || leaf.values[pos] != value) {
      return;
    }
    int move_cnt = leaf.size - pos - 1;
    if (move_cnt > 0) {
      memmove(leaf.keys + pos, leaf.keys + pos + 1, move_cnt * sizeof(KeyType));
      memmove(leaf.values + pos, leaf.values + pos + 1,
              move_cnt * sizeof(ValueType));
    }
    leaf.size--;
    if (pos == 0 && leaf.size) {
      update_ancestor_key(leaf_pos, leaf.parent_pos, leaf.keys[0],
                          leaf.values[0]);
    }
    if (leaf.size >= LEAF_ORDER / 2) {
      write_node(leaf_pos, leaf);
    } else {
      adjust_leaf(leaf_pos, leaf);
    }
  }
  void clean() {
    cache.clean();
    file_.close();
    file_.open(file_name, std::ios::in | std::ios::out | std::ios::trunc);
  }
};