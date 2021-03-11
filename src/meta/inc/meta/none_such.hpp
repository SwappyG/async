#ifndef NIL_SRC_META_INC_META_NONESUCH_HPP_
#define NIL_SRC_META_INC_META_NONESUCH_HPP_

#include <type_traits>

namespace nil {

struct none_such {
  none_such() = delete;
  none_such(const none_such&) = delete;
  none_such(none_such&&) = delete;
  none_such& operator=(const none_such&) = delete;
  none_such& operator=(none_such&&) = delete;
};

struct MoveOnly {
  MoveOnly() = default;
  MoveOnly(const MoveOnly&) = delete;
  MoveOnly& operator=(const MoveOnly&) = delete;
  MoveOnly(MoveOnly&&) noexcept = default;
  MoveOnly& operator=(MoveOnly&&) noexcept = default;
};

struct NonDefaultConstructible {
  NonDefaultConstructible() = delete;
  NonDefaultConstructible(int ii_) : ii{ii_} {}
  NonDefaultConstructible(const NonDefaultConstructible&) = default;
  NonDefaultConstructible& operator=(const NonDefaultConstructible&) = default;
  NonDefaultConstructible(NonDefaultConstructible&&) = default;
  NonDefaultConstructible& operator=(NonDefaultConstructible&&) = default;
  int ii;
};

}  // namespace nil

#endif  // NIL_SRC_META_INC_META_NONESUCH_HPP_