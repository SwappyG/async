#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE optional_test

#include "async/optional.hpp"

#include <boost/test/unit_test.hpp>
#include <future>
#include <iostream>
#include <numeric>

#include "async/atomic.hpp"

using namespace nil;

BOOST_AUTO_TEST_CASE(BasicTest) {
  async::optional<std::string> async_optional{std::in_place, "hello"};
  BOOST_CHECK_EQUAL(async_optional.peek().value(), "hello");
  BOOST_CHECK_EQUAL(async_optional.peek().value(), "hello");

  async_optional.push("yo");
  BOOST_CHECK_EQUAL(async_optional.peek().value(), "yo");

  const auto& const_atomic_str = async_optional;
  std::string str = "";
  const_atomic_str.apply([&str](const auto& a_str_opt) { str = (*a_str_opt); });
  BOOST_CHECK_EQUAL("yo", str);

  str = const_atomic_str.apply(
      [](const auto& a_str_opt) { return (*a_str_opt) + std::string{"yo"}; });
  BOOST_CHECK_EQUAL("yoyo", str);
  BOOST_CHECK_EQUAL("yo", async_optional.peek().value());

  str = async_optional.apply([](auto& a_str_opt) {
    (*a_str_opt) += std::string{"ho"};
    return *a_str_opt;
  });
  BOOST_CHECK_EQUAL("yoho", str);
  BOOST_CHECK_EQUAL("yoho", async_optional.peek().value());
}

BOOST_AUTO_TEST_CASE(PushPopTest) {
  async::optional<MoveOnly> async_opt{std::in_place};
  BOOST_CHECK(async_opt.pop() != std::nullopt);
  BOOST_CHECK(async_opt.pop() == std::nullopt);
  BOOST_CHECK(async_opt.pop() == std::nullopt);
  BOOST_CHECK(async_opt.pop() == std::nullopt);

  async_opt.push(std::nullopt);
  BOOST_CHECK(async_opt.pop() == std::nullopt);

  async_opt.push(std::optional<MoveOnly>{});
  BOOST_CHECK(async_opt.pop() == std::nullopt);

  async_opt.push(std::optional<MoveOnly>{MoveOnly{}});
  BOOST_CHECK(async_opt.pop() != std::nullopt);
}

BOOST_AUTO_TEST_CASE(MultithreadedTest) {
  nil::async::optional<int> counter{std::in_place, 0};
  nil::async::optional<std::vector<int>> async_optional_vec{std::in_place};
  const auto counter_max = 10000;

  auto func = [&]() {
    while (true) {
      auto next_val = counter.apply([](auto& ii_opt) {
        (*ii_opt)++;
        return (*ii_opt);
      });
      if (next_val > counter_max) {
        return;
      }
      async_optional_vec.apply(
          [&](auto& vec_opt) { vec_opt->push_back(next_val); });
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

  auto vec = async_optional_vec.peek().value();
  BOOST_CHECK_EQUAL(std::accumulate(vec.cbegin(), vec.cend(), 0),
                    counter_max * (counter_max + 1) / 2);
}

struct OptionalLikeNull {
  constexpr OptionalLikeNull(int) {}
};

inline constexpr auto optional_like_null = OptionalLikeNull{0};

template <class T>
struct OptionalLike {
  using value_type = T;
  union {
    T t_;
    bool b_;
  };

  OptionalLike() {}
  OptionalLike(OptionalLikeNull) {}

  template <class... Args>
  OptionalLike(std::in_place_t, Args&&...) {}

  OptionalLike& operator=(OptionalLikeNull) {}

  template <class U = T>
  OptionalLike& operator=(U&&) {}

  const T& value() const& { return t_; }
  const T&& value() const&& { return t_; }
  T& value() & { return t_; }
  T&& value() && { return t_; }

  void reset() const& {}

  explicit operator bool() { return true; }
};

BOOST_AUTO_TEST_CASE(OptionalLikeTest) {
  using OptType = async::optional_base<int, OptionalLike, OptionalLikeNull>;
  OptType op;
  op.peek();
  op.pop();
  op.push(optional_like_null);
  op.apply([](const auto&) { return 5; });
}