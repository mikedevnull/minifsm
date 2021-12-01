#include "fsm/detail/transitions.hpp"

#include <catch2/catch.hpp>
#include <type_traits>

struct AState {};
struct AnEvent {};
struct AnotherState {};

TEST_CASE("create transition without action", "[core]") {
  using fsm::detail::make_transition;
  auto transition = make_transition(fsm::state<AState>, fsm::event<AnEvent>,
                                    fsm::state<AnotherState>);
  using T = decltype(transition);
  STATIC_REQUIRE(std::is_same_v<decltype(transition.source()), AState>);
  STATIC_REQUIRE(std::is_same_v<T::Source, AState>);
  STATIC_REQUIRE(std::is_same_v<decltype(transition.event()), AnEvent>);
  STATIC_REQUIRE(std::is_same_v<T::Event, AnEvent>);
  STATIC_REQUIRE(std::is_same_v<decltype(transition.target()), AnotherState>);
  STATIC_REQUIRE(std::is_same_v<T::Target, AnotherState>);
}

TEST_CASE("create transition with lambda as action", "[core]") {
  using fsm::detail::make_transition;
  bool invoked = false;
  auto transition = make_transition(
      fsm::state<AState>, fsm::event<AnEvent>, fsm::state<AnotherState>,
      [&invoked](auto source, auto event, auto target) { invoked = true; });
  using T = decltype(transition);
  STATIC_REQUIRE(std::is_same_v<decltype(transition.source()), AState>);
  STATIC_REQUIRE(std::is_same_v<T::Source, AState>);
  STATIC_REQUIRE(std::is_same_v<decltype(transition.event()), AnEvent>);
  STATIC_REQUIRE(std::is_same_v<T::Event, AnEvent>);
  STATIC_REQUIRE(std::is_same_v<decltype(transition.target()), AnotherState>);
  STATIC_REQUIRE(std::is_same_v<T::Target, AnotherState>);

  auto s = AState{};
  auto t = AnotherState{};

  REQUIRE_FALSE(invoked);
  transition.execute(s, AnEvent{}, t);
  REQUIRE(invoked);
}

static bool free_invoked = false;
void free_function(AState, AnEvent, AnotherState) { free_invoked = true; }

TEST_CASE("create transition with free function as action", "[core]") {
  using fsm::detail::make_transition;

  auto transition = make_transition(fsm::state<AState>, fsm::event<AnEvent>,
                                    fsm::state<AnotherState>, free_function);
  using T = decltype(transition);
  STATIC_REQUIRE(std::is_same_v<decltype(transition.source()), AState>);
  STATIC_REQUIRE(std::is_same_v<T::Source, AState>);
  STATIC_REQUIRE(std::is_same_v<decltype(transition.event()), AnEvent>);
  STATIC_REQUIRE(std::is_same_v<T::Event, AnEvent>);
  STATIC_REQUIRE(std::is_same_v<decltype(transition.target()), AnotherState>);
  STATIC_REQUIRE(std::is_same_v<T::Target, AnotherState>);

  auto s = AState{};
  auto t = AnotherState{};

  REQUIRE_FALSE(free_invoked);
  transition.execute(s, AnEvent{}, t);

  REQUIRE(free_function);
}

bool foo_called = false;
bool bar_called = false;
bool foo_c_called = false;
bool bar_c_called = false;

struct CallableTester {
  void foo(const AState&, const AnEvent&, const AnotherState&) {
    foo_called = true;
  }

  void bar(AState&, const AnEvent&, AnotherState&) { bar_called = true; }

  void foo_c(const AState&, const AnEvent&, const AnotherState&) const {
    foo_c_called = true;
  }

  void bar_c(AState&, const AnEvent&, AnotherState&) const {
    bar_c_called = true;
  }
};

