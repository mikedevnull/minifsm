#pragma once

namespace fsm {

template <typename T>
struct State {
  using type = T;
};

template <typename S>
const State<S> state{};

}  // namespace fsm
