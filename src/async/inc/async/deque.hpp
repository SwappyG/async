#ifndef NIL_SRC_ASYNC_INC_ASYNC_DEQUE_HPP_
#define NIL_SRC_ASYNC_INC_ASYNC_DEQUE_HPP_

#include <deque>
#include <mutex>

#include "async/container_base.hpp"

namespace nil::async {

/**
 * Partially specified alias when using container_base with std::deque,
 * std::mutex and std::lock_guard. Prefer this over container_base for deques
 */
template <class T, class... Params>
using deque =
    container_base<std::deque<T, Params...>, std::mutex, std::lock_guard>;

}  // namespace nil::async

#endif  // NIL_SRC_ASYNC_INC_ASYNC_DEQUE_HPP_
