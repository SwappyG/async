# nilcpp

nilcpp is (will be) a collection of small C++ libraries that raises the level of abstraction and makes it easier to write safer code. The implementations are intended to be simple, easy to understand and easy to fork and modify for specific uses. Performance, while important, is a secondary concern to simplicity and safety for this collection. 

## Modules

Currently, there's only one module, `async`. Stay tuned for updates. 

## Async

`async` provides higher level abstractions for working with multi-threaded code in C++ beyond the default STL constructs. It promotes simpler and safer code such that non-thread-safe operations don't compile at all, without limiting overall utility.

### Current Components

#### nil::atomic

`nil::atomic` is similar to `std::atomic`. It couples a mutex with any type T and the public API only provides thread-safe access to the underlying data. 

##### Features / Limitations
- `nil::atomic_base` is templated on the mutex and lock types, so you can use it with STL, Boost or other locks
  - `nil::atomic` is a convenience alias for using `std::mutex` and `std::lock_guard`
- It currently only works with `copyable` types
  - This is because `peek`, the only "normal" accessor, returns a copy, making it hard to work with non-copyable types
- It provides a generic `apply` method which executes any function while locked, allowing for multi-statement thread-safe execution when needed

#### nil::async::optional

`nil::async::optional` is the same as `nil::atomic` with an additional nullstate for the underlying type. As with `nil::optional`, it only provides a thread-safe API.

##### Features / Limitations
- `nil::async::optional_base` is templated on the optional type, allowing you to work with std::optional, boost::optional or others
  - `nil::async::optional` is a convenience alias for `std::optional`
- It will also be templated on the mutex type, so you can use it with any mutex-like type
  - currently, it uses `std::mutex`
- In addition to `push` and `peek`, it also provides a `pop` to leave the data behind in a null-state
- It works with move-only types, since `pop` can return a moved value. `peek` is non usuable if the type is not copyable
- It also has `apply`, which works the same as `nil::atomic`

#### nil::atomic_rw

`nil::atomic_rw` allows for multiple concurrent readers and exclusive writers. It wraps any type T with a shared_mutex and provides RAII read/write proxies through public API

##### Features
- `nil::atomic_rw_base` is templated on mutex and read/write lock type, so it works with STL, Boost and others
  - `nil::atomic_rw` is convenience alias for `std::shared_mutex`, `std::shared_lock` and `std::unique_lock` 
- terse, pointer-like semantics for obtaining read proxies. All proxies are RAII, they release lock on destruction
- proxies provide pointer-like access to underlying data, either read-only for read proxies, or read/write for write proxies.

#### nil::async::container

`nil::async::container` allows you to wrap STL-like containers (vector/deque/list) and provides a thread-safe API to underlying data. The function names are the same, so it can be a (near) drop-in replacement.

##### Features / Limitations
- `nil::async::container_base` is templated on the mutex and lock types to work with STL, Boost or others
  - `nil::async::vector`, `nil::async::deque` and `nil::async::list` are convenience aliases for their respective STL containers
- provides most functions of STL containers, with similar behaviour
  - no calls return refs, as that would not be thread-safe. Instead, you get copies   
  - no iterators, because they are defacto refs
  - `extract` doesn't return nodes, but rather just the (moved) element
- container type used doesn't need to be STL or Boost, and doesn't need to implement all functions
  - if you attempt to use an unimplemented function, it won't compile (no runtime issues or UB)
- generic `apply` function for handling anything the normal API doesn't already do. Works the same as `nil::atomic::apply(...)`
- comes with some traits for SFINAE or similar purposes
