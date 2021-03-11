#ifndef NIL_SRC_ASYNC_INC_ASYNC_OPTIONALBASE_HPP_
#define NIL_SRC_ASYNC_INC_ASYNC_OPTIONALBASE_HPP_

#include <functional>
#include <meta/enable_if.hpp>
#include <mutex>

namespace nil::async {

/**
 * Similar to the atomic class but provides an additional null state for T
 *
 * @tparam T - any type, including move-only types
 * @tparam OptT - the underlying optional type, like std::optional
 * @tparam NullT - the null type for @tparam OptT, like std::nullopt_t
 */
template <class T, template <class> class OptT, class NullT>
class optional_base {
 public:
  using value_type = T;
  using opt_type = OptT<T>;
  using null_type = NullT;

 private:
  mutable std::mutex mutex_;
  opt_type t_{};

 public:
  // construct with null -------------------------------------------------------

  constexpr optional_base() = default;
  constexpr optional_base(null_type) {}

  // construct with T ----------------------------------------------------------

  /** std::in_place_t ensures variadic universal ref constructor isn't greedy */
  template <class... Args, if_constructible<T, Args...>* = nullptr>
  constexpr optional_base(std::in_place_t, Args&&... args)
      : t_{std::in_place, std::forward<Args>(args)...} {}

  // no copying/moving ---------------------------------------------------------

  optional_base(const optional_base&) = delete;
  optional_base& operator=(const optional_base&) = delete;
  optional_base(optional_base&&) = delete;
  optional_base& operator=(optional_base&&) = delete;

  // inspect -------------------------------------------------------------------

  /** only valid if T is copyable */
  opt_type peek() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return t_;
  }

  // update --------------------------------------------------------------------

  void push(const opt_type& opt_val) {
    std::lock_guard<std::mutex> lock(mutex_);
    t_ = opt_val;
  }

  void push(opt_type&& opt_val) {
    std::lock_guard<std::mutex> lock(mutex_);
    t_ = std::move(opt_val);
  }

  template <class U = T, class = if_assignable<T, U&&>>
  void push(U&& u) {
    std::lock_guard<std::mutex> lock(mutex_);
    t_ = {std::forward<U>(u)};
  }

  template <class... Args, if_constructible<T, Args...>* = nullptr>
  void push(std::in_place_t, Args&&... args) {
    std::lock_guard<std::mutex> lock(mutex_);
    t_ = {{std::forward<Args>(args)...}};
  }

  // remove and return ---------------------------------------------------------

  template <class U = T, class = if_constructible<T, U&&>>
  opt_type pop() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!t_) {
      return opt_type{};
    }
    auto temp = std::move(t_.value());
    t_.reset();
    return {std::move(temp)};
  }

  // arbitrary function --------------------------------------------------------

  template <class F>
  auto apply(F&& f) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return std::invoke(std::forward<F>(f), static_cast<const opt_type&>(t_));
  }

  template <class F>
  auto apply(F&& f) {
    std::lock_guard<std::mutex> lock(mutex_);
    return std::invoke(std::forward<F>(f), static_cast<opt_type&>(t_));
  }
};

}  // namespace nil::async

#endif  // NIL_SRC_ASYNC_INC_ASYNC_OPTIONALBASE_HPP_
