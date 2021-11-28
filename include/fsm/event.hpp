#pragma once

namespace fsm {

namespace detail {
template <typename E, typename A>
struct transition_builder_ea {
  constexpr transition_builder_ea(A a) : action_(a) {}

  constexpr auto action() const { return action_; }

  A action_;
};

template <class T, class = void>
struct ActionWrapper : T {};

}  // namespace detail

template <typename E>
struct Event {
  template <typename F>
  constexpr auto operator/(const F &f) const {
    return detail::transition_builder_ea<Event<E>, F>{f};
  }
};

template <typename E>
const Event<E> event{

};
}  // namespace fsm
