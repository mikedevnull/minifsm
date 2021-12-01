
#include "fsm/detail/meta.hpp"
#include "fsm/detail/stateholder.hpp"
#include "fsm/detail/type_utils.hpp"
#include "fsm/event.hpp"
#include "fsm/state.hpp"
#include "fsm/substitutes.hpp"
#include "fsm/transition_table.hpp"
#include "fsm/typelist.hpp"

#include <catch2/catch.hpp>
#include <fakeit.hpp>

// states
struct Idle {};
struct Working {
  int taskid = 2;
};
struct Error {};

// events
struct TaskStarted {};
struct ErrorEvent {};
struct TaskDone {};

class Foo {
 public:
  virtual void bar(Working&, const TaskDone&, Idle&) const = 0;
  virtual void baz(Working&, const TaskDone&, Idle&) = 0;
};

using namespace fsm;

struct MyStateMachine {
  static constexpr auto transition_table() {
    using namespace fsm;
    return make_transition_table(
        make_transition(state<Idle>, event<TaskStarted>, state<Working>),
        make_transition(state<Working>, event<TaskDone>, state<Idle>),
        make_transition(state<Idle>, event<ErrorEvent>, state<Error>),
        make_transition(state<Working>, event<ErrorEvent>, state<Error>));
  }
};

TEST_CASE("extract transition table from config") {
  using TT = detail::transition_table_from_config<MyStateMachine>;
  using Transitions = typename TT::Transitions;
  STATIC_REQUIRE(Transitions::size == 4);
}

TEST_CASE("extract states from transition table") {
  using TT = detail::transition_table_from_config<MyStateMachine>;
  using States = detail::extractStates_t<TT>;
  STATIC_REQUIRE(States::size == 3);
  STATIC_REQUIRE(has_type<States, Idle>);
  STATIC_REQUIRE(has_type<States, Working>);
  STATIC_REQUIRE(has_type<States, Error>);
}

TEST_CASE("default initial state is first source state in table") {
  using TT = detail::transition_table_from_config<MyStateMachine>;
  using InitialState = detail::extract_initial_state<TT>;
  STATIC_REQUIRE(std::is_same_v<InitialState, Idle>);
}

TEST_CASE("create state dispatcher from transition table") {
  using TT = detail::transition_table_from_config<MyStateMachine>;
  using States = detail::extractStates_t<TT>;
  auto states =
      detail::make_state_dispatch_lookup(MyStateMachine::transition_table());
  STATIC_REQUIRE(states.size == 3);

  auto sIdle = states.get<index_of<States, Idle>>();
  using TIdle = std::remove_reference_t<decltype(sIdle.rawState)>;
  STATIC_REQUIRE(std::is_same_v<TIdle, Idle>);

  auto sWorking = states.get<index_of<States, Working>>();
  using TWorking = std::remove_reference_t<decltype(sWorking.rawState)>;
  STATIC_REQUIRE(std::is_same_v<TWorking, Working>);

  auto sError = states.get<index_of<States, Error>>();
  using TError = std::remove_reference_t<decltype(sError.rawState)>;
  STATIC_REQUIRE(std::is_same_v<TError, Error>);
}

TEST_CASE("state dispatcher holds state values") {
  using TT = detail::transition_table_from_config<MyStateMachine>;
  using States = detail::extractStates_t<TT>;
  auto states =
      detail::make_state_dispatch_lookup(MyStateMachine::transition_table());

  constexpr auto WorkingStateIndex = index_of<States, Working>;
  REQUIRE(states.get<WorkingStateIndex>().rawState.taskid == 2);

  states.get<WorkingStateIndex>().rawState.taskid = 42;
  REQUIRE(states.get<WorkingStateIndex>().rawState.taskid == 42);
}

TEST_CASE("filter transition table by source state") {
  using TT = detail::transition_table_from_config<MyStateMachine>;
  constexpr auto tt = MyStateMachine::transition_table();
  constexpr auto idleTransitions =
      detail::filter_transitions_by_state<Idle>(tt.transitions);

  STATIC_REQUIRE(idleTransitions.size == 2);
}
