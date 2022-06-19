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
  struct my_iterator : std::iterator<std::bidirectional_iterator_tag, R> {
    friend struct list<T>;

    my_iterator() = default;

    my_iterator(std::nullptr_t) = delete;

    my_iterator(basenode* ptr) : ptr(ptr) {}

    template <typename Q>
    my_iterator(my_iterator<Q> const& other, typename std::enable_if<std::is_same<const Q, R>::value>::type* = nullptr)
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

    R& operator*() const {
      return static_cast<node*>(ptr)->val;
    }

    R* operator->() const {
      return &static_cast<node*>(ptr)->val;
    }

    friend bool operator==(my_iterator const& a, my_iterator const& b) {
      return a.ptr == b.ptr;
    }

    friend bool operator!=(my_iterator const& a, my_iterator const& b) {
      return a.ptr != b.ptr;
    }

  private:
    basenode* ptr;
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
    if (&other == this) return *this;
    list tmp(other);
    swap(tmp, *this);
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
    return const_iterator(const_cast<node*>(fake.next));
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
    basenode* ans = pos.ptr->next;
    delete static_cast<node*>(pos.ptr);
    return iterator(ans);
  }

  // O(n)
  iterator erase(const_iterator first, const_iterator last) noexcept {
    iterator it = iterator(first.ptr);
    while (it != last) {
      it = erase(it);
    }
    return it;
  }

  // O(1)
  void splice(const_iterator pos, list&, const_iterator first,
              const_iterator last) noexcept {
    if (first == last) return;
    basenode* new_last = last.ptr->prev;
    link_to(first.ptr->prev, last.ptr);
    link_to(pos.ptr->prev, first.ptr);
    link_to(new_last, pos.ptr);
  }

  friend void swap(list& a, list& b) noexcept {
    list tmp;
    tmp.splice(tmp.begin(), tmp, a.begin(), a.end());
    a.splice(a.begin(), a, b.begin(), b.end());
    b.splice(b.begin(), b, tmp.begin(), tmp.end());
  }

private:
  basenode fake;

  void link_to(basenode* a, basenode* b) {
    a->next = b;
    b->prev = a;
  }
};

