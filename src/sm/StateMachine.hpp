#include <functional>
#include <future>
#include <map>
#include <set>

namespace nil {
namespace utils {

/**
 * State machine Utility Example Usage:
 *
 * auto sm = StateMachine<States, Results, Interrupts, ExtData, IntData>();
 *
 * // Attach all state funcs first before anything else // stuff will fail if you try to operate
 * // on a state that does not have an attached state function
 * sm.AttachStateFunc(States::A, &FuncA);
 * sm.AttachStateFunc(States::B, &FuncB);
 * sm.AttachStateFunc(States::C, &FuncC);
 * sm.AttachStateFunc(States::Err, &FuncErr);
 *
 * // Make any groups we're interested in
 * sm.CreateGroup("normal_states", std::set<States>{States::A, States::B, States::C});
 *
 * // Add internal transitions
 * sm.AddTransition( States::A, Results::OK , States::A );
 * sm.AddTransition( States::B, Results::OK , States::C );
 * sm.AddTransition( States::B, Results::FAIL , States::A );
 * sm.AddTransition( States::C, Results::OK , States::A );
 *
 * // Add external transitions
 * sm.AddTransition( States::A, Interrupts::START, States::B );
 * sm.AddTransition( States::Err, Interrupts::CLEAR, States::A );
 *
 * // Add group transitions
 * sm.AddTransition( "normal_states", Results::ERR, States::Err );
 *
 * // Set the initial State
 * sm.SetInitState(States::A);
 *
 * // Set Priorities
 * map<Results, int> res_prios;
 * res_prios.insert({Results::OK, 1});
 * res_prios.insert({Results::FAIL, 2});
 * res_prios.insert({Results::Err, 10});
 *
 * map<Interrupts, int> intr_prios;
 * intr_prios.insert({Interrupts::START, 3});
 * intr_prios.insert({Interrupts::START, 4});
 *
 * sm.SetPriorities(res_prios, intr_prios);
 *
 * // Visualize the state machine if you'd like
 * sm.VisualizeStateMachine();
 *
 * // Start the state machine // this runs Validate_ and fails if you there are setup errors
 * // These include missing state functions, missing priorities, missing initial state, etc
 * sm.Start();
 *
 * // Send an interrupt to move from A to B // payload is empty
 * sm.SetInterrupt(Interrupts::A, nullptr);
 *
 */
template <typename StateType,        // enum for states
          typename ResultType,       // enum for internal events
          typename InterruptType,    // enum for external interrupts
          typename ExternalPayload,  // struct for external payload
          typename InternalData>     // struct for data passed to each state
class StateMachine {
 public:
  using StateFunc = std::function<ResultType(InternalData&, std::shared_ptr<ExternalPayload>)>;

  enum class InterruptResult {
    kAccepted = 0,
    kNoTransitionFromCurrState,
    kDuplicateInterrupt,
    kPreemptedByHigherPriorityInterrupt,
    kInternalError
  };

  enum class Mode {
    kStopped,
    kStarted,
  };

  StateMachine() = default;

  // RUN TIME FUNCTIONS ----------------------------------------------------------------------------

  void Start();  //!< starts the event loop
  void Stop();   //!< stops the event loop

  /**
   * @brief Interrupt - set an interrupt from the outside world with some payload. A future is
   * returned when the interrupt is processed by the event loop. The future here only returns
   * whether this was accepted or rejected and is agnostic to the specific implementation. If more
   * data needs to be returned from the state machine to an interrupt, add futures/promises to the
   * ExternalPayload
   *
   * @param type - the enumerated tpye of the interrupt
   * @param payload - any data that needs to accompany the interrupt
   * @return a future indicating whether the interrupt was accepted, rejected or superceded is
   * returned immediately, and will be valid once the event loop processes it
   */
  std::future<InterruptResult> SetInterrupt(InterruptType type,
                                            std::shared_ptr<ExternalPayload> payload);

  // -----------------------------------------------------------------------------------------------

  // SETUP TIME FUNCTIONS --------------------------------------------------------------------------

  /**
   * @brief CreateGroup - Collect multiple states into a single group for specifying group
   * transitions Each group must be either a subset of any existing group, or have zero overlap. Two
   * groups cannot intersect without one being a subset of the other. Eg. _________ ________ | _____
   * |                      |    ___|___ | |A B| C |  <-this is fine      | A  |B | C|  <- This is
   * not fine | -----   |                      |    ---|---
   *     ---------                        -------
   * @param group_name
   * @param states
   * @return
   */
  bool CreateGroup(std::string group_name, std::set<StateType> states);