TEST_CASE(
    "create transition with non-const member function and const arguments",
    "[core]") {
  using fsm::detail::make_transition;

  auto transition = make_transition(
      fsm::state<AState>, fsm::event<AnEvent>, fsm::state<AnotherState>,
      fsm::context<CallableTester>, &CallableTester::foo);

  using T = decltype(transition);
  STATIC_REQUIRE(std::is_same_v<decltype(transition.source()), AState>);
  STATIC_REQUIRE(std::is_same_v<T::Source, AState>);
  STATIC_REQUIRE(std::is_same_v<decltype(transition.event()), AnEvent>);
  STATIC_REQUIRE(std::is_same_v<T::Event, AnEvent>);
  STATIC_REQUIRE(std::is_same_v<decltype(transition.target()), AnotherState>);
  STATIC_REQUIRE(std::is_same_v<T::Target, AnotherState>);

  auto s = AState{};
  auto t = AnotherState{};

  CallableTester tester;

  REQUIRE_FALSE(foo_called);
  transition.execute(tester, s, AnEvent{}, t);
  REQUIRE(foo_called);
}

TEST_CASE(
    "create transition with non-const member function and non-const arguments",
    "[core]") {
  using fsm::detail::make_transition;

  auto transition = make_transition(
      fsm::state<AState>, fsm::event<AnEvent>, fsm::state<AnotherState>,
      fsm::context<CallableTester>, &CallableTester::bar);

  using T = decltype(transition);
  STATIC_REQUIRE(std::is_same_v<decltype(transition.source()), AState>);
  STATIC_REQUIRE(std::is_same_v<T::Source, AState>);
  STATIC_REQUIRE(std::is_same_v<decltype(transition.event()), AnEvent>);
  STATIC_REQUIRE(std::is_same_v<T::Event, AnEvent>);
  STATIC_REQUIRE(std::is_same_v<decltype(transition.target()), AnotherState>);
  STATIC_REQUIRE(std::is_same_v<T::Target, AnotherState>);

  auto s = AState{};
  auto t = AnotherState{};

  CallableTester tester;

  REQUIRE_FALSE(bar_called);
  transition.execute(tester, s, AnEvent{}, t);
  REQUIRE(bar_called);
}

TEST_CASE("create transition with const member function and const arguments",
          "[core]") {
  using fsm::detail::make_transition;

  auto transition = make_transition(
      fsm::state<AState>, fsm::event<AnEvent>, fsm::state<AnotherState>,
      fsm::context<CallableTester>, &CallableTester::foo_c);

  using T = decltype(transition);
  STATIC_REQUIRE(std::is_same_v<decltype(transition.source()), AState>);
  STATIC_REQUIRE(std::is_same_v<T::Source, AState>);
  STATIC_REQUIRE(std::is_same_v<decltype(transition.event()), AnEvent>);
  STATIC_REQUIRE(std::is_same_v<T::Event, AnEvent>);
  STATIC_REQUIRE(std::is_same_v<decltype(transition.target()), AnotherState>);
  STATIC_REQUIRE(std::is_same_v<T::Target, AnotherState>);

  auto s = AState{};
  auto t = AnotherState{};

  const CallableTester tester;

  REQUIRE_FALSE(foo_c_called);
  transition.execute(tester, s, AnEvent{}, t);
  REQUIRE(foo_c_called);
}

TEST_CASE(
    "create transition with const member function and non-const arguments",
    "[core]") {
  using fsm::detail::make_transition;

  auto transition = make_transition(
      fsm::state<AState>, fsm::event<AnEvent>, fsm::state<AnotherState>,
      fsm::context<CallableTester>, &CallableTester::bar_c);

  using T = decltype(transition);
  STATIC_REQUIRE(std::is_same_v<decltype(transition.source()), AState>);
  STATIC_REQUIRE(std::is_same_v<T::Source, AState>);
  STATIC_REQUIRE(std::is_same_v<decltype(transition.event()), AnEvent>);
  STATIC_REQUIRE(std::is_same_v<T::Event, AnEvent>);
  STATIC_REQUIRE(std::is_same_v<decltype(transition.target()), AnotherState>);
  STATIC_REQUIRE(std::is_same_v<T::Target, AnotherState>);

  auto s = AState{};
  auto t = AnotherState{};

  const CallableTester tester;

  REQUIRE_FALSE(bar_c_called);
  transition.execute(tester, s, AnEvent{}, t);
  REQUIRE(bar_c_called);
}