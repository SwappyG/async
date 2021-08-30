#ifndef NIL_SRC_MEMORY_INC_MEMORY_DEEPPTR_HPP_
#define NIL_SRC_MEMORY_INC_MEMORY_DEEPPTR_HPP_

#include <meta/enable_if.hpp>

#include <memory>

namespace nil {

template <class T>
class default_copier {
 public:
  using element_type = T;
  using uptr = std::unique_ptr<T>;
  using copy_func = std::function<uptr()>;
  
  constexpr default_copier() noexcept = default;
  
  default_copier(const T& t) 
    : copier_{[t]() -> uptr { 
        return std::make_unique<T>(t); 
      }} {}

  default_copier(const default_copier& other) = default;
  default_copier(default_copier&& other) = default;
  
  default_copier& operator=(const default_copier& other) = default;
  default_copier& operator=(default_copier&& other) = default;

  template <class U, typename = detail::if_convertible<U*, T*>>
  default_copier(const default_copier<U>& other) 
    : copier_{[other_copier = other.get_copier()] () -> uptr { 
        return other_copier(); 
      }} {}

  template <class U, typename = detail::if_convertible<U*, T*>>
  default_copier(default_copier<U>&& other) 
    : copier_{[other_copier = std::move(other).get_copier()] () -> uptr { 
        return other_copier(); 
      }} {}

  template <class U, typename = detail::if_convertible<U*, T*>>
  default_copier& operator=(const default_copier<U>& other) {
    copier_ = [other_copier = other.get_copier()] () -> uptr { 
      return other_copier(); 
    };
  }

  template <class U, typename = detail::if_convertible<U*, T*>>
  default_copier& operator=(default_copier<U>&& other) {
    copier_ = [other_copier = std::move(other).get_copier()] () -> uptr { 
      return other_copier(); 
    };
  } 

  std::unique_ptr<T> operator()() const {
    return copier_();
  }

  const copy_func& get_copier() const& { 
    return copier_;
  }

  copy_func get_copier() && { 
    return std::move(copier_);
  }

 private:
  copy_func copier_{[]() -> uptr { return {nullptr}; }};
};

template <class T, class Copier = default_copier<T>>
class deep_ptr {
  static_assert(not std::is_array<T>{}, "deep_ptr doesn't support array types");
  static_assert(not std::is_reference<T>{}, "deep_ptr doesn't support reference types");
  static_assert(not std::is_pointer<T>{}, "deep_ptr shouldn't be templated with raw pointer");

 private:
  using type = T; 
  using uptr = std::unique_ptr<T>;
  
  template <class U, class = if_convertible<U*, T*>>
  friend class deep_ptr;

  // constructor that all public constructors call // inits all fields
  template <class uptr_ = uptr, class C = Copier>
  deep_ptr(uptr_&& pointer, C&& copier)
      : p_{std::forward<uptr_>(pointer)}, 
        copier_{std::forward<C>(copier)} {}

  // internal release for implementing move constructor // mirrors unique_ptr release mechanism
  std::unique_ptr<T> release_() {
    using std::swap;
    std::unique_ptr<T> temp{nullptr};
    swap(p_, temp);
    return temp;
  }

 public:
  /** constructs in null state */
  deep_ptr(std::nullptr_t = nullptr) {}

  /** takes ownership of pointer */
  explicit deep_ptr(T* t) 
    : deep_ptr{std::unique_ptr<T>{t}, Copier(t)} {}

  /** @note since we build off unique_ptr, it handles all RAII for us */
  ~deep_ptr() {}

  /** uses the copier to make new unique_ptr, also copies the copier itself! */
  deep_ptr(const deep_ptr& other) 
    : deep_ptr{other.copier_(), other.copier_} {}

  /** transfers unique_ptr and copier over to this instance */
  deep_ptr(deep_ptr&& other) noexcept = default;

  /** copies from a deep_ptr whose template isn't T, but convertible to T */
  template <class U = T, typename = if_convertible<U*, T*>>
  deep_ptr(const deep_ptr<U>& other) 
    : deep_ptr{other.copier_(), other.copier_} {}

  /** same as above, but move instead of copy */
  template <class U, typename = if_convertible<U*, T*>>
  deep_ptr(deep_ptr<U>&& other) noexcept 
    : deep_ptr{std::move(other.p_), std::move(other.copier_)} {}

  // copy assignment just uses copy constructor to avoid code duplication
  deep_ptr& operator=(const deep_ptr& other) {
    using std::swap;
    deep_ptr temp{other};
    swap(temp, *this);
    return *this;
  }

  // move assignment just uses copy constructor to avoid code duplication
  template <class U, typename = detail::IfConvertible<U*, T*>>
  deep_ptr& operator=(deep_ptr<U>&& other) noexcept {
    using std::swap;
    deep_ptr temp{std::move(other)};
    swap(temp, *this);
    return *this;
  }

  // swaps two instances, must be same template type
  void swap(deep_ptr& other) noexcept {
    using std::swap;
    swap(p_, other.p_);
    swap(copier_, other.copier_);
  }

  /**
   * Rest of API is identical to unique_ptr, lowercase methods ensure it's plug and play
   * @note the same restrictions as unique_ptr apply!
   */

  /** will relinquish ownership of pointer! */
  T* release() {
    p_.release();
  }

  void reset(std::nullptr_t) noexcept {
    deep_ptr temp{nullptr};
    using std::swap;
    swap(*this, temp);
  }

  void reset(T* t) noexcept {
    deep_ptr temp{t};
    using std::swap;
    swap(*this, temp);
  }

  /** undefined behaviour if you try to dereference while in null state, same as unique_ptr */
  T& operator*() {
    return p_.operator*();
  }

  T* operator->() {
    return p_.operator->();
  }

  T* get() {
    return p_.get();
  }

 private:
  std::unique_ptr<T> p_;
  default_copier<T> copier_;
};

template <class T>
void swap(deep_ptr<T>& lhs, deep_ptr<T>& rhs) {
  lhs.swap(rhs);
}

}  // namespace nil

#endif  // NIL_SRC_MEMORY_INC_MEMORY_DEEPPTR_HPP_
