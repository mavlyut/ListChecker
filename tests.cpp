#include <gtest/gtest.h>

#include "list.h"

#include "tests-helpers/element.h"
#include "tests-helpers/fault-injection.h"

using container = list<element>;

template <typename T>
T const& as_const(T& obj) {
  return obj;
}

template <typename C, typename T>
void mass_push_back(C& c, std::initializer_list<T> elems) {
  for (T const& e : elems)
    c.push_back(e);
}

template <typename C, typename T>
void mass_push_front(C& c, std::initializer_list<T> elems) {
  for (T const& e : elems)
    c.push_front(e);
}

template <typename It, typename T>
void expect_eq(It i1, It e1, std::initializer_list<T> elems) {
  auto i2 = elems.begin(), e2 = elems.end();

  for (;;) {
    if (i1 == e1 || i2 == e2) {
      EXPECT_TRUE(i1 == e1 && i2 == e2);
      break;
    }

    EXPECT_EQ(*i2, *i1);
    ++i1;
    ++i2;
  }
}

template <typename C, typename T>
void expect_eq(C const& c, std::initializer_list<T> elems) {
  expect_eq(c.begin(), c.end(), elems);
}

template <typename C, typename T>
void expect_reverse_eq(C const& c, std::initializer_list<T> elems) {
  expect_eq(c.rbegin(), c.rend(), elems);
}

static_assert(
    !std::is_constructible<container::iterator, std::nullptr_t>::value,
    "iterator should not be constructible from nullptr");
static_assert(
    !std::is_constructible<container::const_iterator, std::nullptr_t>::value,
    "const_iterator should not be constructible from nullptr");

struct non_default_constructible {
  non_default_constructible() = delete;
};

TEST(correctness, non_default_type) {
  list<non_default_constructible> c;
  EXPECT_EQ(c.begin(), c.end());
}

TEST(correctness, default_ctor) {
  element::no_new_instances_guard g;

  container c;
  g.expect_no_instances();
}

TEST(correctness, end_iterator) {
  element::no_new_instances_guard g;

  container c;
  container::iterator i = c.end();

  EXPECT_EQ(c.begin(), i);
  c.push_back(5);
  --i;
  EXPECT_EQ(5, *i);
}

TEST(correctness, back_front) {
  element::no_new_instances_guard g;

  container c;
  mass_push_back(c, {1, 2, 3, 4, 5});
  EXPECT_EQ(1, c.front());
  EXPECT_EQ(1, ::as_const(c).front());
  EXPECT_EQ(5, c.back());
  EXPECT_EQ(5, ::as_const(c).back());
}

TEST(correctness, back_front_ref) {
  element::no_new_instances_guard g;

  container c;
  mass_push_back(c, {1, 2, 3, 4, 5});
  c.front() = 6;
  c.back() = 7;
  expect_eq(c, {6, 2, 3, 4, 7});
}

TEST(correctness, back_front_cref) {
  element::no_new_instances_guard g;

  container c;
  mass_push_back(c, {1, 2, 3, 4, 5});
  EXPECT_TRUE(&c.front() == &::as_const(c).front());
  EXPECT_TRUE(&c.back() == &::as_const(c).back());
}

void magic(element& c) {
  c = 42;
}

void magic(element const& c) {}

TEST(correctness, back_front_ncref) {
  element::no_new_instances_guard g;

  container c;
  mass_push_back(c, {1, 2, 3, 4, 5});
  magic(::as_const(c).front());
  magic(::as_const(c).back());

  expect_eq(c, {1, 2, 3, 4, 5});
}

TEST(correctness, iterator_deref_1) {
  element::no_new_instances_guard g;

  container c;
  mass_push_back(c, {1, 2, 3, 4, 5, 6});
  container::iterator i = std::next(c.begin(), 3);
  EXPECT_EQ(4, *i);
  magic(*i);
  expect_eq(c, {1, 2, 3, 42, 5, 6});

  container::const_iterator j = std::next(c.begin(), 2);
  EXPECT_EQ(3, *j);
  magic(*j);
  expect_eq(c, {1, 2, 3, 42, 5, 6});
}

