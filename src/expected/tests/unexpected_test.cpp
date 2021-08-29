#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE UnexpectedTest

#include <boost/test/unit_test.hpp>
#include <expected/unexpected.hpp>
#include <memory>
#include <meta/test_objects.hpp>

using namespace nil;

BOOST_AUTO_TEST_SUITE()

BOOST_AUTO_TEST_CASE(BasicTest) {
  unexpected<int> u{0};
  BOOST_REQUIRE_EQUAL(u.value(), 0);

  auto u2 = u;
  BOOST_REQUIRE_EQUAL(u2.value(), u.value());

  auto u3 = std::move(u2);
  BOOST_REQUIRE_EQUAL(u3.value(), u.value());
}

BOOST_AUTO_TEST_CASE(MoveTest) {
  {
    unexpected<test::move_bench> u{test::move_bench{}};
    auto move_bench = u.value();
    BOOST_REQUIRE(!u.value().IsMoved());
    BOOST_REQUIRE(!move_bench.IsMoved());

    auto move_bench_2 = std::move(u).value();
    BOOST_REQUIRE(!move_bench_2.IsMoved());
    BOOST_REQUIRE(u.value().IsMoved());
  }

  {
    unexpected<test::move_bench> u{test::move_bench{}};
    auto move_bench = std::move(u.value());
    BOOST_REQUIRE(!move_bench.IsMoved());
    BOOST_REQUIRE(u.value().IsMoved());
  }
}

BOOST_AUTO_TEST_SUITE_END()