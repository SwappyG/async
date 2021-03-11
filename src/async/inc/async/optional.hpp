#ifndef NIL_SRC_ASYNC_INC_ASYNC_OPTIONAL_HPP_
#define NIL_SRC_ASYNC_INC_ASYNC_OPTIONAL_HPP_

#include <optional>

#include "async/optional_base.hpp"

namespace nil::async {

/**
 * Partially specified alias if working with std::optional. This should be the
 * prefered usage.
 */
template <class T>
using optional = optional_base<T, std::optional, std::nullopt_t>;

}  // namespace nil::async

#endif  // NIL_SRC_ASYNC_INC_ASYNC_OPTIONAL_HPP_
