#ifndef NIL_SRC_META_INC_META_ENABLEIF_HPP_
#define NIL_SRC_META_INC_META_ENABLEIF_HPP_

#include <type_traits>
#include <utility>

#include "meta/detector.hpp"

namespace nil {

// more readible SFINAE, just wraps STL traits ---------------------------------

template <class T, class U = void>
using if_void = If<std::is_void_v<T>, U>;

template <class T, class U = void>
using if_not_void = If<!std::is_void_v<T>, U>;

template <class T, class U = void>
using if_default_constructible = If<std::is_default_constructible_v<T>, U>;

template <class T, class U = void>
using if_not_default_constructible = If<!std::is_default_constructible_v<T>, U>;

template <class T, class U = void>
using if_copy_constructible = If<std::is_copy_constructible_v<T>, U>;

template <class T, class U = void>
using if_not_copy_constructible = If<!std::is_copy_constructible_v<T>, U>;

template <class T, class U = void>
using if_move_constructible = If<std::is_move_constructible_v<T>, U>;

template <class T, class U = void>
using if_not_move_constructible = If<!std::is_move_constructible_v<T>, U>;

template <class T, class U = void>
using if_copy_assignable = If<std::is_copy_assignable_v<T>, U>;

template <class T, class U = void>
using if_not_copy_assignable = If<!std::is_copy_assignable_v<T>, U>;

template <class T, class U = void>
using if_move_assignable = If<std::is_move_assignable_v<T>, U>;

template <class T, class U = void>
using if_not_move_assignable = If<!std::is_move_assignable_v<T>, U>;

template <class T, class... Args>
using if_constructible = If<std::is_constructible_v<T, Args...>>;

template <class T, class... Args>
using if_not_constructible = If<!std::is_constructible_v<T, Args...>>;

template <class T, class U, class V = void>
using if_assignable = If<std::is_assignable_v<T, U>, V>;

template <class T, class U, class V = void>
using if_not_assignable = If<!std::is_assignable_v<T, U>, V>;

template <class T, class U, class V = void>
using if_same = If<std::is_same_v<T, U>, V>;

template <class T, class U, class V = void>
using if_not_same = If<!std::is_same_v<T, U>, V>;

template <class Fn, class... Args>
using if_invocable = If<std::is_invocable_v<Fn, Args...>>;

template <class Fn, class... Args>
using if_not_invocable = If<!std::is_invocable_v<Fn, Args...>>;

template <class R, class Fn, class... Args>
using if_invocable_r = If<std::is_invocable_r_v<R, Fn, Args...>>;

template <class R, class Fn, class... Args>
using if_not_invocable_r = If<!std::is_invocable_r_v<R, Fn, Args...>>;

template <class Derv, class Base>
using if_convertible = If<std::is_convertible<Derv, Base>{}>;

template <class Derv, class Base>
using if_not_convertible = If<!std::is_convertible<Derv, Base>{}>;

// New traits, not in STL ------------------------------------------------------

template <class T>
using is_in_place = std::is_same<std::in_place_t, std::decay_t<T>>;

template <class T>
inline constexpr auto is_in_place_v = is_in_place<T>::value;

template <class T, class U>
using is_similar = std::is_same<std::decay_t<T>, std::decay_t<U>>;

template <class T, class U>
inline constexpr auto is_similar_v = is_similar<T, U>::value;

template <class T, class U, class V = void>
using if_similar = If<is_similar_v<T, U>, V>;

template <class T, class U, class V = void>
using if_not_similar = If<!is_similar_v<T, U>, V>;

}  // namespace nil

#endif  // NIL_SRC_META_INC_META_ENABLEIF_HPP_