TEST(correctness, iterator_deref_1c) {
  element::no_new_instances_guard g;

  container c;
  mass_push_back(c, {1, 2, 3, 4, 5, 6});
  container::iterator const i = std::next(c.begin(), 3);
  EXPECT_EQ(4, *i);
  magic(*i);
  expect_eq(c, {1, 2, 3, 42, 5, 6});

  container::const_iterator const j = std::next(c.begin(), 2);
  EXPECT_EQ(3, *j);
  magic(*j);
  expect_eq(c, {1, 2, 3, 42, 5, 6});
}

TEST(correctness, iterator_deref_2) {
  element::no_new_instances_guard g;

  container c;
  mass_push_back(c, {1, 2, 3, 4, 5, 6});
  container::iterator i = std::next(c.begin(), 3);
  magic(*i.operator->());
  expect_eq(c, {1, 2, 3, 42, 5, 6});

  container::const_iterator j = std::next(c.begin(), 2);
  magic(*j.operator->());
  expect_eq(c, {1, 2, 3, 42, 5, 6});
}

TEST(correctness, iterator_deref_2c) {
  element::no_new_instances_guard g;

  container c;
  mass_push_back(c, {1, 2, 3, 4, 5, 6});
  container::iterator const i = std::next(c.begin(), 3);
  magic(*i.operator->());
  expect_eq(c, {1, 2, 3, 42, 5, 6});

  container::const_iterator const j = std::next(c.begin(), 2);
  EXPECT_EQ(3, *j);
  magic(*j.operator->());
  expect_eq(c, {1, 2, 3, 42, 5, 6});
}

TEST(correctness, iterator_pre_inc) {
  element::no_new_instances_guard g;

  container c;
  mass_push_back(c, {1, 2, 3, 4, 5, 6});
  container::iterator i = std::next(c.begin(), 3);

  ++ ++i;
  EXPECT_EQ(6, *i);
}

TEST(correctness, const_iterator_pre_inc) {
  element::no_new_instances_guard g;

  container c;
  mass_push_back(c, {1, 2, 3, 4, 5, 6});
  container::const_iterator i = std::next(c.begin(), 3);

  ++ ++i;
  EXPECT_EQ(6, *i);
}

TEST(correctness, iterator_constness) {
  container c;
  mass_push_back(c, {1, 2, 3});
  magic(*::as_const(c).begin());
  magic(*std::prev(::as_const(c).end()));
  expect_eq(c, {1, 2, 3});
}

TEST(correctness, reverse_iterator_constness) {
  container c;
  mass_push_back(c, {1, 2, 3});
  magic(*::as_const(c).rbegin());
  magic(*std::prev(::as_const(c).rend()));
  expect_eq(c, {1, 2, 3});
}

TEST(correctness, push_back) {
  element::no_new_instances_guard g;

  container c;
  mass_push_back(c, {1, 2, 3, 4});
  expect_eq(c, {1, 2, 3, 4});
}

TEST(correctness, copy_ctor) {
  element::no_new_instances_guard g;

  container c;
  mass_push_back(c, {1, 2, 3, 4});
  container c2 = c;
  expect_eq(c2, {1, 2, 3, 4});
}

TEST(correctness, copy_ctor_empty) {
  element::no_new_instances_guard g;

  container c;
  container c2 = c;
  EXPECT_TRUE(c2.empty());
}

TEST(correctness, assignment_operator) {
  element::no_new_instances_guard g;

  container c;
  mass_push_back(c, {1, 2, 3, 4});
  container c2;
  mass_push_back(c2, {5, 6, 7, 8});
  c2 = c;
  expect_eq(c2, {1, 2, 3, 4});
}

TEST(correctness, self_assignment) {
  element::no_new_instances_guard g;

  container c;
  mass_push_back(c, {1, 2, 3, 4});
  c = c;
  expect_eq(c, {1, 2, 3, 4});
}

TEST(correctness, pop_back) {
  element::no_new_instances_guard g;

  container c;
  mass_push_back(c, {1, 2, 3, 4});
  c.pop_back();
  expect_eq(c, {1, 2, 3});
  c.pop_back();
  expect_eq(c, {1, 2});
  c.pop_back();
  expect_eq(c, {1});
  c.pop_back();
  EXPECT_TRUE(c.empty());
}

