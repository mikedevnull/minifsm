#pragma once
#include <fsm/detail/transitions.hpp>
#include <fsm/event.hpp>
#include <fsm/state.hpp>

#include <type_traits>
namespace fsm {

template <typename... Args>
struct TransitionTable {
  constexpr explicit TransitionTable(Args &&...args)
      : transitions(utils::forward<Args>(args)...){};
  using Transitions = fsm::TypeList<Args...>;
  fsm::detail::Tuple<Args...> transitions;

  constexpr auto getTransitions() { return transitions; }
};

template <typename... Ts>
constexpr auto make_transition_table(Ts &&...ts) {
  return TransitionTable<Ts...>{utils::forward<Ts>(ts)...};
}

template <typename... Ts>
constexpr auto make_transition(Ts &&...ts) {
  return detail::make_transition(utils::forward<Ts>(ts)...);
}

}  // namespace fsm