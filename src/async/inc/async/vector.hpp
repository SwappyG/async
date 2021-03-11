#ifndef NIL_SRC_ASYNC_INC_ASYNC_VECTOR_HPP_
#define NIL_SRC_ASYNC_INC_ASYNC_VECTOR_HPP_

#include <mutex>
#include <vector>

#include "async/container_base.hpp"

namespace nil::async {

/**
 * Partially specified alias when using container_base with std::vector,
 * std::mutex and std::lock_guard. Prefer this over container_base for vectors
 */
template <class T, class... Params>
using vector =
    container_base<std::vector<T, Params...>, std::mutex, std::lock_guard>;

}  // namespace nil::async

#endif  // NIL_SRC_ASYNC_INC_ASYNC_VECTOR_HPP_
