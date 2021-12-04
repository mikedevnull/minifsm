#pragma once

namespace fsm {
template <typename T>
struct State {
  using type = T;
};

template <typename E>
struct Event {
  using type = E;
};

template <typename T>
struct Context {
  using type = T;
};
}