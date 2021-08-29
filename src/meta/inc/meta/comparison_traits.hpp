#ifndef NIL_SRC_META_INC_META_COMPARISONTRAITS_HPP_
#define NIL_SRC_META_INC_META_COMPARISONTRAITS_HPP_

#include <type_traits>

#include "meta/detector.hpp"
#include "meta/enable_if.hpp"

namespace nil {

template <class T, class Operator>
using comp_func_r =
    decltype(std::declval<Operator>()(std::declval<T>(), std::declval<T>()));

template <class T, class Operator>
using comparison_check = IsExact<bool, comp_func_r, T, Operator>;

// less than -------------------------------------------------------------------

template <class T>
using less_than_r = decltype(std::invoke_result<std::less<>, T, T>);

template <class T>
using less_than_eq_r = decltype(std::invoke_result<std::less_equal<>, T, T>);

template <class T>
using greater_than_r = decltype(std::invoke_result<std::greater<>, T, T>);

template <class T>
using greater_than_eq_r =
    decltype(std::invoke_result<std::greater_equal<>, T, T>);

template <class T>
using equal_to_r = decltype(std::invoke_result<std::equal_to<>, T, T>);

template <class T>
using not_equal_to_r = decltype(std::invoke_result<std::not_equal_to<>, T, T>);

template <class T>
using if_has_less_than = IsExact<bool, less_than_r, T>;

template <class T>
using if_has_less_than_eq = IsExact<bool, less_than_eq_r, T>;

template <class T>
using if_has_greater = IsExact<bool, greater_than_r, T>;

template <class T>
using if_has_greater_eq = IsExact<bool, greater_than_eq_r, T>;

template <class T>
using if_has_equal_to = IsExact<bool, equal_to_r, T>;

template <class T>
using if_has_not_equal_to = IsExact<bool, not_equal_to_r, T>;

// -----------------------------------------------------------------------------

template <class T>
using is_less_than_comparable = IsExact<bool, comp_func_r, T, std::less<>>;

template <class T>
using if_less_than_comparable = If<is_less_than_comparable<T>{}>;

template <class T>
using if_not_less_than_comparable = If<not is_less_than_comparable<T>{}>;

// less than equal -------------------------------------------------------------

template <class T>
using is_less_than_eq_comparable =
    IsExact<bool, comp_func_r, T, std::less_equal<>>;

template <class T>
using if_less_than_eq_comparable = If<is_less_than_eq_comparable<T>{}>;

template <class T>
using if_not_less_than_eq_comparable =
    If<not is_less_than_eq_comparable<T>::value>;

// greater than ----------------------------------------------------------------

template <class T>
using is_greater_than_comparable = IsExact<bool, comp_func_r, T, std::greater<>>;

template <class T>
using if_greater_than_comparable = If<is_greater_than_comparable<T>{}>;

template <class T>
using if_not_greater_than_comparable = If<not is_greater_than_comparable<T>{}>;

// greater than equal ----------------------------------------------------------

template <class T>
using is_greater_than_eq_comparable =
    IsExact<bool, comp_func_r, T, std::greater_equal<>>;

template <class T>
using if_greater_than_eq_comparable = If<is_greater_than_eq_comparable<T>{}>;

template <class T>
using if_not_greater_than_eq_comparable = If<not is_greater_than_eq_comparable<T>{}>;

// equality --------------------------------------------------------------------

template <class T>
using is_equality_comparable = is_exact<bool, comp_func_r, T, std::equal_to<>>;

template <class T>
using if_equality_comparable = If<is_equality_comparable<T>{}>;

template <class T>
using if_not_equality_comparable = If<not is_equality_comparable<T>{}>;

// not equality ----------------------------------------------------------------

template <class T>
using is_notequal_comparable = IsExact<bool, comp_func_r, T, std::not_equal_to<>>;

template <class T>
using if_notequal_comparable = If<is_notequal_comparable<T>{}>;

template <class T>
using if_not_notequal_comparable = If<not is_notequal_comparable<T>{}>;

// all comparison --------------------------------------------------------------

template <class T>
using is_fully_comparable =
    all_exist<T, is_less_than_comparable, is_less_than_eq_comparable,
              is_greater_than_comparable, if_greater_than_eq_comparable,
              is_equality_comparable, is_notequal_comparable>;

template <class T>
using if_fully_comparable = If<is_fully_comparable<T>{}>;

template <class T>
using if_not_fully_comparable = If<not is_fully_comparable<T>{}>;

}  // namespace nil

#endif  // NIL_SRC_META_INC_META_COMPARISONTRAITS_HPP_