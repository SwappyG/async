#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE atomic_rw_test

#include "async/atomic_rw.hpp"

#include <boost/test/unit_test.hpp>
#include <future>
#include <iostream>
#include <numeric>
#include <set>

#include "async/atomic.hpp"
#include "async/vector.hpp"

using namespace nil;
using namespace nil::async;

BOOST_AUTO_TEST_CASE(ReadTest) {
  atomic_rw<std::set<int>> async_set{1, 4, 5, 6};
  BOOST_CHECK(async_set.copy() == (std::set<int>{1, 4, 5, 6}));
  async_set.assign(4, 6, 7, 9);
  BOOST_CHECK(async_set.copy() == (std::set<int>{4, 6, 7, 9}));

  auto read_func_1 = [&]() {
    for (size_t ii{0}; ii < 100; ++ii) {
      auto read_proxy = async_set.read();
      BOOST_CHECK_EQUAL(read_proxy->size(), 4);
      BOOST_CHECK_EQUAL(read_proxy.value().count(4), 1u);
      BOOST_CHECK_EQUAL((*read_proxy).count(2), 0u);
      BOOST_CHECK(!read_proxy->empty());
      BOOST_CHECK_EQUAL(4 + 6 + 7 + 9, std::accumulate(read_proxy->cbegin(),
                                                       read_proxy->cend(), 0));
    }
  };

  auto read_func_2 = [&]() {
    for (size_t ii{0}; ii < 100; ++ii) {
      BOOST_CHECK_EQUAL(async_set->size(), 4);
      BOOST_CHECK_EQUAL(async_set->count(4), 1u);
      BOOST_CHECK_EQUAL((*async_set)->count(2), 0u);
      BOOST_CHECK(!async_set.copy().empty());
      BOOST_CHECK_EQUAL(4 + 6 + 7 + 9, std::accumulate(async_set->cbegin(),
                                                       async_set->cend(), 0));
    }
  };

  auto f1 = std::async(std::launch::async, read_func_1);
  auto f2 = std::async(std::launch::async, read_func_1);
  auto f3 = std::async(std::launch::async, read_func_2);
  auto f4 = std::async(std::launch::async, read_func_1);
  auto f5 = std::async(std::launch::async, read_func_2);

  f1.get();
  f2.get();
  f3.get();
  f4.get();
  f5.get();
}

BOOST_AUTO_TEST_CASE(WriteTest) {
  atomic_rw<std::set<int>> async_set{1, 4, 5, 6};
  const auto& const_async_set = async_set;
  auto read_func = [&const_async_set]() {
    for (size_t ii{0}; ii < 100; ++ii) {
      {
        auto read_proxy = const_async_set.read();
        BOOST_CHECK_EQUAL(read_proxy->size(), 4);
        BOOST_CHECK_EQUAL(read_proxy->count(4), 1u);
        BOOST_CHECK_EQUAL(read_proxy->count(12), 0u);
        BOOST_CHECK_EQUAL(read_proxy->count(13), 0u);
        BOOST_CHECK_EQUAL(read_proxy->count(14), 0u);
        BOOST_CHECK_EQUAL(read_proxy->count(15), 0u);
        BOOST_CHECK(!read_proxy->empty());
        BOOST_CHECK_EQUAL(
            1 + 4 + 5 + 6,
            std::accumulate(read_proxy->cbegin(), read_proxy->cend(), 0));
      }
      for (size_t ii{0}; ii < 100; ++ii) {
        BOOST_CHECK_EQUAL(4, 4);
      }
    }
  };

  auto write_func = [&async_set]() {
    for (size_t ii{0}; ii < 100; ++ii) {
      {
        auto write_proxy = async_set.write();
        write_proxy->insert(12);
        write_proxy->insert(13);
        write_proxy->insert(14);
        write_proxy->insert(15);
        write_proxy->erase(12);
        write_proxy->erase(13);
        write_proxy->erase(14);
        write_proxy->erase(15);
        auto cp = *write_proxy;
        *write_proxy = std::set<int>({1, 2, 3, 4, 5});
        *write_proxy = cp;
      }
      for (size_t ii{0}; ii < 100; ++ii) {
        BOOST_CHECK_EQUAL(4, async_set->size());
      }
    }
  };

  auto f1 = std::async(std::launch::async, read_func);
  auto f2 = std::async(std::launch::async, read_func);
  auto f3 = std::async(std::launch::async, write_func);

  f1.get();
  f2.get();
  f3.get();
}