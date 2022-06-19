#include "element.h"
#include "fault-injection.h"
#include <gtest/gtest.h>

element::element(int data) : data(data) {
  auto p = instances.insert(this);
  EXPECT_TRUE(p.second);
}

element::element(element const& other) : data(other.data) {
  auto p = instances.insert(this);
  EXPECT_TRUE(p.second);
}

element::~element() {
  size_t n = instances.erase(this);
  EXPECT_EQ(1u, n);
}

element& element::operator=(element const& c) {
  EXPECT_TRUE(instances.find(this) != instances.end());

  data = c.data;
  return *this;
}

element::operator int() const {
  EXPECT_TRUE(instances.find(this) != instances.end());

  return data;
}

bool operator==(element const& a, element const& b) {
  fault_injection_point();
  return a.data == b.data;
}

bool operator!=(element const& a, element const& b) {
  fault_injection_point();
  return a.data != b.data;
}

bool operator<(element const& a, element const& b) {
  fault_injection_point();
  return a.data < b.data;
}

bool operator<=(element const& a, element const& b) {
  fault_injection_point();
  return a.data <= b.data;
}

bool operator>(element const& a, element const& b) {
  fault_injection_point();
  return a.data > b.data;
}

bool operator>=(element const& a, element const& b) {
  fault_injection_point();
  return a.data >= b.data;
}

bool operator==(element const& a, int b) {
  fault_injection_point();
  return a.data == b;
}

bool operator!=(element const& a, int b) {
  fault_injection_point();
  return a.data != b;
}

bool operator<(element const& a, int b) {
  fault_injection_point();
  return a.data < b;
}

bool operator<=(element const& a, int b) {
  fault_injection_point();
  return a.data <= b;
}

bool operator>(element const& a, int b) {
  fault_injection_point();
  return a.data > b;
}

bool operator>=(element const& a, int b) {
  fault_injection_point();
  return a.data >= b;
}

bool operator==(int a, element const& b) {
  fault_injection_point();
  return a == b.data;
}

bool operator!=(int a, element const& b) {
  fault_injection_point();
  return a != b.data;
}

bool operator<(int a, element const& b) {
  fault_injection_point();
  return a < b.data;
}

bool operator<=(int a, element const& b) {
  fault_injection_point();
  return a <= b.data;
}

bool operator>(int a, element const& b) {
  fault_injection_point();
  return a > b.data;
}

bool operator>=(int a, element const& b) {
  fault_injection_point();
  return a >= b.data;
}

std::set<element const*> element::instances;

element::no_new_instances_guard::no_new_instances_guard()
    : old_instances(instances) {}

element::no_new_instances_guard::~no_new_instances_guard() {
  EXPECT_TRUE(old_instances == instances);
}

void element::no_new_instances_guard::expect_no_instances() {
  EXPECT_TRUE(old_instances == instances);
}
