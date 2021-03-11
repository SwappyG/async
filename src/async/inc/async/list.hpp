#ifndef NIL_SRC_THREADSAFE_INC_THREADSAFE_LIST_HPP_
#define NIL_SRC_THREADSAFE_INC_THREADSAFE_LIST_HPP_

#include <list>
#include <mutex>

#include "async/container_base.hpp"

namespace nil::async {

/**
 * Partially specified alias when using container_base with std::list,
 * std::mutex and std::lock_guard. Prefer this over container_base for list
 */
template <class T, class... Params>
using list =
    container_base<std::list<T, Params...>, std::mutex, std::lock_guard>;

}  // namespace nil::async

#endif  // NIL_SRC_THREADSAFE_INC_THREADSAFE_LIST_HPP_
