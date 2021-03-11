#ifndef NIL_SRC_ASYNC_INC_ASYNC_CONTAINERTRAITS_HPP_
#define NIL_SRC_ASYNC_INC_ASYNC_CONTAINERTRAITS_HPP_

#include <meta/enable_if.hpp>

namespace nil::async {

template <class T, class S>
using at_func = decltype(std::declval<const T>().at(std::declval<S>()));

template <class T>
using front_func = decltype(std::declval<const T>().front());

template <class T>
using back_func = decltype(std::declval<const T>().back());

template <class T>
using top_func = decltype(std::declval<const T>().top());

template <class T, class V>
using push_func = decltype(std::declval<T>().push(std::declval<V>()));

template <class T, class V>
using push_back_func = decltype(std::declval<T>().push_back(std::declval<V>()));

template <class T, class V>
using push_front_func =
    decltype(std::declval<T>().push_front(std::declval<V>()));

template <class T, class... Args>
using emplace_back_func =
    decltype(std::declval<T>().emplace_back(std::declval<Args>()...));

template <class T, class... Args>
using emplace_front_func =
    decltype(std::declval<T>().emplace_front(std::declval<Args>()...));

template <class T>
using pop_func = decltype(std::declval<T>().pop());

template <class T>
using pop_back_func = decltype(std::declval<T>().pop_back());

template <class T>
using pop_front_func = decltype(std::declval<T>().pop_front());

template <class T, class I, class V>
using insert_func =
    decltype(std::declval<T>().insert(std::declval<I>(), std::declval<V>()));

template <class T, class I>
using erase_func = decltype(std::declval<T>().erase(std::declval<I>()));

template <class T>
using clear_func = decltype(std::declval<T>().clear());

template <class T>
using size_func = decltype(std::declval<const T>().size());

template <class T>
using empty_func = decltype(std::declval<const T>().empty());

// -----------------------------------------------------------------------------

template <class T,                           //
          class V = typename T::value_type,  //
          class S = typename T::size_type,   //
          class I = typename T::iterator>
using is_vector_like = std::conjunction<  //
    exists_similar<V, at_func, T, S>,     //
    exists_similar<V, front_func, T>,     //
    exists_similar<V, back_func, T>,      //
    exists<push_back_func, T, V>,         //
    exists<emplace_back_func, T, V>,      //
    exists<pop_back_func, T>,             //
    exists<insert_func, T, I, V>,         //
    exists<erase_func, T, I>,             //
    is_exact<S, size_func, T>,            //
    is_exact<bool, empty_func, T>,        //
    exists<clear_func, T>>;

template <class T,                           //
          class V = typename T::value_type,  //
          class S = typename T::size_type,   //
          class I = typename T::iterator>
inline constexpr auto is_vector_like_v = is_vector_like<T, V, S, I>::value;

template <class T,                           //
          class V = typename T::value_type,  //
          class S = typename T::size_type,   //
          class I = typename T::iterator>
using is_deque_like = std::conjunction<  //
    exists_similar<V, at_func, T, S>,    //
    exists_similar<V, front_func, T>,    //
    exists_similar<V, back_func, T>,     //
    exists<push_back_func, T, V>,        //
    exists<push_front_func, T, V>,       //
    exists<emplace_back_func, T, V>,     //
    exists<emplace_front_func, T, V>,    //
    exists<pop_back_func, T>,            //
    exists<pop_front_func, T>,           //
    exists<insert_func, T, I, V>,        //
    exists<erase_func, T, I>,            //
    is_exact<S, size_func, T>,           //
    is_exact<bool, empty_func, T>,       //
    exists<clear_func, T>>;

template <class T,                           //
          class V = typename T::value_type,  //
          class S = typename T::size_type,   //
          class I = typename T::iterator>
inline constexpr auto is_deque_like_v = is_deque_like<T, V, S, I>::value;

template <class T,                           //
          class V = typename T::value_type,  //
          class S = typename T::size_type,   //
          class I = typename T::iterator>
using is_list_like = std::conjunction<exists_similar<V, front_func, T>,  //
                                      exists_similar<V, back_func, T>,   //
                                      exists<push_back_func, T, V>,      //
                                      exists<push_front_func, T, V>,     //
                                      exists<emplace_back_func, T, V>,   //
                                      exists<emplace_front_func, T, V>,  //
                                      exists<pop_back_func, T>,          //
                                      exists<pop_back_func, T>,          //
                                      exists<insert_func, T, I, V>,      //
                                      exists<erase_func, T, I>,          //
                                      is_exact<S, size_func, T>,         //
                                      is_exact<bool, empty_func, T>,     //
                                      exists<clear_func, T>>;

template <class T,                           //
          class V = typename T::value_type,  //
          class S = typename T::size_type,   //
          class I = typename T::iterator>
inline constexpr auto is_list_like_v = is_list_like<T, V, S, I>::value;

}  // namespace nil::async

#endif  // NIL_SRC_ASYNC_INC_ASYNC_CONTAINERTRAITS_HPP_