  /**
   * @brief SetPriorities - When there are multiple interrupts, or an interrupt and result at the
   * same time, the event loop needs to decide which one to honor. This sets up the priorities for
   * all Results and Interrupts. Each priority must be unique. The higher the number, the higher the
   * priority
   * @param result_prio
   * @param interrupt_prio
   * @return
   */
  bool SetPriorities(const std::map<ResultType, int>& result_prio,
                     const std::map<InterruptType, int>& interrupt_prio);

  /**
   * @brief SetInitState - Sets the initial state of the state machine, this is only valid when
   * stopped
   * @param state - the enumerate state
   */
  void SetInitState(StateType state);

  /**
   * @brief SetMaxFrequency for states which are repeated (start and returning to the same state),
   * this will throttle how quickly we dispatch states to avoid hammering the CPU. Transitions to
   * new states will NOT throttle.
   */
  bool SetMaxFrequency(double frequency);

  /**
   * @brief AttachStateFunc - Attach a callable corresponding to a state or class with overloaded
   * @param state - the enumerated state to attach the callable to
   * @param state_func - a callable to be called when we enter this state, MUST be non blocking!
   */
  void AttachStateFunc(StateType state, const StateFunc& state_func);

  /**
   * @brief AddTransition - Adds an internal transition for the state machine based on curr state
   * and result from that state
   * @param curr_state - the current state that were executing
   * @param result - the outcome of that state
   * @param next_state - The next state we should go to for this [state/result] pair
   */
  void AddTransition(StateType curr_state, ResultType result, StateType next_state);

  /**
   * @brief AddTransition - same as other overload, but with a group instead of StateType. Must add
   * group before calling this, or this does nothing.
   * @return false if the group name doesn't exist, true otherwise
   */
  bool AddTransition(std::string group_name, ResultType result, StateType next_state);

  /**
   * @brief AddTransition - Adds a transition due to external event based on current state
   * @param curr_state
   * @param interrupt
   * @param next_state
   */
  void AddTransition(StateType curr_state, InterruptType interrupt, StateType next_state);

  /**
   * @brief AddTransition - same as other overload, but with a group instead of StateType. Must add
   * group before calling this, or this does nothing.
   * @return false if the group name doesn't exist, true otherwise
   */
  bool AddTransition(std::string group_name, InterruptType interrupt, StateType next_state);

  // -----------------------------------------------------------------------------------------------

  /**
   * @brief VisualizeStateMachine - visualizes all states transitions in the state machine,
   * collapsing any group transitions into a single arrow
   */
  void VisualizeStateMachine() const;

  StateType GetState()
      const;             //!< retreive current state (may right after call, beware race conditions)
  Mode GetMode() const;  //!< tells you if this state machine is running or note
  StateType GetInitState() const;  //!< Get starting state of this state machine
  std::map<StateType, StateFunc> GetStateFuncs() const;
  std::map<std::pair<StateType, ResultType>, StateType> GetTransitionMap() const;
  std::map<std::string, std::set<StateType>> GetGroups() const;
  std::map<std::pair<std::string, ResultType>, StateType> GetGroupTransitionMap() const;

 private:
  void EventLoop_();  //!< runs periodically, dispatching the approrpriate state every step

  bool Validate_();  //!< check all transitions to make sure we have an attach state function for
                     //!< every state

  StateType curr_state_;
  StateType init_state_;

  /// All interrupts
  std::mutex event_loop_mutex_;
  std::map<InterruptType, std::shared_ptr<ExternalPayload>> interrupts_;

  std::map<StateType, StateFunc> state_funcs_;

  /// collect states into groups for visualization and to specify a large number of transitions at
  /// once
  std::map<std::string, std::set<StateType>> state_groups_;

  /// This contains every single possible transition in this state machine
  std::map<std::pair<StateType, ResultType>, StateType> transition_map_;

  /// This is for visualization only, not for runtime usage
  std::map<std::pair<std::string, ResultType>, StateType> group_transition_map_;

  /// The priorities of all results and interrupts, each value in the combined map must be unique
  std::map<ResultType, int> result_priorities_;
  std::map<InterruptType, int> interrupt_priorities_;

  InternalData internal_data_;  //!< This gets passed by ref to each state every time they're called

  std::map<StateType, ResultType> all_results_;
};

}  // namespace utils
}  // namespace nil