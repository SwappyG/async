#include <future>
#include <boost/variant.hpp>


namespace nil {

enum class EventLoopResult {
  COMPLETED,
  PREEMPTED,
  REJECTED
};

template<class Event, class Return, bool SharedReturns, class... EventArgs>
class EventLoop {
 public:
  using ArgsVariant = boost::variant<boost::blank, EventArgs...>;
  using Response = std::pair<EventLoopResult, Return>;
  using ResponseFuture = std::future<Response>;
  using EventProcessFunc = std::function<void(boost::optional<Event>, ArgsVariant)>;

  struct Request {
    Event event;
    ArgsVariant args;
    std::promise<ResponseFuture> response_promise;
  };

 public:
  EventLoop(const std::string& name, std::chrono::microseconds period,
            EventProcessFunc event_process_func)
    : event_process_func_(event_process_func),
      event_loop_(CyclicalTask::Create([this](){ return EventTask_(); }, period, name))  {
    event_loop_->Start();
  }

  ~EventLoop() {
    event_loop_->Stop().get();
  }

  ResponseFuture PushRequest(Event event, ArgsVariant args) {
    std::promise<Request> p;
    auto f = p.get_future();
    requests_.PushBack({event, std::move(args), std::move(p)});
    return f;
  }

 private:
  void EventTask_() {
    auto request = requests_.PopFront();
    if (boost::none == request) {
      event_process_func_(boost::none, boost::blank{});
      return;
    }

    auto next_request = boost::optional<Request>{boost::none};
    while (boost::none != request) {
      next_request = requests_.PopFront();
      if (boost::none == next_request) {
         request->response_promise.set_value(event_process_func_(request->event, std::move(request->args)));
         return;
      }

      if (static_cast<int>(next_request->event) > static_cast<int>(request->event)) {
        request->response_promise.set_value(MakeReadyFuture(Response{EventLoopResult::PREEMPTED, {}}));
        request = std::move(next_request);
      } else {
        next_request->response_promise.set_value(MakeReadyFuture(Response{EventLoopResult::PREEMPTED, {}}))
      }
    }

    auto next_request = requests_.PopFront();
    if (boost::none == next_request) {
      request->response_promise.set_value(sm_.Update(request->event, std::move(request->args)));
    }

    while (true) {
      next_request = requests_.PopFront();
      if (boost::none == next_request) {
        return request->response_promise.set_value(sm_.Update(request->event, std::move(request->args)));
      }
    }
  }

  EventProcessFunc event_process_func_;
  CyclicalTask::Ptr event_loop_;
  threadsafe::Deque<Request> requests_;
};

}  // namespace nil