#ifndef MY_SORT_HPP
#define MY_SORT_HPP

#include <functional>
#include <utility>

namespace my {

template <typename Container, typename Compare>
void insertion_sort(Container &arr, int left, int right, Compare comp) {
  for (int i = left + 1; i <= right; ++i) {
    auto key = arr[i];
    int j = i - 1;
    while (j >= left && comp(key, arr[j])) {
      arr[j + 1] = arr[j];
      --j;
    }
    arr[j + 1] = key;
  }
}

template <typename Container, typename Compare>
int partition(Container &arr, int left, int right, Compare comp) {
  int mid = left + (right - left) / 2;
  if (comp(arr[mid], arr[left]))
    std::swap(arr[left], arr[mid]);
  if (comp(arr[right], arr[left]))
    std::swap(arr[left], arr[right]);
  if (comp(arr[right], arr[mid]))
    std::swap(arr[mid], arr[right]);
  std::swap(arr[mid], arr[right]);

  auto &pivot = arr[right];
  int i = left - 1;
  for (int j = left; j < right; ++j) {
    if (comp(arr[j], pivot)) {
      ++i;
      std::swap(arr[i], arr[j]);
    }
  }
  std::swap(arr[i + 1], arr[right]);
  return i + 1;
}

template <typename Container, typename Compare>
void quick_sort(Container &arr, int left, int right, Compare comp) {
  const int THRESHOLD = 16;
  if (right - left + 1 <= THRESHOLD) {
    insertion_sort(arr, left, right, comp);
    return;
  }
  if (left < right) {
    int p = partition(arr, left, right, comp);
    quick_sort(arr, left, p - 1, comp);
    quick_sort(arr, p + 1, right, comp);
  }
}

template <typename Container,
          typename Compare = std::less<typename Container::value_type>>
void sort(Container &arr, Compare comp = Compare()) {
  if (arr.size() <= 1)
    return;
  quick_sort(arr, 0, static_cast<int>(arr.size()) - 1, comp);
}

} // namespace my

#endif