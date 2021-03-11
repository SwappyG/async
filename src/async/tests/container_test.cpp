#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE container_test

#include <boost/mpl/list.hpp>
#include <boost/test/unit_test.hpp>
#include <future>
#include <meta/none_such.hpp>
#include <numeric>

#include "async/deque.hpp"
#include "async/list.hpp"
#include "async/vector.hpp"

using namespace nil;
using namespace nil::async;

template <class CT, class V = typename CT::value_type>
void VerifyAt(const CT& c, const V& front, const V& back, const size_t& size,
              const typename CT::container_type& elements) {
  BOOST_CHECK_EQUAL(size, c.size());

  if (size == 0) {
    BOOST_CHECK(c.empty());
  } else {
    BOOST_CHECK_EQUAL(front, c.front().value());
    BOOST_CHECK_EQUAL(back, c.back().value());
    BOOST_CHECK(!c.empty());
  }

  if constexpr (CT::is_vector_like || CT::is_deque_like) {
    for (size_t ii{0}; ii < size; ii++) {
      BOOST_CHECK_EQUAL(elements.at(ii), c.at(ii).value());
    }
  }
}

template <class CT>
void VerifySize(const CT& c, typename CT::size_type expected_size) {
  BOOST_CHECK_EQUAL(c.size(), expected_size);
  BOOST_CHECK_EQUAL(c.empty(), (expected_size == 0));
}

using IntTypes = boost::mpl::list<vector<int>, deque<int>, list<int>>;
using StrTypes = boost::mpl::list<vector<std::string>, deque<std::string>,
                                  list<std::string>>;

