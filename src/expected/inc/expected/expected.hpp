#ifndef NIL_SRC_EXPECTED_INC_EXPECTED_HPP_
#define NIL_SRC_EXPECTED_INC_EXPECTED_HPP_

#include <iostream>
#include <meta/identity.hpp>
#include <variant>

#include "expected/expected_traits.hpp"
#include "expected/unexpected.hpp"

namespace nil {

/**
 * Allows for returning one of two types from a function, @tparam V on success
 * and @tparam E on faillure
 *
 * @tparam V - any type except special types used exclusive to this utility
 * @tparam E - any type except void and special types used exclusive to this
 * utility
 */
template <class V, class E>
class expected {
  static_assert(not std::is_void_v<E>, "E can't be void");
  static_assert(not std::is_reference_v<E>, "E can't be a reference");
  static_assert(not std::is_reference_v<V>, "V can't be a reference");
  static_assert(not detail::is_unexpected_t<E>{}, "E can't be unexpected<>");
  static_assert(not detail::is_unexpected_t<V>{}, "V can't be unexpected<>");

  using This_ = expected<V, E>;
  using SafeV_ = VoidSafe<V>;

 public:
  using value_type = V;
  using error_type = E;
  using unexpected_type = unexpected<E>;

  template <class T = V, IfVoid<T>* = nullptr>
  constexpr expected() : variant_{SafeV_{}} {}

  template <class T = V, IfNotVoid<T> = nullptr>
  expected() = delete;

  // constructors --------------------------------------------------------------

  /** Construct a new expected object with the expected type */
  template <class T = V, detail::if_proper_value_type<T, V, E>* = nullptr>
  constexpr expected(T&& v) : variant_{std::forward<T>(v)} {}

  /** Construct a new expected object with the unexpected type (by copy) */
  template <class T = E, IfConstructible<E, const T&>* = nullptr>
  constexpr expected(const unexpected<T>& e) : variant_{e} {}

  /** Construct a new expected object with the unexpected type (by move) */
  template <class T = E, IfConstructible<E, T&&>* = nullptr>
  constexpr expected(unexpected<T>&& e) : variant_{std::move(e)} {}

  // copy and move constructors and assignment ---------------------------------

  // implicitly defined based on whether std::variant<SafeV_, unexpected<E>>
  // defines or deletes them.

  // assignment from V and E ---------------------------------------------------

  template <class T = V, class = IfAssignable<V, T&&>>
  expected& operator=(T&& t) {
    variant_ = std::forward<T>(t);
    return *this;
  }

  template <class T = E, class = IfCopyAssignable<V, T>>
  expected& operator=(const unexpected<T>& e) {
    variant_ = e;
    return *this;
  }

  template <class T = E, class = IfMoveAssignable<V, T>>
  expected& operator=(unexpected<T>&& e) {
    variant_ = std::move(e);
    return *this;
  }

  // value observers -----------------------------------------------------------

  constexpr bool has_v() const { return variant_.index() == 0; }

  template <class T = V, class = IfNotVoid<T>>
  constexpr const SafeV_& value() const& {
    return std::get<V>(variant_);
  }

  template <class T = V, class = IfNotVoid<T>>
  constexpr SafeV_& value() & {
    return std::get<V>(variant_);
  }

  template <class T = V, class = IfNotVoid<T>>
  constexpr SafeV_&& value() && {
    return std::get<V>(std::move(variant_));
  }

  template <class T = V, class = IfNotVoid<T>>
  constexpr const SafeV_&& value() const&& {
    return std::get<V>(std::move(variant_));
  }

  template <class T = V, class = IfNotVoid<T>>
  constexpr const SafeV_& operator*() const& {
    return std::get<V>(variant_);
  }

  template <class T = V, class = IfNotVoid<T>>
  constexpr SafeV_& operator*() & {
    return std::get<V>(variant_);
  }

  template <class T = V, class = IfNotVoid<T>>
  constexpr SafeV_&& operator*() && {
    return std::get<V>(std::move(variant_));
  }

  template <class T = V, class = IfNotVoid<T>>
  constexpr const SafeV_* operator->() const {
    return &std::get<V>(variant_);
  }

  template <class T = V, class = IfNotVoid<T>>
  constexpr SafeV_* operator->() {
    return &std::get<V>(variant_);
  }

  // error observers -----------------------------------------------------------

  constexpr const bool has_e() const { return not has_v(); }

  constexpr const E& error() const& {
    return std::get<unexpected<E>>(variant_).value();
  }

  constexpr E& error() & { return std::get<unexpected<E>>(variant_).value(); }

  constexpr E&& error() && {
    return std::move(std::get<unexpected<E>>(std::move(variant_))).value();
  }

  // unexpected error observers ------------------------------------------------

  constexpr const unexpected<E>& as_unexpected() const& {
    return std::get<unexpected<E>>(variant_);
  }

  constexpr unexpected<E>& as_unexpected() & {
    return std::get<unexpected<E>>(variant_);
  }

  constexpr unexpected<E>&& as_unexpected() && {
    return std::get<unexpected<E>>(std::move(variant_));
  }

 private:
  /** let std variant do the heavy lifting for us */
  std::variant<SafeV_, unexpected<E>> variant_;
};

}  // namespace nil

#endif  // NIL_SRC_EXPECTED_INC_EXPECTED_HPP_