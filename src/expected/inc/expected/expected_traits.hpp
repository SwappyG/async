#ifndef NIL_SRC_EXPECTED_INC_EXPECTEDTRAITS_HPP_
#define NIL_SRC_EXPECTED_INC_EXPECTEDTRAITS_HPP_

#include "expected/unexpected_traits.hpp"

namespace nil {

template <class V, class E>
class expected;

namespace detail {

template <class T, class V, class E, class Enable = void>
struct is_proper_value_type : std::false_type {};

template <class T, class V, class E>
struct is_proper_value_type<
    T, V, E,
    std::void_t<If<std::is_constructible_v<V, T&&> and        //
                   not std::is_void_v<V> and                  //
                   not std::is_same_v<T, expected<V, E>> and  //
                   not is_unexpected_v<T>>>> : public std::true_type {
  using type = T;
};  // namespace detail

template <class T, class V, class E>
inline constexpr auto is_proper_value_type_v = is_proper_value_type<T, V, E>::value;

template <class T, class V, class E>
using if_proper_value_type = If<is_proper_value_type<T, V, E>{}>;

template <class V, class E>
using if_expected_copyable = std::enable_if_t<                  //
    (std::is_void_v<V> || std::is_copy_constructible_v<V>)&&  //
    std::is_copy_constructible_v<E>>;

template <class V, class E>
using if_expected_movable = std::enable_if_t<                   //
    (std::is_void_v<V> || std::is_move_constructible_v<V>)&&  //
    std::is_move_constructible_v<E>>;

}  // namespace detail
}  // namespace nil

#endif  // NIL_SRC_EXPECTED_INC_EXPECTEDTRAITS_HPP_