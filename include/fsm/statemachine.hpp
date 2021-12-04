#pragma once

#include <fsm/detail/stateholder.hpp>
#include <fsm/detail/statemachine.hpp>

namespace fsm {

template <typename SMConfig, typename Dep = detail::NoDependency>
class StateMachine {
  using TransitionTable = detail::transition_table_from_config<SMConfig>;
  using StateList = detail::extractStates_t<TransitionTable>;
  using States = rebind<StateList, detail::Tuple>;
  using InitialState = detail::extract_initial_state<TransitionTable>;
  using ContextList = detail::extract_contexts<TransitionTable>;
  using ContextHoldingTypes = transform<ContextList, utils::add_pointer_t>;
  using Contexts = rebind<ContextHoldingTypes, detail::Tuple>;

 public:
  template <typename... Ts>
  constexpr explicit StateMachine(Ts &&...deps)
      : contexts_(utils::forward<Ts>(deps)...),
        states_{},
        tt_{SMConfig::transition_table()} {
    static_assert(sizeof...(Ts) == ContextList::size);
  }

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
            using Context = typename Match::Context;
            if constexpr (utils::is_same_v<Context, detail::NoContext>) {
              detail::get<matchTransitionIndex>(tt_.transitions)
                  .execute(currentState, event,
                           detail::get<targetIndex>(states_));
            } else {
              constexpr auto contextIndex =
                  index_of<typename Contexts::TL,
                           utils::add_pointer_t<Context>>;
              static_assert(contextIndex > -1);
              detail::get<matchTransitionIndex>(tt_.transitions)
                  .execute(detail::get<contextIndex>(contexts_), currentState,
                           event, detail::get<targetIndex>(states_));
            }
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
  Contexts contexts_;
  unsigned int currentStateIndex_ = index_of<typename States::TL, InitialState>;
};
}  // namespace fsm