TEST(correctness, push_front) {
  element::no_new_instances_guard g;

  container c;
  mass_push_front(c, {1, 2, 3, 4});
  expect_eq(c, {4, 3, 2, 1});
}

TEST(correctness, empty) {
  element::no_new_instances_guard g;

  container c;
  EXPECT_EQ(c.begin(), c.end());
  EXPECT_TRUE(c.empty());
  c.push_back(1);
  EXPECT_NE(c.begin(), c.end());
  EXPECT_FALSE(c.empty());
  c.pop_front();
  EXPECT_EQ(c.begin(), c.end());
  EXPECT_TRUE(c.empty());
}

TEST(correctness, reverse_iterators) {
  element::no_new_instances_guard g;

  container c;
  mass_push_front(c, {1, 2, 3, 4});
  expect_reverse_eq(c, {1, 2, 3, 4});

  EXPECT_EQ(1, *c.rbegin());
  EXPECT_EQ(2, *std::next(c.rbegin()));
  EXPECT_EQ(4, *std::prev(c.rend()));
}

TEST(correctness, iterator_conversions) {
  element::no_new_instances_guard g;

  container c;
  container::const_iterator i1 = c.begin();
  container::iterator i2 = c.end();
  EXPECT_TRUE(i1 == i1);
  EXPECT_TRUE(i1 == i2);
  EXPECT_TRUE(i2 == i1);
  EXPECT_TRUE(i2 == i2);
  EXPECT_FALSE(i1 != i1);
  EXPECT_FALSE(i1 != i2);
  EXPECT_FALSE(i2 != i1);
  EXPECT_FALSE(i2 != i2);

  EXPECT_TRUE(::as_const(i1) == i1);
  EXPECT_TRUE(::as_const(i1) == i2);
  EXPECT_TRUE(::as_const(i2) == i1);
  EXPECT_TRUE(::as_const(i2) == i2);
  EXPECT_FALSE(::as_const(i1) != i1);
  EXPECT_FALSE(::as_const(i1) != i2);
  EXPECT_FALSE(::as_const(i2) != i1);
  EXPECT_FALSE(::as_const(i2) != i2);

  EXPECT_TRUE(i1 == ::as_const(i1));
  EXPECT_TRUE(i1 == ::as_const(i2));
  EXPECT_TRUE(i2 == ::as_const(i1));
  EXPECT_TRUE(i2 == ::as_const(i2));
  EXPECT_FALSE(i1 != ::as_const(i1));
  EXPECT_FALSE(i1 != ::as_const(i2));
  EXPECT_FALSE(i2 != ::as_const(i1));
  EXPECT_FALSE(i2 != ::as_const(i2));

  EXPECT_TRUE(::as_const(i1) == ::as_const(i1));
  EXPECT_TRUE(::as_const(i1) == ::as_const(i2));
  EXPECT_TRUE(::as_const(i2) == ::as_const(i1));
  EXPECT_TRUE(::as_const(i2) == ::as_const(i2));
  EXPECT_FALSE(::as_const(i1) != ::as_const(i1));
  EXPECT_FALSE(::as_const(i1) != ::as_const(i2));
  EXPECT_FALSE(::as_const(i2) != ::as_const(i1));
  EXPECT_FALSE(::as_const(i2) != ::as_const(i2));
}

TEST(correctness, iterators_postfix) {
  element::no_new_instances_guard g;

  container s;
  mass_push_back(s, {1, 2, 3});
  container::iterator i = s.begin();
  EXPECT_EQ(1, *i);
  container::iterator j = i++;
  EXPECT_EQ(2, *i);
  EXPECT_EQ(1, *j);
  j = i++;
  EXPECT_EQ(3, *i);
  EXPECT_EQ(2, *j);
  j = i++;
  EXPECT_EQ(s.end(), i);
  EXPECT_EQ(3, *j);
  j = i--;
  EXPECT_EQ(3, *i);
  EXPECT_EQ(s.end(), j);
}

