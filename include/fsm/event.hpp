#pragma once

namespace fsm {

template <typename E>
struct Event {
  using type = E;
};

template <typename E>
const Event<E> event{};

}  // namespace fsm
