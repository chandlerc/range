//===------------------------- range_test.cpp -----------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <range>

#include <assert.h>
#include <array>
#include <deque>
#include <forward_list>
#include <iterator>
#include <list>
#include <numeric>
#include <sstream>
#include <unistd.h>
#include <vector>

template<typename T, typename U> struct AssertTypesSame;
template<typename T> struct AssertTypesSame<T, T> {};
#define ASSERT_TYPES_SAME(T, U) AssertTypesSame<T, U>()

struct no_begin {};
struct member_begin {
  int* begin();
};
struct free_begin {};
int* begin(free_begin);

template<typename T>
auto type_of_forward(T&& t) -> decltype(std::forward<T>(t)) {
  return std::forward<T>(t);
}

template<typename To>
To implicit_cast(To val) { return val; }

void test_traits() {
  using namespace std::detail;
  ASSERT_TYPES_SAME(begin_result<no_begin>::type, undefined);
  ASSERT_TYPES_SAME(begin_result<decltype(type_of_forward(member_begin()))>::type, int*);
  ASSERT_TYPES_SAME(begin_result<free_begin>::type, int*);
}

void test_construction() {
  std::vector<int> v(5);
  std::iota(v.begin(), v.end(), 0);
  std::range<std::vector<int>::iterator> r = v;
  assert(r.begin() == v.begin());
  assert(r.end() == v.end());

  int arr[] = {1, 2, 3, 4, 5};
  std::begin(arr);
  std::range<int*> r2 = arr;
  assert(r2.back() == 5);
}

void test_conversion_to_pointer_range() {
  std::vector<int> v(5);
  std::iota(v.begin(), v.end(), 0);
  std::range<int*> r = v;
  assert(r.begin() == &*v.begin());
  assert(r[2] == 2);
}

template<typename Iter>
void takes_range(std::range<Iter> r) {
  int expected = 0;
  for (int val : r) {
    assert(val == expected++);
  }
}

void takes_ptr_range(std::range<const int*> r) {
  int expected = 0;
  for (int val : r) {
    assert(val == expected++);
  }
}

void test_passing() {
  using std::make_range;
  std::list<int> l(5);
  std::iota(l.begin(), l.end(), 0);
  takes_range(make_range(l));
  takes_range(make_range(implicit_cast<const std::list<int>&>(l)));
  std::deque<int> d(5);
  std::iota(d.begin(), d.end(), 0);
  takes_range(make_range(d));
  takes_range(make_range(implicit_cast<const std::deque<int>&>(d)));
  std::vector<int> v(5);
  std::iota(v.begin(), v.end(), 0);
  takes_range(make_range(v));
  takes_range(make_range(implicit_cast<const std::vector<int>&>(v)));
  static_assert(std::is_same<decltype(make_ptr_range(v)),
                             std::range<int*>>::value,
                "make_ptr_range should return a range of pointers");
  takes_range(make_ptr_range(v));
  takes_range(make_ptr_range(implicit_cast<const std::vector<int>&>(v)));
  int arr[] = {0, 1, 2, 3, 4};
  takes_range(make_range(arr));
  const int carr[] = {0, 1, 2, 3, 4};
  takes_range(make_range(carr));

  takes_ptr_range(v);
  takes_ptr_range(implicit_cast<const std::vector<int>&>(v));
  takes_ptr_range(arr);
  takes_ptr_range(carr);
}

void test_access() {
  std::array<int, 5> a = {{1, 2, 3, 4, 5}};
  std::range<decltype(a.begin())> r = a;
  assert(r[3] == 4);
  assert(r[-2] == 4);
}

template<bool b> struct CompileAssert;
template<> struct CompileAssert<true> {};

constexpr int arr[] = {1, 2, 3, 4, 5};
void test_constexpr() {
  constexpr std::range<const int*> r(arr, arr + 5);
  CompileAssert<r.front() == 1>();
  CompileAssert<r.size() == 5>();
  CompileAssert<r[4] == 5>();
}

template<typename Container>
void test_slice() {
  Container cont(10);
  std::iota(cont.begin(), cont.end(), 0);
  std::range<decltype(cont.begin())> r = cont;

  // One argument.
  assert(r.slice(0).size() == 10);
  assert(r.slice(2).size() == 8);
  assert(r.slice(2).front() == 2);
  assert(r.slice(-1).size() == 1);
  assert(r.slice(-1).front() == 9);

  // Two positive arguments.
  assert(r.slice(5, 2).empty());
  assert(r.slice(5, 2).begin() == next(cont.begin(), 5));
  assert(r.slice(1, 2).size() == 1);
  assert(r.slice(1, 2).front() == 1);

  // Two negative arguments.
  assert(r.slice(-2, -5).empty());
  assert(r.slice(-2, -5).begin() == next(cont.begin(), 8));
  assert(r.slice(-2, -1).size() == 1);
  assert(r.slice(-2, -1).front() == 8);

  // Positive start, negative stop.
  assert(r.slice(6, -3).size() == 1);
  assert(r.slice(6, -3).front() == 6);
  assert(r.slice(6, -5).empty());
  assert(r.slice(6, -5).begin() == next(cont.begin(), 6));

  // Negative start, positive stop.
  assert(r.slice(-3, 6).empty());
  assert(r.slice(-3, 6).begin() == next(cont.begin(), 7));
  assert(r.slice(-5, 6).size() == 1);
  assert(r.slice(-5, 6).front() == 5);
}

void test_istream_range() {
  std::istringstream stream("1 2 3 4 5");
  std::range<std::istream_iterator<int> > input({stream}, {});
  assert(input.front() == 1);
  input.pop_front();
  assert(input.front() == 2);
  input.pop_front(2);
  assert(input.front() == 4);
  input.pop_front_upto(7);
  assert(input.empty());
}

//! [algorithm using range]
template<typename T>
void partial_sum(T& container) {
  using std::make_range;
  auto range = make_range(container);
  typename T::value_type sum = 0;
  // One would actually use a range-based for loop
  // in this case, but you get the idea:
  for (; !range.empty(); range.pop_front()) {
    sum += range.front();
    range.front() = sum;
  }
}

void user1() {
  std::vector<int> v(5, 2);
  partial_sum(v);
  assert(v[3] == 8);
}
//! [algorithm using range]

//! [algorithm using ptr_range]
void my_write(int fd, std::range<const char*> buffer) {
  write(fd, buffer.begin(), buffer.size());
}

void user2() {
  std::string s("Hello world");
  my_write(1, s);
}
//! [algorithm using ptr_range]

int main() {
  test_traits();
  test_construction();
  test_conversion_to_pointer_range();
  test_passing();
  test_access();
  test_constexpr();
  test_slice<std::forward_list<int>>();
  test_slice<std::list<int>>();
  test_slice<std::deque<int>>();
  test_istream_range();
  user1();
  //user2();
}
