#ifndef NIL_SRC_ASYNC_INC_ASYNC_CONTAINER_HPP_
#define NIL_SRC_ASYNC_INC_ASYNC_CONTAINER_HPP_

#include "async/container_traits.hpp"

namespace nil::async {

/**
 * Wraps an STL-like container and provides thread-safe only access to all
 * functions. Uses the same names as std::vector, std::deque and std::list, for
 * a near drop in replacement.
 *
 * In addition to singular operations, it also provides a generic `apply`
 * function which can execute arbitary number of statements while a lock is
 * acquired
 *
 * @note If using STL containers, it's recommeneded to use the helper aliases
 *
 * @note iterators are not supported, since returning references to internal
 * data while not acquiring a lock is not thread-safe. May be extended in future
 * with read-write proxies
 *
 * @note querying by reference is also not supported for the same reason as
 * above. So at, front and back all return copies
 *
 * @tparam C - a fully templated STL-Like container, such as std::vector<int>.
 * @tparam Mutex - a standard mutex type, like std::mutex
 * @tparam LockGuard - an RAII lock type, like std::lock_guard
 */
template <class C, class Mutex, template <class> class LockGuard>
class container_base {
 public:
  using container_type = C;
  using value_type = typename container_type::value_type;
  using size_type = typename container_type::size_type;
  using mutex_type = Mutex;
  using lock_type_t = LockGuard<Mutex>;

  // traits to detect what type this container is similar to -------------------

  static constexpr auto is_vector_like = is_vector_like_v<container_type>;
  static constexpr auto is_deque_like = is_deque_like_v<container_type>;
  static constexpr auto is_list_like = is_list_like_v<container_type>;

  // constructors --------------------------------------------------------------

  container_base() = default;

  template <class CT>
  using if_container_assignable = if_assignable<container_type, CT>;

  template <class... Args>
  container_base(std::in_place_t, Args&&... args) {
    (c_.emplace_back(std::forward<Args>(args)), ...);
  }

  template <class CT = container_type, if_container_assignable<CT>* = nullptr>
  explicit container_base(CT&& ct) {
    c_ = std::forward<CT>(ct);
  }

  // Copy / Move ---------------------------------------------------------------

  container_base(const container_base&) = delete;
  container_base& operator=(const container_base&) = delete;
  container_base(container_base&&) = delete;
  container_base& operator=(container_base&&) = delete;

  // Assignment ----------------------------------------------------------------

  template <class CT = container_type>
  void assign(CT&& ct) {
    lock_type_t lock{mutex_};
    c_ = std::forward<CT>(ct);
  }

  template <class... Args>
  void assign(std::in_place_t, Args&&... args) {
    lock_type_t lock{mutex_};
    c_.clear();
    (c_.emplace_back(std::forward<Args>(args)), ...);
  }

  // Access --------------------------------------------------------------------

  std::optional<value_type> at(size_type ii) const {
    lock_type_t lock{mutex_};
    if (ii >= c_.size()) {
      return std::nullopt;
    }
    return c_.at(ii);
  }

  std::optional<value_type> front() const {
    lock_type_t lock{mutex_};
    if (c_.empty()) {
      return std::nullopt;
    }
    return c_.front();
  }

  std::optional<value_type> back() const {
    lock_type_t lock{mutex_};
    if (c_.empty()) {
      return std::nullopt;
    }
    return c_.back();
  }

  // Insertion -----------------------------------------------------------------

  template <class S = size_type, class V = value_type>
  bool insert(S index, V&& v) {
    lock_type_t lock{mutex_};
    if (index > c_.size()) {
      return false;
    }
    c_.insert(std::next(c_.begin(), index), std::forward<V>(v));
    return true;
  }

  template <class V = value_type>
  void push_back(V&& v) {
    lock_type_t lock{mutex_};
    return c_.push_back(std::forward<V>(v));
  }

  template <class V = value_type>
  void push_front(V&& v) {
    lock_type_t lock{mutex_};
    return c_.push_front(std::forward<V>(v));
  }

  // Remove --------------------------------------------------------------------

  void clear() {
    lock_type_t lock{mutex_};
    c_.clear();
  }

  void erase(size_type ii) {
    lock_type_t lock{mutex_};
    if (ii >= c_.size()) {
      return;
    }
    c_.erase(std::next(c_.begin(), ii));
  }

  void pop_back() {
    lock_type_t lock{mutex_};
    if (c_.empty()) {
      return;
    }
    c_.pop_back();
  }

  void pop_front() {
    lock_type_t lock{mutex_};
    if (c_.empty()) {
      return;
    }
    c_.pop_front();
  }

  std::optional<value_type> extract(size_type ii) {
    lock_type_t lock{mutex_};
    if (ii >= c_.size()) {
      return std::nullopt;
    }
    auto v = std::move(c_.at(ii));
    c_.erase(std::next(c_.begin(), ii));
    return std::move(v);
  }

  std::optional<value_type> extract_back() {
    lock_type_t lock{mutex_};
    if (c_.empty()) {
      return std::nullopt;
    }
    auto v = std::move(c_.back());
    c_.pop_back();
    return std::move(v);
  }

  std::optional<value_type> extract_front() {
    lock_type_t lock{mutex_};
    if (c_.empty()) {
      return std::nullopt;
    }
    auto v = std::move(c_.front());
    c_.pop_front();
    return std::move(v);
  }

  // arbitrary function --------------------------------------------------------

  template <class F, class = if_invocable<F, const container_type&>>
  auto apply(F&& f) const {
    lock_type_t lock{mutex_};
    return std::invoke(std::forward<F>(f), c_);
  }

  template <class F, class = if_invocable<F, container_type&>>
  auto apply(F&& f) {
    lock_type_t lock{mutex_};
    return std::invoke(std::forward<F>(f), c_);
  }

  template <class F, class = if_invocable<F, const value_type&>>
  void apply_each(F&& f) const {
    lock_type_t lock{mutex_};
    for (const auto& ii : c_) {
      std::invoke(std::forward<F>(f), ii);
    }
  }

  template <class F, class = If<std::is_invocable_v<F, value_type&> &&
                                not std::is_invocable_v<F, const value_type&>>>
  void apply_each(F&& f) {
    lock_type_t lock{mutex_};
    for (auto& ii : c_) {
      std::invoke(std::forward<F>(f), ii);
    }
  }

  // state observers -----------------------------------------------------------

  size_type size() const noexcept {
    lock_type_t lock{mutex_};
    return c_.size();
  }

  bool empty() const noexcept {
    lock_type_t lock{mutex_};
    return c_.empty();
  }

 private:
  mutable mutex_type mutex_;
  container_type c_;
};

}  // namespace nil::async

#endif  // NIL_SRC_ASYNC_INC_ASYNC_CONTAINER_HPP_