TEST(correctness, const_iterators_postfix) {
  element::no_new_instances_guard g;

  container s;
  mass_push_back(s, {1, 2, 3});
  container::const_iterator i = s.begin();
  EXPECT_EQ(1, *i);
  container::const_iterator j = i++;
  EXPECT_EQ(2, *i);
  EXPECT_EQ(1, *j);
  j = i++;
  EXPECT_EQ(3, *i);
  EXPECT_EQ(2, *j);
  j = i++;
  EXPECT_TRUE(i == s.end());
  EXPECT_EQ(3, *j);
  j = i--;
  EXPECT_EQ(3, *i);
  EXPECT_TRUE(j == s.end());
}

TEST(correctness, insert_begin) {
  element::no_new_instances_guard g;

  container c;
  mass_push_back(c, {1, 2, 3, 4});
  c.insert(c.begin(), 0);
  expect_eq(c, {0, 1, 2, 3, 4});
}

TEST(correctness, insert_middle) {
  element::no_new_instances_guard g;

  container c;
  mass_push_back(c, {1, 2, 3, 4});
  c.insert(std::next(c.begin(), 2), 5);
  expect_eq(c, {1, 2, 5, 3, 4});
}

TEST(correctness, insert_end) {
  element::no_new_instances_guard g;

  container c;
  mass_push_back(c, {1, 2, 3, 4});
  c.insert(c.end(), 5);
  expect_eq(c, {1, 2, 3, 4, 5});
}

TEST(correctness, insert_iterators) {
  element::no_new_instances_guard g;

  container c;
  mass_push_back(c, {1, 2, 3, 4});

  container::iterator i2 = c.begin();
  ++i2;
  container::iterator i3 = i2;
  ++i3;

  c.insert(i3, 5);
  --i3;
  EXPECT_EQ(5, *i3);
  ++i2;
  EXPECT_EQ(5, *i2);
  --i3;
  EXPECT_EQ(2, *i3);
  ++i2;
  EXPECT_EQ(3, *i2);
}

TEST(correctness, insert_return_value) {
  element::no_new_instances_guard g;

  container c;
  mass_push_back(c, {1, 2, 3, 4});

  container::iterator i = c.insert(std::next(c.begin(), 2), 5);
  EXPECT_EQ(5, *i);
  EXPECT_EQ(2, *std::prev(i));
  EXPECT_EQ(3, *std::next(i));
}

TEST(correctness, erase_begin) {
  element::no_new_instances_guard g;

  container c;
  mass_push_back(c, {1, 2, 3, 4});
  c.erase(c.begin());
  expect_eq(c, {2, 3, 4});
}

TEST(correctness, erase_middle) {
  element::no_new_instances_guard g;

  container c;
  mass_push_back(c, {1, 2, 3, 4});
  c.erase(std::next(c.begin(), 2));
  expect_eq(c, {1, 2, 4});
}

TEST(correctness, erase_end) {
  element::no_new_instances_guard g;

  container c;
  mass_push_back(c, {1, 2, 3, 4});
  c.erase(std::prev(c.end()));
  expect_eq(c, {1, 2, 3});
}

TEST(correctness, erase_iterators) {
  element::no_new_instances_guard g;

  container c;
  mass_push_back(c, {1, 2, 3, 4});

  container::iterator i2 = c.begin();
  ++i2;
  container::iterator i3 = i2;
  ++i3;
  container::iterator i4 = i3;
  ++i4;

  c.erase(i3);
  --i4;
  ++i2;
  EXPECT_EQ(2, *i4);
  EXPECT_EQ(4, *i2);
}

TEST(correctness, erase_end_whole) {
  element::no_new_instances_guard g;

  container c;
  mass_push_back(c, {1, 2, 3, 4});
  c.erase(c.begin(), c.end());
  EXPECT_TRUE(c.empty());
  EXPECT_EQ(c.begin(), c.end());
}

TEST(correctness, erase_return_value) {
  element::no_new_instances_guard g;

  container c;
  mass_push_back(c, {1, 2, 3, 4});
  container::iterator i = c.erase(std::next(::as_const(c).begin()));
  EXPECT_EQ(3, *i);
  i = c.erase(i);
  EXPECT_EQ(4, *i);
}

TEST(correctness, erase_range_return_value) {
  element::no_new_instances_guard g;

  container c;
  mass_push_back(c, {1, 2, 3, 4, 5});
  container::iterator i = c.erase(std::next(::as_const(c).begin()),
                                  std::next(::as_const(c).begin(), 3));
  EXPECT_EQ(4, *i);
  i = c.erase(i);
  EXPECT_EQ(5, *i);
}

