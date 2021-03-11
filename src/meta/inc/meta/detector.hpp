/**
 * Inspired from CppNow Talk by Marshall Clow, published to youtube on
 * Jun 6, 2017
 *
 * Link: https://www.youtube.com/watch?v=U3jGdnRL3KI
 */

#ifndef NIL_SRC_META_INC_META_DETECTOR_HPP_
#define NIL_SRC_META_INC_META_DETECTOR_HPP_

#include "meta/identity.hpp"
#include "meta/none_such.hpp"

namespace nil {

template <bool predicate, class RetType = void>
using If = std::enable_if_t<predicate, RetType>;

template <class... T>
using if_all = If<std::conjunction<T...>::value>;

template <class Default, class, template <class...> class, class...>
struct detector : public std::false_type {
  static constexpr auto value = std::false_type::value;
  using type = Default;
  operator bool() { return value; }
};

template <class Default, template <class...> class MetaFunc, class... Args>
struct detector<Default, std::void_t<MetaFunc<Args...>>, MetaFunc, Args...>
    : public std::true_type {
  static constexpr auto value = std::true_type::value;
  using type = MetaFunc<Args...>;
  operator bool() { return value; }
};

template <template <class...> class MetaFunc, class... Args>
using exists = detector<none_such, void, MetaFunc, Args...>;

template <template <class...> class MetaFunc, class... Args>
using exists_t = typename detector<none_such, void, MetaFunc, Args...>::type;

template <template <class...> class MetaFunc, class... Args>
inline constexpr auto exists_v = exists<MetaFunc, Args...>::value;

template <template <class...> class MetaFunc, class... Args>
using if_exists = std::enable_if_t<exists_v<MetaFunc, Args...>>;

template <template <class...> class MetaFunc, class... Args>
using if_not_exists = std::enable_if_t<not exists_v<MetaFunc, Args...>>;

template <class ExpectedT, template <class...> class MetaFunc, class... Args>
using is_exact = std::is_same<exists_t<MetaFunc, Args...>, ExpectedT>;

template <class ExpectedT, template <class...> class MetaFunc, class... Args>
inline constexpr auto is_exact_v = is_exact<ExpectedT, MetaFunc, Args...>{};

template <class ExpectedT, template <class...> class MetaFunc, class... Args>
using if_exact = std::enable_if_t<is_exact_v<ExpectedT, MetaFunc, Args...>>;

template <class ExpectedT, template <class...> class MetaFunc, class... Args>
using if_not_exact =
    std::enable_if_t<not is_exact_v<ExpectedT, MetaFunc, Args...>>;

template <class ExpectedT, template <class...> class MetaFunc, class... Args>
using exists_similar =
    std::is_same<ExpectedT, std::decay_t<exists_t<MetaFunc, Args...>>>;

template <class ExpectedT, template <class...> class MetaFunc, class... Args>
inline constexpr auto exists_similar_v =
    exists_similar<ExpectedT, MetaFunc, Args...>{};

template <class ExpectedT, template <class...> class MetaFunc, class... Args>
using if_exists_similar = If<exists_similar_v<ExpectedT, MetaFunc, Args...>>;

template <class ExpectedT, template <class...> class MetaFunc, class... Args>
using if_not_exists_similar =
    If<not exists_similar_v<ExpectedT, MetaFunc, Args...>>;

template <class T, template <class> class... Ops>
using all_exist = std::conjunction<exists_t<Ops, T>...>;

template <class T, template <class> class... Ops>
using any_exist = std::disjunction<exists_t<Ops, T>...>;

template <class T, template <class> class... Ops>
using all_not_exist = std::negation<any_exist<T, Ops...>>;

template <class T, template <class> class... Ops>
using if_all_exist = If<all_exist<T, Ops...>{}>;

template <class T, template <class> class... Ops>
using if_any_exist = If<any_exist<T, Ops...>{}>;

template <class T, template <class> class... Ops>
using if_all_not_exist = If<all_not_exist<T, Ops...>{}>;

}  // namespace nil

#endif  // NIL_SRC_META_INC_META_DETECTOR_HPP_