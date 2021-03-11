#ifndef NIL_SRC_ASYNC_INC_ASYNC_ATOMICBASE_HPP_
#define NIL_SRC_ASYNC_INC_ASYNC_ATOMICBASE_HPP_

#include <functional>
#include <meta/enable_if.hpp>

namespace nil {

/**
 * Base class for atomic, templated on some T + mutex and lock types
 *
 * This class wraps any type with a mutex and ensures all operations on this
 * class are thread-safe.
 *
 * In addition to the standard push and peek, apply takes any function, allowing
 * you to perform complex operations while the mutex is locked
 *
 * @tparam T - any copyable type
 * @tparam Mutex - a standard mutex, like std::mutex
 * @tparam LockGuard - an RAII lock, like std::lock_guard
 */
template <class T, class Mutex, template <class> class LockGuard>
class atomic_base {
  static_assert(std::is_copy_constructible_v<T>, "T must be copyable");

 public:
  using value_type = T;
  using mutex_type = Mutex;
  using lock_type_t = LockGuard<Mutex>;

  // constructors --------------------------------------------------------------

  template <class U = T, if_default_constructible<U>* = nullptr>
  constexpr atomic_base() {}

  template <class... Args>
  constexpr atomic_base(Args&&... args) : t_{std::forward<Args>(args)...} {}

  // deleted copy and move constructors and assignment -------------------------

  atomic_base(const atomic_base&) = delete;
  atomic_base& operator=(const atomic_base&) = delete;
  atomic_base(atomic_base&&) = delete;
  atomic_base& operator=(atomic_base&&) = delete;

  // get a copy of data --------------------------------------------------------

  T peek() const {
    lock_type_t lock(mutex_);
    return t_;
  }

  // mutate data ---------------------------------------------------------------

  template <class U = T, class = if_assignable<T, U&&>>
  void push(U&& u) {
    lock_type_t lock(mutex_);
    t_ = std::forward<U>(u);
  }

  // execute arbitary function on data -----------------------------------------

  template <class F>
  auto apply(F&& f) const {
    lock_type_t lock(mutex_);
    return std::invoke(std::forward<F>(f), static_cast<const T&>(t_));
  }

  template <class F>
  auto apply(F&& f) {
    lock_type_t lock(mutex_);
    return std::invoke(std::forward<F>(f), static_cast<T&>(t_));
  }

 private:
  mutable mutex_type mutex_;
  T t_;
};

}  // namespace nil

#endif  // NIL_SRC_ASYNC_INC_ASYNC_ATOMICBASE_HPP_
