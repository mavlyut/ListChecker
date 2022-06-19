#pragma once
#include <cassert>
#include <iterator>
#include <iostream>

template <typename T>
class list {

private:
  struct basenode {
    basenode* prev;
    basenode* next;

    basenode() = default;
    basenode(basenode* left, basenode* right) : prev(left), next(right) {}
    virtual ~basenode() = default;
  };

  struct node : basenode {
    T val;

    node() = delete;
    node(T const& val, basenode* l = nullptr, basenode* r = nullptr) : val(val), basenode(l, r) {}
    ~node() = default;
  };

  template<typename R>
  struct my_iterator {
    using difference_type = ptrdiff_t;
    using reference = R const&;
    using pointer = R const*;
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = R;

    friend struct list<T>;

    my_iterator() = default;

    my_iterator(std::nullptr_t) = delete;

    template <typename Q>
    explicit my_iterator(my_iterator<Q>& other, typename std::enable_if<std::is_same<R, const Q>::value>::type* = nullptr)
      : ptr(other.ptr) {}

    my_iterator& operator=(my_iterator const&) = default;

    ~my_iterator() = default;

    my_iterator& operator++() {
      ptr = ptr->next;
      return *this;
    }

    my_iterator operator++(int) {
      my_iterator x = *this;
      ++*this;
      return x;
    }

    my_iterator& operator--() {
      ptr = ptr->prev;
      return *this;
    }

    my_iterator operator--(int) {
      my_iterator x = *this;
      --*this;
      return x;
    }

    const R& operator*() const {
      return static_cast<node*>(ptr)->val;
    }

    const R* operator->() const {
      return &static_cast<node*>(ptr)->val;
    }

    bool operator==(my_iterator const& a) const {
      return a.ptr == ptr;
    }

    bool operator!=(my_iterator const& a) const {
      return a.ptr != ptr;
    }

  private:
    basenode* ptr;

    explicit my_iterator(basenode* ptr) : ptr(ptr) {}
  };

public:
  // bidirectional iterator
  using iterator = my_iterator<T>;
  // bidirectional iterator
  using const_iterator = my_iterator<const T>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  // O(1)
  list() noexcept : fake(&fake, &fake) {}

  // O(n), strong
  list(list const& other) : list() {
    for (iterator it = other.begin(); it != end(); it++) {
      push_back(*it);
    }
  }

  // O(n), strong
  list& operator=(list const& other) {
    if (*this != other) {
      list tmp(other);
      swap(tmp, *this);
    }
    return *this;
  }

  // O(n)
  ~list() {
    clear();
  }

  // O(1)
  bool empty() const noexcept {
    return fake.next == &fake;
  }

  // O(1)
  T& front() noexcept {
    return *begin();
  }

  // O(1)
  T const& front() const noexcept {
    return *begin();
  }

  // O(1), strong
  void push_front(T const& x) {
    insert(begin(), x);
  }

  // O(1)
  void pop_front() noexcept {
    erase(begin());
  }

  // O(1)
  T& back() noexcept {
    return *(--end());
  }

  // O(1)
  T const& back() const noexcept {
    return *(--end());
  }

  // O(1), strong
  void push_back(T const& x) {
    insert(end(), x);
  }

  // O(1)
  void pop_back() noexcept {
    erase(--end());
  }

  // O(1)
  iterator begin() noexcept {
    return iterator(fake.next);
  }

  // O(1)
  const_iterator begin() const noexcept {
    return const_iterator(fake.next);
  }

  // O(1)
  iterator end() noexcept {
    return iterator(&fake);
  }

  // O(1)
  const_iterator end() const noexcept {
    return const_iterator(const_cast<node*>(&fake));
  }

  // O(1)
  reverse_iterator rbegin() noexcept {
    return reverse_iterator(end());
  }

  // O(1)
  const_reverse_iterator rbegin() const noexcept {
    return const_reverse_iterator(end());
  }

  // O(1)
  reverse_iterator rend() noexcept {
    return reverse_iterator(begin());
  }

  // O(1)
  const_reverse_iterator rend() const noexcept {
    return const_reverse_iterator(begin());
  }

  // O(n)
  void clear() noexcept {
    while (!empty()) {
      pop_front();
    }
  }

  // O(1), strong
  iterator insert(const_iterator pos, T const& val) {
    node* new_node = new node(val, pos.ptr->prev, pos.ptr);
    new_node->prev->next = new_node;
    new_node->next->prev = new_node;
    return iterator(new_node);
  }

  // O(1)
  iterator erase(const_iterator pos) noexcept {
    pos.ptr->next->prev = pos.ptr->prev;
    pos.ptr->prev->next = pos.ptr->next;
    node* ans = pos.ptr->next;
    delete static_cast<node*>(pos.ptr);
    return iterator(ans);
  }

  // O(n)
  iterator erase(const_iterator first, const_iterator last) noexcept {
    iterator it = first;
    while (it != last) {
      it = erase(it);
    }
    return it;
  }

  // O(1)
  void splice(const_iterator pos, list&, const_iterator first,
              const_iterator last) noexcept {
    if (first == last) return;
    first.ptr->prev->next = last.ptr;
    basenode* new_last = last.ptr->prev;
    last.ptr->prev = first.ptr->prev;
    new_last->next = pos.ptr;
    pos.ptr->prev = new_last;
    pos.ptr = first.ptr;
    first.ptr->prev = pos.ptr->prev;
  }

  friend void swap(list& a, list& b) noexcept {
    std::swap(a.fake, b.fake);
  }

private:
  basenode fake;

  void link_to(basenode* a, basenode* b) {
    a->next = b;
    b->prev = a;
  }
};

