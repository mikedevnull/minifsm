#pragma once

#include "fsm/detail/transition_table.hpp"
#include "fsm/detail/tuple.hpp"
#include "fsm/state.hpp"
#include "fsm/substitutes.hpp"
#include "fsm/typelist.hpp"

namespace fsm {
namespace detail {

template <typename S, typename Tsl>
struct StateHolder {
  using RawState = S;
  using Transitions = Tsl;
  RawState rawState;
  Transitions transitions;
};

template <typename S, typename Transitions>
constexpr auto make_state_holder(S s, Transitions t) {
  auto v = S{};
  return StateHolder<S, Transitions>{v, t};
}

template <typename TT>
auto make_state_dispatch_lookup(TT &&tt) {
  using States = extractStates_t<TT>;
  auto states = detail::tuple_from_typelist(States{});

  return transform_tuple(states, [tt](auto state) {
    using State = utils::remove_cv_t<decltype(state)>;

    auto statetransitions =
        detail::filter_transitions_by_state<State>(tt.transitions);
    return make_state_holder(state, statetransitions);
  });
}

}  // namespace detail
}  // namespace fsm