TEST(correctness, erase_upto_end_return_value) {
  element::no_new_instances_guard g;

  container c;
  mass_push_back(c, {1, 2, 3, 4, 5});
  container::iterator i =
      c.erase(std::next(::as_const(c).begin(), 2), ::as_const(c).end());
  EXPECT_TRUE(i == c.end());
  --i;
  EXPECT_EQ(2, *i);
}

TEST(correctness, splice_begin_begin) {
  element::no_new_instances_guard g;

  container c1, c2;
  mass_push_back(c1, {1, 2, 3, 4});
  mass_push_back(c2, {5, 6, 7, 8});
  c1.splice(c1.begin(), c2, c2.begin(), std::next(c2.begin(), 2));
  expect_eq(c1, {5, 6, 1, 2, 3, 4});
  expect_eq(c2, {7, 8});
}

TEST(correctness, splice_begin_middle) {
  element::no_new_instances_guard g;

  container c1, c2;
  mass_push_back(c1, {1, 2, 3, 4});
  mass_push_back(c2, {5, 6, 7, 8});
  c1.splice(c1.begin(), c2, std::next(c2.begin()), std::next(c2.begin(), 2));
  expect_eq(c1, {6, 1, 2, 3, 4});
  expect_eq(c2, {5, 7, 8});
}

TEST(correctness, splice_begin_end) {
  element::no_new_instances_guard g;

  container c1, c2;
  mass_push_back(c1, {1, 2, 3, 4});
  mass_push_back(c2, {5, 6, 7, 8});
  c1.splice(c1.begin(), c2, std::next(c2.begin(), 2), c2.end());
  expect_eq(c1, {7, 8, 1, 2, 3, 4});
  expect_eq(c2, {5, 6});
}

TEST(correctness, splice_begin_whole) {
  element::no_new_instances_guard g;

  container c1, c2;
  mass_push_back(c1, {1, 2, 3, 4});
  mass_push_back(c2, {5, 6, 7, 8});
  c1.splice(c1.begin(), c2, c2.begin(), c2.end());
  expect_eq(c1, {5, 6, 7, 8, 1, 2, 3, 4});
  EXPECT_TRUE(c2.empty());
}

TEST(correctness, splice_begin_empty) {
  element::no_new_instances_guard g;

  container c1, c2;
  mass_push_back(c1, {1, 2, 3, 4});
  mass_push_back(c2, {5, 6, 7, 8});
  c1.splice(c1.begin(), c2, std::next(c2.begin(), 2), std::next(c2.begin(), 2));
  expect_eq(c1, {1, 2, 3, 4});
  expect_eq(c2, {5, 6, 7, 8});
}

TEST(correctness, splice_middle_begin) {
  element::no_new_instances_guard g;

  container c1, c2;
  mass_push_back(c1, {1, 2, 3, 4});
  mass_push_back(c2, {5, 6, 7, 8});
  c1.splice(std::next(c1.begin(), 2), c2, c2.begin(), std::next(c2.begin(), 2));
  expect_eq(c1, {1, 2, 5, 6, 3, 4});
  expect_eq(c2, {7, 8});
}

TEST(correctness, splice_middle_middle) {
  element::no_new_instances_guard g;

  container c1, c2;
  mass_push_back(c1, {1, 2, 3, 4});
  mass_push_back(c2, {5, 6, 7, 8});
  c1.splice(std::next(c1.begin(), 2), c2, std::next(c2.begin()),
            std::next(c2.begin(), 3));
  expect_eq(c1, {1, 2, 6, 7, 3, 4});
  expect_eq(c2, {5, 8});
}

TEST(correctness, splice_middle_end) {
  element::no_new_instances_guard g;

  container c1, c2;
  mass_push_back(c1, {1, 2, 3, 4});
  mass_push_back(c2, {5, 6, 7, 8});
  c1.splice(std::next(c1.begin(), 2), c2, std::next(c2.begin(), 2), c2.end());
  expect_eq(c1, {1, 2, 7, 8, 3, 4});
  expect_eq(c2, {5, 6});
}

