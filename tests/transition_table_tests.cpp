
#include "fsm/detail/meta.hpp"
#include "fsm/detail/transition_table.hpp"
#include "fsm/detail/typelist.hpp"
#include "fsm/dsl.hpp"
#include "fsm/utils/stl_substitutes.hpp"

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
  virtual void buh(Working&, const ErrorEvent&, Error&) = 0;
};

using namespace fsm;

struct MyStateMachine {
  static constexpr auto transition_table() {
    using namespace fsm;
    return make_transition_table(
        make_transition(state<Idle>, event<TaskStarted>, state<Working>),
        make_transition(state<Working>, event<TaskDone>, state<Idle>,
                        context<Foo>, &Foo::bar),
        make_transition(state<Idle>, event<ErrorEvent>, state<Error>),
        make_transition(state<Working>, event<ErrorEvent>, state<Error>,
                        context<Foo>, &Foo::buh));
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
  STATIC_REQUIRE(detail::has_type<States, Idle>);
  STATIC_REQUIRE(detail::has_type<States, Working>);
  STATIC_REQUIRE(detail::has_type<States, Error>);
}

TEST_CASE("extract contexts from transition table") {
  using TT = detail::transition_table_from_config<MyStateMachine>;
  using Contexts = detail::extract_contexts<TT>;
  STATIC_REQUIRE(Contexts::size == 1);
  STATIC_REQUIRE(detail::has_type<Contexts, Foo>);
}

TEST_CASE("default initial state is first source state in table") {
  using TT = detail::transition_table_from_config<MyStateMachine>;
  using InitialState = detail::extract_initial_state<TT>;
  STATIC_REQUIRE(std::is_same_v<InitialState, Idle>);
}
