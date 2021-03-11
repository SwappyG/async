#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE container_traits_test

#include "async/container_traits.hpp"

#include <boost/mpl/list.hpp>
#include <boost/test/unit_test.hpp>
#include <mutex>

#include "async/container_base.hpp"

using namespace nil;
using namespace nil::async;

template <class T>
struct VectorLike {
  using value_type = T;
  using size_type = int;
  using iterator = int;
  VectorLike() = default;

  value_type at(size_type ii) const { return value_type{}; }
  value_type front() const { return value_type{}; }
  value_type back() const { return value_type{}; }

  void insert(iterator, const value_type&) {}
  void push_back(const value_type&) {}
  void pop_back() {}

  template <class... Args>
  void emplace_back(Args&&...) {}

  size_type size() const { return size_type{}; }
  bool empty() const { return true; }
  void clear() {}
  void erase(iterator) {}
};

template <class T>
struct DequeLike {
  using value_type = T;
  using size_type = int;
  using iterator = int;
  DequeLike() = default;

  value_type at(size_type ii) const { return value_type{}; }
  value_type front() const { return value_type{}; }
  value_type back() const { return value_type{}; }

  void insert(iterator, const value_type&) {}
  void push_back(const value_type&) {}
  void push_front(const value_type&) {}
  void pop_back() {}
  void pop_front() {}

  template <class... Args>
  void emplace_back(Args&&...) {}

  template <class... Args>
  void emplace_front(Args&&...) {}

  size_type size() const { return size_type{}; }
  bool empty() const { return true; }
  void clear() {}
  void erase(iterator) {}
};

template <class T>
struct ListLike {
  using value_type = T;
  using size_type = int;
  using iterator = int;
  ListLike() = default;

  value_type front() const { return value_type{}; }
  value_type back() const { return value_type{}; }

  void insert(iterator, const value_type&) {}
  void push_back(const value_type&) {}
  void push_front(const value_type&) {}
  void pop_back() {}
  void pop_front() {}

  template <class... Args>
  void emplace_back(Args&&...) {}

  template <class... Args>
  void emplace_front(Args&&...) {}

  size_type size() const { return size_type{}; }
  bool empty() const { return true; }
  void clear() const {}
  void erase(size_type ii) {}
};

using VectorTypes = boost::mpl::list<int, std::string, size_t, MoveOnly>;

BOOST_AUTO_TEST_CASE_TEMPLATE(VectorTraitTest, T, VectorTypes) {
  using CT = async::container_base<VectorLike<T>, std::mutex, std::lock_guard>;
  using C = typename CT::container_type;
  using V = typename C::value_type;
  using S = typename C::size_type;
  using I = typename C::iterator;

  BOOST_CHECK(CT::is_vector_like);
  BOOST_CHECK((is_vector_like_v<C>));
  BOOST_CHECK((is_vector_like_v<C, V, S, I>));

  BOOST_CHECK(!CT::is_deque_like);
  BOOST_CHECK((!is_deque_like_v<C>));
  BOOST_CHECK((!is_deque_like_v<C, V, S, I>));

  BOOST_CHECK(!CT::is_list_like);
  BOOST_CHECK((!is_list_like_v<C>));
  BOOST_CHECK((!is_list_like_v<C, V, S, I>));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(DequeTraitTest, T, VectorTypes) {
  using CT = async::container_base<DequeLike<T>, std::mutex, std::lock_guard>;
  using C = typename CT::container_type;
  using V = typename C::value_type;
  using S = typename C::size_type;
  using I = typename C::iterator;

  BOOST_CHECK(CT::is_vector_like);
  BOOST_CHECK((is_vector_like_v<C>));
  BOOST_CHECK((is_vector_like_v<C, V, S, I>));

  BOOST_CHECK(CT::is_deque_like);
  BOOST_CHECK((is_deque_like_v<C>));
  BOOST_CHECK((is_deque_like_v<C, V, S, I>));

  BOOST_CHECK(CT::is_list_like);
  BOOST_CHECK((is_list_like_v<C>));
  BOOST_CHECK((is_list_like_v<C, V, S, I>));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ListTraitTest, T, VectorTypes) {
  using CT = async::container_base<ListLike<T>, std::mutex, std::lock_guard>;
  using C = typename CT::container_type;
  using V = typename C::value_type;
  using S = typename C::size_type;
  using I = typename C::iterator;

  BOOST_CHECK(!CT::is_vector_like);
  BOOST_CHECK((!is_vector_like_v<C>));
  BOOST_CHECK((!is_vector_like_v<C, V, S, I>));

  BOOST_CHECK(!CT::is_deque_like);
  BOOST_CHECK((!is_deque_like_v<C>));
  BOOST_CHECK((!is_deque_like_v<C, V, S, I>));

  BOOST_CHECK(CT::is_list_like);
  BOOST_CHECK((is_list_like_v<C>));
  BOOST_CHECK((is_list_like_v<C, V, S, I>));
}