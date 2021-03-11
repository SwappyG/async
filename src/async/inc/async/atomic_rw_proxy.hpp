#ifndef NIL_SRC_THREADSAFE_INC_THREADSAFE_ATOMICRWPROXY_HPP_
#define NIL_SRC_THREADSAFE_INC_THREADSAFE_ATOMICRWPROXY_HPP_

namespace nil {

/**
 * @note this class is returned the atomic_rw_base class and should not be used
 * directly
 *
 * Captures a reader lock and a const ref to some data. Provides const-only
 * pointer-like access to the underlying data
 *
 * @note assumes @tparam ReadLock is RAII and release mutex on destruction
 * @note the instance of this class must NOT outlive the original atomic class
 */
template <class T, class SharedMutex, template <class> class ReadLock>
class atomic_r_proxy {
 public:
  using value_type = T;
  using mutex_type = SharedMutex;
  using read_lock = ReadLock<SharedMutex>;

  atomic_r_proxy() = delete;  //!< no default construction, must have lock

  /** Takes ownership of lock */
  atomic_r_proxy(read_lock&& lk, const T& t) : lk_{std::move(lk)}, t_{t} {}

  // pointer-like access -------------------------------------------------------

  const T& operator*() const { return t_; }
  const T* operator->() const { return &t_; }

  // function style access -----------------------------------------------------

  const T& value() const { return t_; }

 private:
  read_lock lk_;
  const T& t_;
};

/**
 * Same as atomic_r_proxy, but captures its data by non-const ref, and provides
 * non-const acccess to it.
 *
 * @note same as atomic_r_proxy, this should not be constructed directly
 * @note same as atomic_r_proxy, this must NOT outlive the original atomic class
 */
template <class T, class SharedMutex, template <class> class WriteLock>
class atomic_rw_proxy {
 public:
  using value_type = T;
  using mutex_type = SharedMutex;
  using write_lock = WriteLock<SharedMutex>;

  atomic_rw_proxy() = delete;  //!< must be initialized with a lock and data

  /** takes ownership of write lock and ref to data */
  atomic_rw_proxy(write_lock&& lk, T& t) : lk_{std::move(lk)}, t_{t} {}

  // pointer like const and non-const access to data ---------------------------
  T& operator*() { return t_; }
  const T& operator*() const { return t_; }

  T* operator->() { return &t_; }
  const T* operator->() const { return &t_; }

  // function style const and non-const access to data -------------------------

  T& value() & { return t_; }
  const T& value() const& { return t_; }

 private:
  write_lock lk_;
  T& t_;
};

}  // namespace nil

#endif  // NIL_SRC_THREADSAFE_INC_THREADSAFE_ATOMICRWPROXY_HPP_
