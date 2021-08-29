#ifndef NIL_SRC_EXPECTED_INC_UNEXPECTEDTRAITS_HPP_
#define NIL_SRC_EXPECTED_INC_UNEXPECTEDTRAITS_HPP_

#include <type_traits>

#include <meta/detector.hpp>>
#include <meta/enable_if.hpp>>

namespace nil {

template <class E>
class unexpected;

namespace detail {

struct unexpected_tag {};

template <class T>
using get_unexpected_tag = typename T::tag;

template <class T>
using is_unexpected_t = is_exact<unexpected_tag, get_unexpected_tag, T>;

template <class T>
inline constexpr auto is_unexpected_v = is_unexpected_t<T>::value;

}  // namespace detail
}  // namespace nil

#endif  // NIL_SRC_EXPECTED_INC_UNEXPECTEDTRAITS_HPP_