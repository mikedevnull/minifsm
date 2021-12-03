#pragma once

#include <fsm/detail/stateholder.hpp>
#include <fsm/detail/statemachine.hpp>

namespace fsm {

template <typename SMConfig, typename Dep = detail::NoDependency>
class StateMachine {
  using TransitionTable = detail::transition_table_from_config<SMConfig>;
  using StateList = detail::extractStates_t<TransitionTable>;
  using States =
      decltype(detail::tuple_from_typelist(utils::declval<StateList>()));
  using InitialState = detail::extract_initial_state<TransitionTable>;

 public:
  StateMachine() : states_{}, tt_{SMConfig::transition_table()} {}

  template <typename D = Dep>
  explicit StateMachine(D dep) : dependency_(dep) {}

  template <typename Event>
  void processEvent(const Event &event) {
    detail::visit(
        states_, currentStateIndex_, [this, event](auto &currentState) {
          using Transitions = decltype(tt_.transitions);

          using Source = utils::remove_cvref_t<decltype(currentState)>;
          using Match = detail::matchTransition_t<typename Transitions::TL,
                                                  Source, Event>;
          // detail::showTheType<Source> x;
          if constexpr (utils::is_same_v<Match, detail::NoMatch>) {
            return;
          } else {
            constexpr auto matchTransitionIndex =
                index_of<typename Transitions::TL, Match>;
            static_assert(matchTransitionIndex > -1);
            using Target = typename Match::Target;
            constexpr auto targetIndex = index_of<typename States::TL, Target>;
            static_assert(targetIndex > -1);
            detail::get<matchTransitionIndex>(tt_.transitions)
                .execute(currentState, event,
                         detail::get<targetIndex>(states_));
            currentStateIndex_ = targetIndex;
          }
        });
  }

  template <typename State>
  constexpr bool isState() {
    return currentStateIndex_ == index_of<typename States::TL, State>;
  }

 private:
  States states_;
  TransitionTable tt_;
  detail::DependencyHolder<Dep> dependency_;
  unsigned int currentStateIndex_ = index_of<typename States::TL, InitialState>;
};
}  // namespace fsm