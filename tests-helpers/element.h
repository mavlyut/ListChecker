#pragma once

#include <set>

struct element {
  struct no_new_instances_guard;

  element() = delete;
  element(int data);
  element(element const& other);
  ~element();

  element& operator=(element const& c);
  operator int() const;

private:
  int data;

  friend bool operator==(element const& a, element const& b);
  friend bool operator!=(element const& a, element const& b);
  friend bool operator<(element const& a, element const& b);
  friend bool operator<=(element const& a, element const& b);
  friend bool operator>(element const& a, element const& b);
  friend bool operator>=(element const& a, element const& b);

  friend bool operator==(element const& a, int b);
  friend bool operator!=(element const& a, int b);
  friend bool operator<(element const& a, int b);
  friend bool operator<=(element const& a, int b);
  friend bool operator>(element const& a, int b);
  friend bool operator>=(element const& a, int b);

  friend bool operator==(int a, element const& b);
  friend bool operator!=(int a, element const& b);
  friend bool operator<(int a, element const& b);
  friend bool operator<=(int a, element const& b);
  friend bool operator>(int a, element const& b);
  friend bool operator>=(int a, element const& b);

  static std::set<element const*> instances;
};

struct element::no_new_instances_guard {
  no_new_instances_guard();

  no_new_instances_guard(no_new_instances_guard const&) = delete;
  no_new_instances_guard& operator=(no_new_instances_guard const&) = delete;

  ~no_new_instances_guard();

  void expect_no_instances();

private:
  std::set<element const*> old_instances;
};
