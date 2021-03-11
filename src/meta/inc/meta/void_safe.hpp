#ifndef NIL_SRC_META_INC_META_VOIDSAFE_HPP_
#define NIL_SRC_META_INC_META_VOIDSAFE_HPP_

#include "meta/devoid.hpp"

namespace nil {

/** Either just @tparam T or devoid if T is void */
template <class T>
using void_safe = std::conditional_t<std::is_void_v<T>, devoid, T>;

}  // namespace nil

#endif  // NIL_SRC_META_INC_META_VOID_SAFE_HPP_