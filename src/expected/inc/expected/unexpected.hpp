#ifndef NIL_SRC_MAYBE_INC_UNEXPECTED_HPP_
#define NIL_SRC_MAYBE_INC_UNEXPECTED_HPP_

#include "expected/ExpectedTraits.hpp"

namespace nil {

/**
 * Disambiguates success and failure types for expected<>, allowing both to be
 * the same underlying type
 *
 * @tparam E - any non-ref, non-void type. Types special to this utility are
 * also not allowed
 */
template <class E>
class unexpected;

/** Convnience function for automatic type deduction */
template <class E>
auto MakeUnexpected(E&& e) {
  return unexpected<std::decay_t<E>>(std::forward<E>(e));
}

template <class E>
class unexpected {
  static_assert(not std::is_void<E>{}, "E can't be void");
  static_assert(not std::is_reference<E>{}, "E can't be a reference");
  static_assert(not detail::is_unexpected_t<E>::value, "E can't be unexpected<>");

 public:
  using tag = detail::unexpected_tag;  //!< helps ID this as unexpected Type

  unexpected() = delete;

  template <class U = E, class = void, IfConstructible<E, U&&>* = nullptr>
  explicit constexpr unexpected(U&& e) : e_{std::forward<U>(e)} {}

  constexpr const E& value() const& { return e_; }
  constexpr const E&& value() const&& { return std::move(e_); }
  constexpr E& value() & { return e_; }
  constexpr E&& value() && { return std::move(e_); }

 private:
  E e_;
};

}  // namespace nil

#endif  // NIL_SRC_MAYBE_INC_UNEXPECTED_HPP_
