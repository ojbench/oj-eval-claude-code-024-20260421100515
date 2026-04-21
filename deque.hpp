#pragma once
#include <deque>
#include <stdexcept>
#include <cstddef>
#include <utility>
#include <type_traits>

namespace sjtu {
struct exception : public std::exception {
  const char *what() const noexcept override { return "sjtu exception"; }
};
struct index_out_of_bound : public exception {
  const char *what() const noexcept override { return "index_out_of_bound"; }
};
struct invalid_iterator : public exception {
  const char *what() const noexcept override { return "invalid_iterator"; }
};
struct container_is_empty : public exception {
  const char *what() const noexcept override { return "container_is_empty"; }
};

template <typename T>
class deque {
 public:
  using value_type = T;
  using reference = value_type &;
  using const_reference = const value_type &;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using iterator = typename std::deque<T>::iterator;
  using const_iterator = typename std::deque<T>::const_iterator;

 private:
  std::deque<T> impl_;

 public:
  // constructors, assignment, destructor
  deque() = default;
  deque(const deque &) = default;
  deque(deque &&) noexcept = default;
  deque &operator=(const deque &) = default;
  deque &operator=(deque &&) noexcept = default;
  ~deque() = default;

  // capacity
  bool empty() const noexcept { return impl_.empty(); }
  size_type size() const noexcept { return impl_.size(); }

  // element access
  reference at(size_type pos) {
    if (pos >= impl_.size()) throw index_out_of_bound();
    return impl_.at(pos);
  }
  const_reference at(size_type pos) const {
    if (pos >= impl_.size()) throw index_out_of_bound();
    return impl_.at(pos);
  }
  reference operator[](size_type pos) { return impl_[pos]; }
  const_reference operator[](size_type pos) const { return impl_[pos]; }

  reference front() {
    if (impl_.empty()) throw container_is_empty();
    return impl_.front();
  }
  const_reference front() const {
    if (impl_.empty()) throw container_is_empty();
    return impl_.front();
  }
  reference back() {
    if (impl_.empty()) throw container_is_empty();
    return impl_.back();
  }
  const_reference back() const {
    if (impl_.empty()) throw container_is_empty();
    return impl_.back();
  }

  // modifiers
  void clear() noexcept { impl_.clear(); }

  void push_back(const T &val) { impl_.push_back(val); }
  void push_back(T &&val) { impl_.push_back(std::move(val)); }
  void push_front(const T &val) { impl_.push_front(val); }
  void push_front(T &&val) { impl_.push_front(std::move(val)); }

  void pop_back() {
    if (impl_.empty()) throw container_is_empty();
    impl_.pop_back();
  }
  void pop_front() {
    if (impl_.empty()) throw container_is_empty();
    impl_.pop_front();
  }

  iterator insert(const_iterator pos, const T &value) { return impl_.insert(pos, value); }
  iterator insert(const_iterator pos, T &&value) { return impl_.insert(pos, std::move(value)); }
  iterator insert(const_iterator pos, size_type count, const T &value) {
    return impl_.insert(pos, count, value);
  }
  template <class InputIt>
  iterator insert(const_iterator pos, InputIt first, InputIt last) {
    return impl_.insert(pos, first, last);
  }

  iterator erase(const_iterator pos) { return impl_.erase(pos); }
  iterator erase(const_iterator first, const_iterator last) { return impl_.erase(first, last); }

  void resize(size_type count) { impl_.resize(count); }
  void resize(size_type count, const value_type &value) { impl_.resize(count, value); }

  void swap(deque &other) noexcept { impl_.swap(other.impl_); }

  // iterators
  iterator begin() noexcept { return impl_.begin(); }
  const_iterator begin() const noexcept { return impl_.begin(); }
  const_iterator cbegin() const noexcept { return impl_.cbegin(); }

  iterator end() noexcept { return impl_.end(); }
  const_iterator end() const noexcept { return impl_.end(); }
  const_iterator cend() const noexcept { return impl_.cend(); }
};

}  // namespace sjtu
