#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE atomic_test

#include "async/atomic.hpp"

#include <boost/test/unit_test.hpp>
#include <future>
#include <iostream>
#include <numeric>

using namespace nil;

BOOST_AUTO_TEST_CASE(BasicTest) {
  atomic<std::string> atomic_str{"hello"};
  BOOST_CHECK_EQUAL(atomic_str.peek(), "hello");
  BOOST_CHECK_EQUAL(atomic_str.peek(), "hello");

  atomic_str.push("yo");
  BOOST_CHECK_EQUAL(atomic_str.peek(), "yo");

  const auto& const_atomic_str = atomic_str;
  std::string str = "";
  const_atomic_str.apply([&str](const auto& a_str) { str = a_str; });
  BOOST_CHECK_EQUAL("yo", str);

  str = const_atomic_str.apply(
      [](const auto& a_str) { return a_str + std::string{"yo"}; });
  BOOST_CHECK_EQUAL("yoyo", str);
  BOOST_CHECK_EQUAL("yo", atomic_str.peek());

  str = atomic_str.apply([](auto& a_str) {
    a_str += std::string{"ho"};
    return a_str;
  });
  BOOST_CHECK_EQUAL("yoho", str);
  BOOST_CHECK_EQUAL("yoho", atomic_str.peek());
}

BOOST_AUTO_TEST_CASE(MultithreadedTest) {
  nil::atomic<int> counter{0};
  nil::atomic<std::vector<int>> atomic_vec;
  const auto counter_max = 10000;

  auto func = [&]() {
    while (true) {
      auto next_val = counter.apply([](auto& ii) {
        ii++;
        return ii;
      });
      if (next_val > counter_max) {
        return;
      }
      atomic_vec.apply([&](auto& vec) { vec.push_back(next_val); });
    }
  };

  auto f1 = std::async(std::launch::async, func);
  auto f2 = std::async(std::launch::async, func);
  auto f3 = std::async(std::launch::async, func);
  auto f4 = std::async(std::launch::async, func);

  f1.get();
  f2.get();
  f3.get();
  f4.get();

  auto vec = atomic_vec.peek();
  BOOST_CHECK_EQUAL(std::accumulate(vec.cbegin(), vec.cend(), 0),
                    counter_max * (counter_max + 1) / 2);
}