#pragma once

#include <fsm/detail/stateholder.hpp>
#include <fsm/detail/statemachine.hpp>

namespace fsm {

template <typename SMConfig, typename Dep = detail::NoDependency>
class StateMachine {
  using TransitionTable = detail::transition_table_from_config<SMConfig>;
  using States = detail::extractStates_t<TransitionTable>;
  using InitialState = detail::extract_initial_state<TransitionTable>;
  using StateLookup = decltype(detail::make_state_dispatch_lookup(
      SMConfig::transition_table()));

 public:
  StateMachine()
      : eventDispatch_(
            detail::make_state_dispatch_lookup(SMConfig::transition_table())) {}

  template <typename D = Dep>
  explicit StateMachine(D dep)
      : dependency_(dep),
        eventDispatch_(
            detail::make_state_dispatch_lookup(SMConfig::transition_table())) {}

  template <typename Event>
  void processEvent(const Event &event) {
    detail::visit(
        eventDispatch_, currentStateIndex_, [this, event](auto &currentState) {
          using Transitions = decltype(currentState.transitions);
          using Source = decltype(currentState.rawState);
          using Match = detail::matchTransition_t<typename Transitions::TL,
                                                  Source, Event>;
          if constexpr (utils::is_same_v<Match, detail::NoMatch>) {
            return;
          } else {
            constexpr auto matchTransitionIndex =
                index_of<typename Transitions::TL, Match>;
            static_assert(matchTransitionIndex > -1);
            using Target = typename Match::Target;
            constexpr auto targetIndex = index_of<States, Target>;
            static_assert(targetIndex > -1);
            detail::get<matchTransitionIndex>(currentState.transitions)
                .execute(currentState.rawState, event,
                         detail::get<targetIndex>(eventDispatch_).rawState);
            currentStateIndex_ = targetIndex;
          }
        });
  }

  template <typename State>
  constexpr bool isState() {
    return currentStateIndex_ == index_of<States, State>;
  }

 private:
  StateLookup eventDispatch_;
  detail::DependencyHolder<Dep> dependency_;
  unsigned int currentStateIndex_ = index_of<States, InitialState>;
};
}  // namespace fsm