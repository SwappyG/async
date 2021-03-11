#ifndef NIL_SRC_META_INC_META_IDENTITY_HPP_
#define NIL_SRC_META_INC_META_IDENTITY_HPP_

namespace nil {

/** holds @tparam under alias type */
template <class T>
struct identity {
  using type = T;
};

}  // namespace nil

#endif  // NIL_SRC_META_INC_META_IDENTITY_HPP_