BOOST_AUTO_TEST_CASE_TEMPLATE(ConstructAssignTest, CT, StrTypes) {
  using container_type = typename CT::container_type;
  CT async_vec;  // default construct
  VerifySize(async_vec, 0);

  async_vec.assign(container_type{"1", "2", "4"});
  VerifySize(async_vec, 3);

  async_vec.assign({"1", "9", "4", "7"});
  VerifySize(async_vec, 4);

  async_vec.assign(std::in_place, "7", "4");
  VerifySize(async_vec, 2);

  async_vec.clear();
  VerifySize(async_vec, 0);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(AtTest, CT, StrTypes) {
  using container_type = typename CT::container_type;
  CT async_vec;
  VerifyAt(async_vec, {}, {}, 0, {});

  async_vec.push_back("1");
  VerifyAt(async_vec, "1", "1", 1, {"1"});

  if constexpr (CT::is_vector_like || CT::is_deque_like) {
    auto elem_0_copy = async_vec.at(0).value();
    elem_0_copy = "2";  // shouldn't modify original
    VerifyAt(async_vec, "1", "1", 1, {"1"});
  }

  async_vec.clear();
  VerifyAt(async_vec, {}, {}, 0, {});
}

BOOST_AUTO_TEST_CASE_TEMPLATE(PushPopBackTest, CT, StrTypes) {
  using container_type = typename CT::container_type;
  CT async_vec;
  VerifySize(async_vec, 0);

  async_vec.push_back("3");
  async_vec.push_back("4");
  async_vec.push_back("5");
  VerifyAt(async_vec, "3", "5", 3, {"3", "4", "5"});

  async_vec.pop_back();
  async_vec.pop_back();
  VerifyAt(async_vec, "3", "3", 1, {"3"});

  if constexpr (CT::is_deque_like || CT::is_list_like) {
    async_vec.push_front("1");
    async_vec.push_front("2");
    async_vec.push_front("7");
    async_vec.pop_front();
    VerifyAt(async_vec, "2", "3", 3, {"2", "1", "3"});
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ExtractTest, CT, StrTypes) {
  using container_type = typename CT::container_type;
  CT async_vec{std::in_place, "1", "2", "3"};
  VerifySize(async_vec, 3);

  if constexpr (CT::is_vector_like || CT::is_deque_like) {
    auto str_opt = async_vec.extract(0);
    BOOST_CHECK(str_opt != std::nullopt);
    BOOST_CHECK(str_opt.value() == "1");
    VerifyAt(async_vec, "2", "3", 2, {"2", "3"});
  }
  async_vec.assign(std::in_place, "2", "3");

  auto str_opt = async_vec.extract_back();
  BOOST_CHECK(str_opt != std::nullopt);
  BOOST_CHECK(str_opt.value() == "3");
  VerifyAt(async_vec, "2", "2", 1, {"2"});

  if constexpr (CT::is_vector_like || CT::is_deque_like) {
    auto str_opt = async_vec.extract(0);
    BOOST_CHECK(str_opt != std::nullopt);
    BOOST_CHECK(str_opt.value() == "2");
    VerifyAt(async_vec, {}, {}, 0, {});
    str_opt = async_vec.extract(0);
    BOOST_CHECK(str_opt == std::nullopt);
  }
  async_vec.clear();
  str_opt = async_vec.extract_back();
  BOOST_CHECK(str_opt == std::nullopt);

  if constexpr (CT::is_deque_like || CT::is_list_like) {
    async_vec.assign(std::in_place, "1", "2", "3");
    auto str_opt = async_vec.extract_front();
    BOOST_CHECK(str_opt != std::nullopt);
    BOOST_CHECK(str_opt.value() == "1");
    VerifyAt(async_vec, "2", "3", 2, {"2", "3"});
    async_vec.clear();
    str_opt = async_vec.extract_front();
    BOOST_CHECK(str_opt == std::nullopt);
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(EraseTest, CT, StrTypes) {
  using container_type = typename CT::container_type;
  CT async_vec{std::in_place, "1", "2", "3"};

  async_vec.erase(1);
  VerifyAt(async_vec, "1", "3", 2, {"1", "3"});

  async_vec.erase(1);
  VerifyAt(async_vec, "1", "1", 1, {"1"});

  async_vec.erase(1);
  VerifyAt(async_vec, "1", "1", 1, {"1"});
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ApplyTest, CT, StrTypes) {
  using container_type = typename CT::container_type;
  CT async_vec{std::in_place, "1", "2", "3"};

  const auto& const_async_vec = async_vec;
  auto size =
      const_async_vec.apply([](const container_type& c) { return c.size(); });
  BOOST_CHECK(size == const_async_vec.size());

  async_vec.pop_back();
  const_async_vec.apply([&size](const container_type& c) { size = c.size(); });
  BOOST_CHECK(size == const_async_vec.size());

  auto ret = async_vec.apply([](container_type& c) {
    c.push_back("4");
    return 5;
  });
  BOOST_CHECK_EQUAL("4", async_vec.back().value());
  BOOST_CHECK_EQUAL(5, ret);

  size = 0;
  const_async_vec.apply_each([&size](const auto&) { size++; });
  BOOST_CHECK_EQUAL(size, const_async_vec.size());

  async_vec.apply_each([](std::string& v) { v = "0"; });
  auto non_async_c = async_vec.apply([](const auto& c) { return c; });
  for (const auto& ii : non_async_c) {
    BOOST_CHECK_EQUAL(ii, "0");
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(PushExtractMultithreadTest, CT, IntTypes) {
  using container_type = typename CT::container_type;
  CT async_vec;

  const auto num_elements = 10000;

  container_type c_in;
  int total = 0;
  for (size_t ii{0}; ii < num_elements; ii++) {
    c_in.push_back(ii);
    total += ii;
  }

  container_type c_out;

  auto f1 = std::async(std::launch::async, [&]() {
    for (const auto& ii : c_in) {
      async_vec.push_back(ii);
    }
  });

  auto f2 = std::async(std::launch::async, [&]() {
    while (c_out.size() != num_elements) {
      auto opt = async_vec.extract_back();
      if (opt != std::nullopt) {
        c_out.push_back(opt.value());
      }
    }
  });

  auto f3 = std::async(std::launch::async, [&]() -> bool {
    while (f2.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
      if constexpr (CT::is_vector_like || CT::is_deque_like) {
        if (async_vec.at(0).value_or(0) >= num_elements) {
          return false;
        }
      }
      if (async_vec.front().value_or(0) >= num_elements) {
        return false;
      }
      if (async_vec.back().value_or(0) >= num_elements) {
        return false;
      }
    }
    return true;
  });

  f1.get();
  f2.get();
  BOOST_CHECK(f3.get());
  BOOST_CHECK_EQUAL(c_in.size(), c_out.size());
  BOOST_CHECK_EQUAL(total, std::accumulate(c_out.cbegin(), c_out.cend(), 0));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ApplyMultiThreadTest, CT, IntTypes) {
  using container_type = typename CT::container_type;
  CT async_vec{std::in_place, 0, 10, 100};
  auto func = [&]() {
    for (size_t ii{0}; ii < 1000; ii++) {
      async_vec.apply_each([](int& ii) { ii++; });
    }
  };

  std::vector<std::future<void>> v_f;
  for (size_t ii{0}; ii < 10; ii++) {
    v_f.push_back(std::async(std::launch::async, func));
  }

  for (auto& f : v_f) {
    f.get();
  }

  BOOST_CHECK_EQUAL(1000 * 10 + 100, async_vec.extract_back().value());
  BOOST_CHECK_EQUAL(1000 * 10 + 10, async_vec.extract_back().value());
  BOOST_CHECK_EQUAL(1000 * 10, async_vec.extract_back().value());
}

using MoveOnlyTypes =
    boost::mpl::list<vector<MoveOnly>, deque<MoveOnly>, list<MoveOnly>>;

BOOST_AUTO_TEST_CASE_TEMPLATE(MoveOnlyTypeTest, CT, MoveOnlyTypes) {
  using container_type = typename CT::container_type;
  CT async_vec{std::in_place, MoveOnly{}, MoveOnly{}, MoveOnly{}};
  VerifySize(async_vec, 3);

  auto move_only_opt = async_vec.extract_back();
  BOOST_CHECK(move_only_opt != std::nullopt);
  VerifySize(async_vec, 2);

  move_only_opt = [&]() {
    if constexpr (CT::is_vector_like) {
      return async_vec.extract(0);
    } else {
      return async_vec.extract_front();
    }
  }();

  BOOST_CHECK(move_only_opt != std::nullopt);
  VerifySize(async_vec, 1);

  async_vec.push_back(MoveOnly{});
  VerifySize(async_vec, 2);

  async_vec.push_back({});
  VerifySize(async_vec, 3);

  async_vec.assign(std::in_place, MoveOnly{}, MoveOnly{});
  VerifySize(async_vec, 2);

  container_type underlying;
  underlying.emplace_back(MoveOnly{});
  async_vec.assign(std::move(underlying));
  VerifySize(async_vec, 1);
}
