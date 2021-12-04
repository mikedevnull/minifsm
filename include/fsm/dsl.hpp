#pragma once

#include <fsm/detail/transition_table.hpp>
#include <fsm/detail/transitions.hpp>
#include <fsm/types.hpp>

namespace fsm {

template <typename S>
const State<S> state{};

template <typename E>
const Event<E> event{};

template <typename C>
const Context<C> context{};

template <typename... Ts>
constexpr auto make_transition_table(Ts &&...ts) {
  return detail::TransitionTable<Ts...>{utils::forward<Ts>(ts)...};
}

template <typename... Ts>
constexpr auto make_transition(Ts &&...ts) {
  return detail::make_transition(utils::forward<Ts>(ts)...);
}

}  // namespace fsm