TEST(correctness, splice_middle_whole) {
  element::no_new_instances_guard g;

  container c1, c2;
  mass_push_back(c1, {1, 2, 3, 4});
  mass_push_back(c2, {5, 6, 7, 8});
  c1.splice(std::next(c1.begin(), 2), c2, c2.begin(), c2.end());
  expect_eq(c1, {1, 2, 5, 6, 7, 8, 3, 4});
  EXPECT_TRUE(c2.empty());
}

TEST(correctness, splice_middle_empty) {
  element::no_new_instances_guard g;

  container c1, c2;
  mass_push_back(c1, {1, 2, 3, 4});
  mass_push_back(c2, {5, 6, 7, 8});
  c1.splice(std::next(c1.begin(), 2), c2, std::next(c2.begin(), 2),
            std::next(c2.begin(), 2));
  expect_eq(c1, {1, 2, 3, 4});
  expect_eq(c2, {5, 6, 7, 8});
}

TEST(correctness, splice_end_begin) {
  element::no_new_instances_guard g;

  container c1, c2;
  mass_push_back(c1, {1, 2, 3, 4});
  mass_push_back(c2, {5, 6, 7, 8});
  c1.splice(c1.end(), c2, c2.begin(), std::next(c2.begin(), 2));
  expect_eq(c1, {1, 2, 3, 4, 5, 6});
  expect_eq(c2, {7, 8});
}

TEST(correctness, splice_end_middle) {
  element::no_new_instances_guard g;

  container c1, c2;
  mass_push_back(c1, {1, 2, 3, 4});
  mass_push_back(c2, {5, 6, 7, 8});
  c1.splice(c1.end(), c2, std::next(c2.begin()), std::next(c2.begin(), 3));
  expect_eq(c1, {1, 2, 3, 4, 6, 7});
  expect_eq(c2, {5, 8});
}

TEST(correctness, splice_end_end) {
  element::no_new_instances_guard g;

  container c1, c2;
  mass_push_back(c1, {1, 2, 3, 4});
  mass_push_back(c2, {5, 6, 7, 8});
  c1.splice(c1.end(), c2, std::next(c2.begin(), 2), c2.end());
  expect_eq(c1, {1, 2, 3, 4, 7, 8});
  expect_eq(c2, {5, 6});
}

TEST(correctness, splice_end_whole) {
  element::no_new_instances_guard g;

  container c1, c2;
  mass_push_back(c1, {1, 2, 3, 4});
  mass_push_back(c2, {5, 6, 7, 8});
  c1.splice(c1.end(), c2, c2.begin(), c2.end());
  expect_eq(c1, {1, 2, 3, 4, 5, 6, 7, 8});
  EXPECT_TRUE(c2.empty());
}

TEST(correctness, splice_end_empty) {
  element::no_new_instances_guard g;

  container c1, c2;
  mass_push_back(c1, {1, 2, 3, 4});
  mass_push_back(c2, {5, 6, 7, 8});
  c1.splice(c1.end(), c2, std::next(c2.begin(), 2), std::next(c2.begin(), 2));
  expect_eq(c1, {1, 2, 3, 4});
  expect_eq(c2, {5, 6, 7, 8});
}

TEST(correctness, splice_empty_begin) {
  element::no_new_instances_guard g;

  container c1, c2;
  mass_push_back(c2, {5, 6, 7, 8});
  c1.splice(c1.end(), c2, c2.begin(), std::next(c2.begin(), 2));
  expect_eq(c1, {5, 6});
  expect_eq(c2, {7, 8});
}

TEST(correctness, splice_empty_middle) {
  element::no_new_instances_guard g;

  container c1, c2;
  mass_push_back(c2, {5, 6, 7, 8});
  c1.splice(c1.end(), c2, std::next(c2.begin(), 1), std::next(c2.begin(), 3));
  expect_eq(c1, {6, 7});
  expect_eq(c2, {5, 8});
}

TEST(correctness, splice_empty_end) {
  element::no_new_instances_guard g;

  container c1, c2;
  mass_push_back(c2, {5, 6, 7, 8});
  c1.splice(c1.end(), c2, std::next(c2.begin(), 2), c2.end());
  expect_eq(c1, {7, 8});
  expect_eq(c2, {5, 6});
}

