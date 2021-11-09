#pragma once

#include <fsm/detail/stateholder.hpp>
#include <fsm/detail/statemachine.hpp>
#include <fsm/detail/visitors.hpp>

namespace fsm {

template <typename States, typename Dep = detail::NoDependency>
class StateMachine {
  using StateTypes = States;

 public:
  StateMachine() {}
  template <typename D = Dep>
  explicit StateMachine(D dep) : dependency_(dep) {}
  template <typename Event>
  void processEvent(const Event &event) {
    auto visitor =
        detail::createOnEventVisitor(event, *this, dependency_.value);
    visit(currentStateIndex_, states_, visitor);
  }

  template <typename State>
  constexpr bool isState() {
    return currentStateIndex_ == IndexOf<StateTypes, State>::value;
  }

  template <typename TargetState, typename Event>
  void transitionTo(const Event &event) {
    constexpr auto targetIndex = IndexOf<StateTypes, TargetState>::value;
    static_assert(targetIndex >= 0, "Unkown target state");
    if (targetIndex == currentStateIndex_) {
      return;
    }
    // visit(currentStateIndex_, states_,
    //       ExitStateVisitor{event, states_.template get<targetIndex>()});
    // visit(currentStateIndex_, states_,
    //       EnterStateVisitor{event, states_.template get<targetIndex>(),
    //                         dependency_});
    currentStateIndex_ = targetIndex;
  }

 private:
  detail::StateHolder<States> states_;
  detail::DependencyHolder<Dep> dependency_;
  unsigned int currentStateIndex_ = 0;
};
}  // namespace fsm