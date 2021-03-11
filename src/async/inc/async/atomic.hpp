#ifndef NIL_SRC_ASYNC_INC_ASYNC_ATOMIC_HPP_
#define NIL_SRC_ASYNC_INC_ASYNC_ATOMIC_HPP_

#include <mutex>

#include "async/atomic_base.hpp"

namespace nil {

/**
 * Specialized for std::mutex with std::shared_lock. This is the expected
 * way for this utility to be used, but the base version can be used direcly if
 * using other mutex types (like boost)
 */
template <class T>
using atomic = atomic_base<T, std::mutex, std::lock_guard>;

}  // namespace nil

#endif  // NIL_SRC_ASYNC_INC_ASYNC_ATOMIC_HPP_
