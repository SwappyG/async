#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE ExpectedTest

#include <boost/test/unit_test.hpp>
#include <meta/test_objects.hpp>
#include <variant>

#include "expected/expected.hpp"

using namespace nil;

BOOST_AUTO_TEST_SUITE()

BOOST_AUTO_TEST_CASE(BasicTest) {
  auto maybe_int = [](bool return_value, int v, int e) -> expected<int, int> {
    if (return_value) {
      return v;
    }
    return MakeUnexpected(e);
  };

  auto exp_int = maybe_int(true, 7, 8);
  BOOST_CHECK((exp_int.has_v()));
  BOOST_CHECK((not exp_int.has_e()));
  BOOST_CHECK((exp_int.value() == 7));
  BOOST_CHECK((*exp_int == 7));

  exp_int = maybe_int(false, 7, 8);
  BOOST_CHECK((not exp_int.has_v()));
  BOOST_CHECK((exp_int.has_e()));
  BOOST_CHECK((exp_int.error() == 8));

  auto exp_int_copy = exp_int;
  BOOST_CHECK((not exp_int_copy.has_v()));
  BOOST_CHECK((exp_int_copy.has_e()));
  BOOST_CHECK((exp_int_copy.error() == 8));

  auto exp_int_move = std::move(exp_int_copy);
  BOOST_CHECK((not exp_int_move.has_v()));
  BOOST_CHECK((exp_int_move.has_e()));
  BOOST_CHECK((exp_int_move.error() == 8));
}

BOOST_AUTO_TEST_CASE(VoidValueTest, *boost::unit_test::disabled()) {
  auto exp = expected<void, int>{};
  BOOST_CHECK(exp.has_v());
  BOOST_CHECK(!exp.has_e());

  exp = unexpected<int>{6};
  BOOST_CHECK(!exp.has_v());
  BOOST_CHECK(exp.has_e());
  BOOST_CHECK_EQUAL(exp.error(), 6);

  BOOST_CHECK_EQUAL(exp.as_unexpected().value(), 6);
}

BOOST_AUTO_TEST_CASE(MovedTest, *boost::unit_test::disabled()) {
  auto exp = expected<test::move_bench, test::move_bench>{test::move_bench{}};
  BOOST_CHECK(exp.has_v());
  BOOST_CHECK(!exp.has_e());

  auto v = exp.value();
  BOOST_CHECK(!v.IsMoved());
  BOOST_CHECK(!exp.value().IsMoved());

  v = std::move(exp).value();
  BOOST_CHECK(!v.IsMoved());
  BOOST_CHECK(exp.value().IsMoved());

  exp = expected<test::move_bench, test::move_bench>{test::move_bench{}};
  BOOST_CHECK(!v.IsMoved());
  BOOST_CHECK(!exp.value().IsMoved());
}

BOOST_AUTO_TEST_CASE(AssignmentTest) {
  auto exp = expected<int, bool>{5};
  BOOST_CHECK(exp.has_v() && !exp.has_e());
  BOOST_CHECK_EQUAL(*exp, 5);
  exp = 7;
  BOOST_CHECK_EQUAL(*exp, 7);

  exp = MakeUnexpected(false);
  BOOST_CHECK(!exp.has_v() && exp.has_e());
  BOOST_CHECK_EQUAL(exp.error(), false);

  exp = decltype(exp){8};
  BOOST_CHECK(exp.has_v() && !exp.has_e());
  BOOST_CHECK_EQUAL(*exp, 8);

  exp = decltype(exp){MakeUnexpected(true)};
  BOOST_CHECK(!exp.has_v() && exp.has_e());
  BOOST_CHECK_EQUAL(exp.error(), true);

  auto exp_2 = decltype(exp){10};
  exp = exp_2;
  BOOST_CHECK(exp.has_v() && !exp.has_e());
  BOOST_CHECK_EQUAL(*exp, 10);

  auto exp_3 = std::move(exp);
  BOOST_CHECK(exp_3.has_v() && !exp.has_e());
  BOOST_CHECK_EQUAL(*exp_3, 10);
}