TEST(correctness, splice_empty_whole) {
  element::no_new_instances_guard g;

  container c1, c2;
  mass_push_back(c2, {5, 6, 7, 8});
  c1.splice(c1.end(), c2, c2.begin(), c2.end());
  expect_eq(c1, {5, 6, 7, 8});
  EXPECT_TRUE(c2.empty());
}

TEST(correctness, splice_self) {
  element::no_new_instances_guard g;

  container c1;
  mass_push_back(c1, {1, 2, 3, 4, 5});
  c1.splice(std::next(c1.begin()), c1, std::next(c1.begin(), 2),
            std::prev(c1.end()));
  expect_eq(c1, {1, 3, 4, 2, 5});
}

TEST(correctness, splice_iterators) {
  element::no_new_instances_guard g;

  container c1;
  container c2;
  mass_push_back(c1, {1, 2, 3, 4});
  mass_push_back(c2, {5, 6, 7, 8});
  container::const_iterator i = std::next(c1.begin(), 2);
  container::const_iterator j = std::next(c2.begin());
  container::const_iterator k = std::prev(c2.end());
  c1.splice(i, c2, j, k);
  expect_eq(c1, {1, 2, 6, 7, 3, 4});
  expect_eq(c2, {5, 8});

  EXPECT_EQ(3, *i);
  EXPECT_EQ(6, *j);
  EXPECT_EQ(8, *k);

  EXPECT_EQ(7, *std::prev(i));
  EXPECT_EQ(2, *std::prev(j));
  EXPECT_EQ(5, *std::prev(k));
}

TEST(correctness, swap) {
  element::no_new_instances_guard g;

  container c1, c2;
  mass_push_back(c1, {1, 2, 3, 4});
  mass_push_back(c2, {5, 6, 7, 8});
  swap(c1, c2);
  expect_eq(c1, {5, 6, 7, 8});
  expect_eq(c2, {1, 2, 3, 4});
}

TEST(correctness, swap_self) {
  element::no_new_instances_guard g;

  container c1;
  mass_push_back(c1, {1, 2, 3, 4});
  swap(c1, c1);
}

TEST(correctness, swap_empty) {
  element::no_new_instances_guard g;

  container c1, c2;
  mass_push_back(c1, {1, 2, 3, 4});
  swap(c1, c2);
  EXPECT_TRUE(c1.empty());
  expect_eq(c2, {1, 2, 3, 4});
  swap(c1, c2);
  expect_eq(c1, {1, 2, 3, 4});
  EXPECT_TRUE(c2.empty());
}

TEST(correctness, swap_empty_empty) {
  element::no_new_instances_guard g;

  container c1, c2;
  swap(c1, c2);
}

TEST(correctness, swap_empty_self) {
  element::no_new_instances_guard g;

  container c1;
  swap(c1, c1);
}

TEST(correctness, clear_empty) {
  element::no_new_instances_guard g;

  container c;
  c.clear();
  EXPECT_TRUE(c.empty());
  c.clear();
  EXPECT_TRUE(c.empty());
  c.clear();
  EXPECT_TRUE(c.empty());
}

TEST(correctness, clear) {
  element::no_new_instances_guard g;

  container c;
  mass_push_back(c, {1, 2, 3, 4});
  c.clear();
  EXPECT_TRUE(c.empty());
  EXPECT_EQ(c.begin(), c.end());
  mass_push_back(c, {5, 6, 7, 8});
  expect_eq(c, {5, 6, 7, 8});
}

TEST(fault_injection, push_back) {
  element::no_new_instances_guard g;
  faulty_run([] {
    container c;
    mass_push_back(c, {1, 2, 3, 4});
  });
}

TEST(fault_injection, copy_ctor) {
  element::no_new_instances_guard g;
  faulty_run([] {
    container c;
    mass_push_back(c, {1, 2, 3, 4});
    container c2 = c;
    expect_eq(c2, {1, 2, 3, 4});
  });
}

TEST(fault_injection, assignment_operator) {
  element::no_new_instances_guard g;
  faulty_run([] {
    container c;
    mass_push_back(c, {1, 2, 3, 4});
    container c2;
    mass_push_back(c2, {5, 6, 7, 8});
    c2 = c;
    expect_eq(c2, {1, 2, 3, 4});
  });
}
