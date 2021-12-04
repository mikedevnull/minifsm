#include "fsm/statemachine.hpp"
#include "fsm/transition_table.hpp"

#include <catch2/catch.hpp>
#include <fakeit.hpp>

// states
struct Idle {};
struct Working {};
struct Error {};

// events
struct TaskStarted {};
struct ErrorEvent {};
struct TaskDone {};
struct Reset {};

struct BasicStateMachine {
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
  fsm::StateMachine<BasicStateMachine> x;
  REQUIRE(x.isState<Idle>());
  x.processEvent(TaskStarted{});
  REQUIRE(x.isState<Working>());
}

TEST_CASE("events without transition for current state are ignored",
          "[frontend]") {
  fsm::StateMachine<BasicStateMachine> x;
  REQUIRE(x.isState<Idle>());
  x.processEvent(Reset{});
  REQUIRE(x.isState<Idle>());
}

namespace {
bool lambdaActionCalled = false;
bool functorCalled = false;
bool freeFunctionCalled = false;

void freeAction(const Working&, const TaskDone&, const Idle&) {
  freeFunctionCalled = true;
};
struct Functor {
  void operator()(const Idle&, const ErrorEvent&, const Error&) {
    functorCalled = true;
  }
};
auto lambda_action = [](auto s, auto e, auto t) { lambdaActionCalled = true; };
struct StateMachineWithActions {
  static constexpr auto transition_table() {
    using namespace fsm;

    return make_transition_table(
        make_transition(state<Idle>, event<TaskStarted>, state<Working>,
                        lambda_action),
        make_transition(state<Idle>, event<ErrorEvent>, state<Error>,
                        Functor{}),
        make_transition(state<Working>, event<TaskDone>, state<Idle>,
                        freeAction),
        make_transition(state<Working>, event<ErrorEvent>, state<Error>),
        make_transition(state<Error>, event<Reset>, state<Idle>));
  }
};
}  // namespace

TEST_CASE("state machine calls actions", "[frontend]") {
  fsm::StateMachine<StateMachineWithActions> x;
  lambdaActionCalled = false;
  functorCalled = false;
  freeFunctionCalled = false;

  SECTION("lambdas as actions will be called") {
    REQUIRE(x.isState<Idle>());
    REQUIRE_FALSE(lambdaActionCalled);
    x.processEvent(TaskStarted{});

    REQUIRE(lambdaActionCalled);

    REQUIRE(x.isState<Working>());
  }

  SECTION("free functions as actions will be called") {
    x.processEvent(TaskStarted{});
    REQUIRE_FALSE(freeFunctionCalled);
    x.processEvent(TaskDone{});

    REQUIRE(freeFunctionCalled);

    REQUIRE(x.isState<Idle>());
  }
  SECTION("functors as actions will be called") {
    REQUIRE_FALSE(functorCalled);
    x.processEvent(ErrorEvent{});

    REQUIRE(functorCalled);

    REQUIRE(x.isState<Error>());
  }
}

namespace {
class Foo {
 public:
  virtual void doFoo(const Idle&, const TaskStarted&, const Working&) = 0;
};

struct StateMachineWithActionsAndContext {
  static constexpr auto transition_table() {
    using namespace fsm;

    return make_transition_table(
        make_transition(state<Idle>, event<TaskStarted>, state<Working>,
                        context<Foo>, &Foo::doFoo),
        make_transition(state<Idle>, event<ErrorEvent>, state<Error>,
                        Functor{}),
        make_transition(state<Working>, event<TaskDone>, state<Idle>,
                        freeAction),
        make_transition(state<Working>, event<ErrorEvent>, state<Error>),
        make_transition(state<Error>, event<Reset>, state<Idle>));
  }
};
}  // namespace

TEST_CASE("State machine with actions and context", "[frontend]") {
  using namespace fakeit;
  Mock<Foo> mock;
  When(Method(mock, doFoo)).Return();

  auto& foo = mock.get();

  fsm::StateMachine<StateMachineWithActionsAndContext> x{&foo};

  x.processEvent(TaskStarted{});
  REQUIRE(x.isState<Working>());
  Verify(Method(mock, doFoo));
}