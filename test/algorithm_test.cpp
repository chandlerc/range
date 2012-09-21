//===------------------------- algoruthm_test.cpp -------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <algorithm>
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

template<typename T>
auto type_of_forward(T&& t) -> decltype(std::forward<T>(t)) {
  return std::forward<T>(t);
}

template<typename To>
To implicit_cast(To val) { return val; }

int main() {
}
