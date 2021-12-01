#include "fsm/statemachine.hpp"
#include "fsm/transition_table.hpp"

#include <catch2/catch.hpp>

// states
struct Idle {};
struct Working {};
struct Error {};

// events
struct TaskStarted {};
struct ErrorEvent {};
struct TaskDone {};
struct Reset {};

using States = fsm::TypeList<Idle, Working, Error>;

template <typename C>
struct Context {};

template <typename C>
const Context<C> context{};

struct MyStateMachine {
  static constexpr auto transition_table() {
    using namespace fsm;
    return make_transition_table(
        make_transition(state<Idle>, event<TaskStarted>, state<Working>),
        make_transition(state<Idle>, event<ErrorEvent>, state<Error>),
        make_transition(state<Working>, event<TaskDone>, state<Idle>),
        make_transition(state<Working>, event<ErrorEvent>, state<Error>),
        make_transition(state<Error>, event<Reset>, state<Idle>));
  }
};

TEST_CASE("state machine simple state transition", "[frontend]") {
  fsm::StateMachine<MyStateMachine> x;
  REQUIRE(x.isState<Idle>());
  x.processEvent(TaskStarted{});
  REQUIRE(x.isState<Working>());
}