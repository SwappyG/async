#ifndef NIL_SRC_META_INC_META_DEVOID_HPP_
#define NIL_SRC_META_INC_META_DEVOID_HPP_

#include <type_traits>

namespace nil {

struct devoid {};

/** trait to detect struct devoid */
template <class T>
using is_devoid = std::is_same<devoid, T>;

/** convenience inline variable for devoid detection */
template <class T>
inline constexpr auto is_devoid_v = is_devoid<T>::value;

/** convenience alias for SFINAE */
template <class T>
using if_devoid = std::enable_if_t<is_devoid_v<T>>;

/** convenience alias for SFINAE */
template <class T>
using if_not_devoid = std::enable_if_t<not is_devoid_v<T>>;

}  // namespace nil

#endif  // NIL_SRC_META_INC_META_DEVOID_HPP_