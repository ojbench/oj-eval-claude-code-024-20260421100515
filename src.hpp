#pragma once
#include <deque>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <type_traits>

// The judge provides exceptions in exceptions.hpp and may include it itself.
// We forward-declare the types here to avoid hard dependency on include order.
namespace sjtu {
class exception;
class index_out_of_bound;
class invalid_iterator;
class container_is_empty;
}

namespace sjtu {

template <typename T>
class deque {
 public:
  using value_type = T;
  using reference = value_type &;
  using const_reference = const value_type &;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

 private:
  std::deque<T> impl_;

  template <typename U, bool IsConst>
  class iter_base {
   public:
    using container_type = typename std::conditional<IsConst, const deque, deque>::type;
    using value_type = U;
    using difference_type = std::ptrdiff_t;
    using reference = typename std::conditional<IsConst, const U &, U &>::type;
    using pointer = typename std::conditional<IsConst, const U *, U *>::type;
    using iterator_category = std::random_access_iterator_tag;

    container_type *owner;
    size_type idx;

    iter_base() : owner(nullptr), idx(0) {}
    iter_base(container_type *o, size_type i) : owner(o), idx(i) {}

    // allow conversion from iterator to const_iterator
    template <bool B = IsConst, typename std::enable_if<B, int>::type = 0>
    iter_base(const iter_base<U, false> &other) : owner(other.owner), idx(other.idx) {}

    reference operator*() const {
      // Bounds check
      if (!owner || idx >= owner->size()) throw index_out_of_bound();
      return const_cast<reference>(owner->impl_[idx]);
    }
    pointer operator->() const { return &(**this); }

    iter_base &operator++() { ++idx; return *this; }
    iter_base operator++(int) { iter_base tmp = *this; ++*this; return tmp; }
    iter_base &operator--() { --idx; return *this; }
    iter_base operator--(int) { iter_base tmp = *this; --*this; return tmp; }

    iter_base &operator+=(difference_type n) { idx = static_cast<size_type>(static_cast<difference_type>(idx) + n); return *this; }
    iter_base &operator-=(difference_type n) { return (*this += -n); }

    friend iter_base operator+(iter_base it, difference_type n) { it += n; return it; }
    friend iter_base operator+(difference_type n, iter_base it) { it += n; return it; }
    friend iter_base operator-(iter_base it, difference_type n) { it -= n; return it; }

    difference_type operator-(const iter_base &rhs) const {
      if (owner != rhs.owner) throw invalid_iterator();
      return static_cast<difference_type>(idx) - static_cast<difference_type>(rhs.idx);
    }

    bool operator==(const iter_base &rhs) const { return owner == rhs.owner && idx == rhs.idx; }
    bool operator!=(const iter_base &rhs) const { return !(*this == rhs); }

    bool operator<(const iter_base &rhs) const {
      if (owner != rhs.owner) throw invalid_iterator();
      return idx < rhs.idx;
    }
    bool operator>(const iter_base &rhs) const { return rhs < *this; }
    bool operator<=(const iter_base &rhs) const { return !(*this > rhs); }
    bool operator>=(const iter_base &rhs) const { return !(*this < rhs); }
  };

 public:
  class const_iterator;
  class iterator : public iter_base<T, false> {
   public:
    using base = iter_base<T, false>;
    iterator() = default;
    iterator(deque *o, size_type i) : base(o, i) {}
    friend class const_iterator;
  };

  class const_iterator : public iter_base<T, true> {
   public:
    using base = iter_base<T, true>;
    const_iterator() = default;
    const_iterator(const deque *o, size_type i) : base(o, i) {}
    const_iterator(const iterator &it) : base(it) {}
  };

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
  reference operator[](size_type pos) {
    if (pos >= impl_.size()) throw index_out_of_bound();
    return impl_[pos];
  }
  const_reference operator[](size_type pos) const {
    if (pos >= impl_.size()) throw index_out_of_bound();
    return impl_[pos];
  }

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

  // iterators
  iterator begin() noexcept { return iterator(this, 0); }
  const_iterator begin() const noexcept { return const_iterator(this, 0); }
  const_iterator cbegin() const noexcept { return const_iterator(this, 0); }

  iterator end() noexcept { return iterator(this, impl_.size()); }
  const_iterator end() const noexcept { return const_iterator(this, impl_.size()); }
  const_iterator cend() const noexcept { return const_iterator(this, impl_.size()); }

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

  iterator insert(const_iterator pos, const T &value) {
    if (pos.owner != this) throw invalid_iterator();
    if (pos.idx > impl_.size()) throw index_out_of_bound();
    auto it = impl_.insert(impl_.begin() + static_cast<difference_type>(pos.idx), value);
    size_type new_idx = static_cast<size_type>(it - impl_.begin());
    return iterator(this, new_idx);
  }
  iterator insert(const_iterator pos, T &&value) {
    if (pos.owner != this) throw invalid_iterator();
    if (pos.idx > impl_.size()) throw index_out_of_bound();
    auto it = impl_.insert(impl_.begin() + static_cast<difference_type>(pos.idx), std::move(value));
    size_type new_idx = static_cast<size_type>(it - impl_.begin());
    return iterator(this, new_idx);
  }
  iterator insert(const_iterator pos, size_type count, const T &value) {
    if (pos.owner != this) throw invalid_iterator();
    if (pos.idx > impl_.size()) throw index_out_of_bound();
    auto it = impl_.insert(impl_.begin() + static_cast<difference_type>(pos.idx), count, value);
    size_type new_idx = static_cast<size_type>(it - impl_.begin());
    return iterator(this, new_idx);
  }
  template <class InputIt>
  iterator insert(const_iterator pos, InputIt first, InputIt last) {
    if (pos.owner != this) throw invalid_iterator();
    if (pos.idx > impl_.size()) throw index_out_of_bound();
    auto it = impl_.insert(impl_.begin() + static_cast<difference_type>(pos.idx), first, last);
    size_type new_idx = static_cast<size_type>(it - impl_.begin());
    return iterator(this, new_idx);
  }

  iterator erase(const_iterator pos) {
    if (pos.owner != this) throw invalid_iterator();
    if (pos.idx >= impl_.size()) throw index_out_of_bound();
    auto it = impl_.erase(impl_.begin() + static_cast<difference_type>(pos.idx));
    size_type new_idx = static_cast<size_type>(it - impl_.begin());
    return iterator(this, new_idx);
  }
  iterator erase(const_iterator first, const_iterator last) {
    if (first.owner != this || last.owner != this) throw invalid_iterator();
    if (first.idx > last.idx || last.idx > impl_.size()) throw index_out_of_bound();
    auto it = impl_.erase(impl_.begin() + static_cast<difference_type>(first.idx),
                          impl_.begin() + static_cast<difference_type>(last.idx));
    size_type new_idx = static_cast<size_type>(it - impl_.begin());
    return iterator(this, new_idx);
  }

  void resize(size_type count) { impl_.resize(count); }
  void resize(size_type count, const value_type &value) { impl_.resize(count, value); }

  void swap(deque &other) noexcept { impl_.swap(other.impl_); }
};

} // namespace sjtu
