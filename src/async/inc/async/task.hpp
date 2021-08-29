#ifndef NIL_SRC_ASYNC_INC_ASYNC_TASK_HPP_
#define NIL_SRC_ASYNC_INC_ASYNC_TASK_HPP_

#include <future>
#include <memory>

#include "async/optional.hpp"

namespace nil::async {

template <class Payload>
class send_pipe {
  send_pipe(std::shared_ptr<async::optional<Payload>> pipe)
      : pipe_{std::move(pipe)} {}

  template <class P = Payload>
  void send(P&& p) {
    pipe_->push(std::forward<P>(p));
  }

 private:
  std::shared_ptr<async::optional<Payload>> pipe_;
};

template <class Payload>
class recv_pipe {
  recv_pipe(std::shared_ptr<async::optional<Payload>> pipe)
      : pipe_{std::move(pipe)} {}

  std::optional<Payload> recv_opt() { pipe_->pop(); }

  Payload blocking_recv() {
    while (true) {
      auto r = pipe_->pop();
      if (r != boost::none) {
        return r.value();
      }
    }
  }

  template <class R, class P>
  std::optional<Payload> try_recv(std::chrono::duration<R, P> timeout) {
    auto start_time = std::chrono::steady_clock::now();
    while ((std::chrono::steady_clock::now() - start_time) < timeout) {
      auto r = pipe_->pop();
      if (r != boost::none) {
        return r.value();
      }
    }
    return std::nullopt;
  }

 private:
  std::shared_ptr<async::optional<Payload>> pipe_;
};

template <class Payload, class Return>
struct task_future {
  task_pipe(std::future<Return>&& future, task_pipe<Payload>&& task_pipe)
      : future_{std::move(future)}, pipe_{std::move(task_pipe)} {}

  std::future<Return> future_;
  send_pipe<Payload> pipe_;
};

template <class Payload, class Func>
auto task(Func&& func) {
  auto payload_ptr = std::make_shared<async::optional<Payload>>(std::nullopt);

  auto future = std::async(
      [pipe = recv_pipe<Payload>{payload_ptr}, f = std::forward<Func>(func)]() {
        std::invoke(std::forward<Func>(func), pipe)
      });

  return task_future<Payload, std::invoke_result<Func, recv_pipe<Payload>>>{
      std::move(future), send_pipe<Payload>{std::move(payload_ptr)}};
}

}  // namespace nil::async

#endif  // NIL_SRC_ASYNC_INC_ASYNC_TASK_HPP_