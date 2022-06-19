#pragma once
#include <cassert>
#include <iterator>

template <typename T>
class list {
public:
  // bidirectional iterator
  using iterator = int;
  // bidirectional iterator
  using const_iterator = int;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  // O(1)
  list() noexcept;

  // O(n), strong
  list(list const&);

  // O(n), strong
  list& operator=(list const&);

  // O(n)
  ~list();

  // O(1)
  bool empty() const noexcept;

  // O(1)
  T& front() noexcept;
  // O(1)
  T const& front() const noexcept;

  // O(1), strong
  void push_front(T const&);
  // O(1)
  void pop_front() noexcept;

  // O(1)
  T& back() noexcept;
  // O(1)
  T const& back() const noexcept;

  // O(1), strong
  void push_back(T const&);
  // O(1)
  void pop_back() noexcept;

  // O(1)
  iterator begin() noexcept;
  // O(1)
  const_iterator begin() const noexcept;

  // O(1)
  iterator end() noexcept;
  // O(1)
  const_iterator end() const noexcept;

  // O(1)
  reverse_iterator rbegin() noexcept;
  // O(1)
  const_reverse_iterator rbegin() const noexcept;

  // O(1)
  reverse_iterator rend() noexcept;
  // O(1)
  const_reverse_iterator rend() const noexcept;

  // O(n)
  void clear() noexcept;

  // O(1), strong
  iterator insert(const_iterator pos, T const& val);
  // O(1)
  iterator erase(const_iterator pos) noexcept;
  // O(n)
  iterator erase(const_iterator first, const_iterator last) noexcept;
  // O(1)
  void splice(const_iterator pos, list& other, const_iterator first,
              const_iterator last) noexcept;

  friend void swap(list& a, list& b) noexcept;
};
