#ifndef NIL_SRC_THREADSAFE_INC_THREADSAFE_RWATOMIC_HPP_
#define NIL_SRC_THREADSAFE_INC_THREADSAFE_RWATOMIC_HPP_

#include <shared_mutex>

#include "async/atomic_rw_base.hpp"

namespace nil {

/**
 * Specialized for std::shared_mutex with std::shared_lock. This is the expected
 * way for this utility to be used, but the base version can be used direcly if
 * using other mutex types (like boost, or std::shared_timed_mutex)
 */
template <class T>
using atomic_rw =
    atomic_rw_base<T, std::shared_mutex, std::unique_lock, std::shared_lock>;

}  // namespace nil

#endif  // NIL_SRC_THREADSAFE_INC_THREADSAFE_RWATOMIC_HPP_