BOOST_AUTO_TEST_CASE(AssignmentComplexTypeTest) {
  auto make_ptr = [](auto ii) {
    return std::make_unique<std::decay_t<decltype(ii)>>(ii);
  };

  auto exp = expected<std::unique_ptr<int>, std::unique_ptr<bool>>{make_ptr(5)};
  BOOST_CHECK(exp.has_v() && !exp.has_e());
  BOOST_CHECK_EQUAL(*(exp.value()), 5);
  exp = make_ptr(7);
  BOOST_CHECK_EQUAL(*(exp.value()), 7);

  exp = MakeUnexpected(make_ptr(false));
  BOOST_CHECK(!exp.has_v() && exp.has_e());
  BOOST_CHECK_EQUAL(*(exp.error()), false);

  exp = decltype(exp){make_ptr(8)};
  BOOST_CHECK(exp.has_v() && !exp.has_e());
  BOOST_CHECK_EQUAL(*(exp.value()), 8);

  exp = decltype(exp){MakeUnexpected(make_ptr(true))};
  BOOST_CHECK(!exp.has_v() && exp.has_e());
  BOOST_CHECK_EQUAL(*(exp.error()), true);

  auto exp_2 = decltype(exp){make_ptr(10)};
  exp = std::move(exp_2);
  BOOST_CHECK(exp.has_v() && !exp.has_e());
  BOOST_CHECK_EQUAL(*(exp.value()), 10);
}

BOOST_AUTO_TEST_CASE(MoveOnlyForValueTest) {
  using Exp = expected<std::unique_ptr<int>, std::unique_ptr<std::string>>;

  {
    auto exp = Exp(std::make_unique<int>(5));

    BOOST_CHECK(exp.has_v());
    BOOST_CHECK(!exp.has_e());

    const auto& val_const_ref = exp.value();
    BOOST_CHECK_EQUAL(*val_const_ref, 5);

    auto val = std::move(exp).value();
    BOOST_CHECK_EQUAL(*val, 5);
  }

  {
    auto exp = Exp(std::make_unique<int>(5));
    auto& val_ref = exp.value();
    // val_ref = std::make_unique<int>(7);  // doesn't compile, V is const
    *val_ref = 7;  // compiles! the type inside unique_ptr isn't const!
    BOOST_CHECK_EQUAL(*val_ref, 7);
  }
}

BOOST_AUTO_TEST_CASE(MoveOnlyForErrorTest) {
  using Exp = expected<std::unique_ptr<int>, std::unique_ptr<std::string>>;
  {
    auto exp = Exp{MakeUnexpected(std::make_unique<std::string>("yo"))};

    BOOST_CHECK(!exp.has_v());
    BOOST_CHECK(exp.has_e());

    const auto& err_const_ref = exp.error();
    BOOST_CHECK_EQUAL(*err_const_ref, "yo");

    auto err = std::move(exp).error();
    BOOST_CHECK_EQUAL(*err, "yo");
  }

  {
    auto exp = Exp{MakeUnexpected(std::make_unique<std::string>("yo"))};
    auto& err_ref = exp.error();
    // err_ref = std::make_unique<std::string>("yo");  // doesn't compile, const
    *err_ref = "yoyo";  // compiles! the type inside unique_ptr isn't const!
    BOOST_CHECK_EQUAL(*err_ref, "yoyo");
  }
}

BOOST_AUTO_TEST_CASE(MoveOnlyForUnexpectedTest) {
  using Exp = expected<std::unique_ptr<int>, std::unique_ptr<std::string>>;
  {
    auto exp = Exp{MakeUnexpected(std::make_unique<std::string>("yo"))};

    BOOST_CHECK(!exp.has_v());
    BOOST_CHECK(exp.has_e());

    const auto& unexp_const_ref = exp.as_unexpected();
    BOOST_CHECK_EQUAL(*(unexp_const_ref.value()), "yo");

    auto unexp = std::move(exp).as_unexpected();
    BOOST_CHECK_EQUAL(*(unexp.value()), "yo");
  }

  {
    auto exp = Exp{MakeUnexpected(std::make_unique<std::string>("yo"))};
    auto& unexp_ref = exp.as_unexpected();
    // err_ref = std::make_unique<std::string>("yo");  // doesn't compile, const
    *(unexp_ref.value()) = "yoyo";  // compiles! type in unique_ptr is non-const
    BOOST_CHECK_EQUAL(*(unexp_ref.value()), "yoyo");
  }
}

BOOST_AUTO_TEST_SUITE_END()