/**
 * Concept is inspired by Louis Charles implementation of `Safe`:
 * https://github.com/LouisCharlesC/safe
 *
 * Small tweaks to API, but the concept is quite similar
 */

#ifndef NIL_SRC_ASYNC_INC_ASYNC_ATOMICRWBASE_HPP_
#define NIL_SRC_ASYNC_INC_ASYNC_ATOMICRWBASE_HPP_

#include <meta/enable_if.hpp>

#include "async/atomic_rw_proxy.hpp"

namespace nil {

/**
 * Wraps any type T with a reader-writer mutex. Provides read-only and write
 * access to the underlying type.
 *
 * The read or write proxies hold their lock RAII style, and release it upon
 * destruction.
 *
 * The provides terse, pointer-like symantics for reading. Writing requires a
 * slightly more verbose function call to mark it as a more expensive operation
 *
 * @note calling write() twice from the same thread within the same scope will
 * cause deadlock (or undefined behaviour)
 *
 * @tparam T - any copyable type
 * @tparam SharedMutex - a reader-writer mutex, like std::shared_mutex
 * @tparam WriteLock - an RAII unique lock, like std::unique_lock
 * @tparam ReadLock - an RAII shared lock, like std::shared_lock
 */
template <class T,                           //
          class SharedMutex,                 //
          template <class> class WriteLock,  //
          template <class> class ReadLock>
class atomic_rw_base {
  static_assert(std::is_copy_constructible_v<T>, "T must be copyable");

 public:
  using value_type = T;
  using mutex_type = SharedMutex;
  using write_lock = WriteLock<SharedMutex>;
  using read_lock = ReadLock<SharedMutex>;
  using read_proxy_t = atomic_r_proxy<T, mutex_type, ReadLock>;
  using write_proxy_t = atomic_rw_proxy<T, mutex_type, WriteLock>;

  // constructors --------------------------------------------------------------

  template <class U = T, if_default_constructible<U>* = nullptr>
  constexpr atomic_rw_base() {}

  template <class... Args>
  constexpr atomic_rw_base(Args&&... args) : t_{std::forward<Args>(args)...} {}

  // deleted copy and move constructors and assignment -------------------------

  atomic_rw_base(const atomic_rw_base&) = delete;
  atomic_rw_base& operator=(const atomic_rw_base&) = delete;
  atomic_rw_base(atomic_rw_base&&) = delete;
  atomic_rw_base& operator=(atomic_rw_base&&) = delete;

  // some convenience functions ------------------------------------------------

  T copy() const {
    read_lock lock{mutex_};
    return t_;
  }

  template <class... Args>
  void assign(Args&&... args) {
    write_lock lock{mutex_};
    t_ = {std::forward<Args>(args)...};
  }

  // get read proxy ------------------------------------------------------------

  auto operator*() const { return read_proxy_t{read_lock(mutex_), t_}; }
  auto operator->() const { return read_proxy_t{read_lock(mutex_), t_}; }
  auto read() const { return read_proxy_t{read_lock(mutex_), t_}; }

  // get write proxy -----------------------------------------------------------

  auto write() { return write_proxy_t{write_lock(mutex_), t_}; }

 private:
  mutable mutex_type mutex_;
  T t_;
};

}  // namespace nil

#endif  // NIL_SRC_ASYNC_INC_ASYNC_ATOMICRWBASE_HPP_
