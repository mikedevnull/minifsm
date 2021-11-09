#pragma once

#include "fsm/substitutes.hpp"
#include "fsm/typelist.hpp"

namespace fsm {
namespace detail {

template <typename T>
struct StateValue {
  StateValue() = default;
  explicit StateValue(T &&v) : value_(utils::move(v)) {}
  T value_;
};

template <typename... States>
struct StateHolder;

template <typename... States>
struct StateHolder<TypeList<States...>> : public StateValue<States>... {
  static constexpr unsigned int size = sizeof...(States);
  using StateTypes = TypeList<States...>;

  template <unsigned int N>
  const auto &get() const {
    static_assert(N < sizeof...(States), "index out of range");
    return get<typename TypeAt<StateTypes, N>::type>();
  }

  template <typename T>
  const auto &get() const {
    return StateValue<T>::value_;
  }

  template <unsigned int N>
  auto &get() {
    static_assert(N < sizeof...(States), "index out of range");
    return get<typename TypeAt<StateTypes, N>::type>();
  }

  template <typename T>
  auto &get() {
    return StateValue<T>::value_;
  }
};

template <unsigned int N, typename Data, typename F>
void visitImpl(unsigned int idx, Data &states, F func) {
  if constexpr (N == 0) {
    return;
  } else if (idx == N - 1) {
    func(states.template get<N - 1>());
  } else {
    visitImpl<N - 1>(idx, states, func);
  }
}

template <typename... States, typename F>
void visit(unsigned int index, StateHolder<States...> &states, F func) {
  visitImpl<StateHolder<States...>::size>(index, states, func);
}

}  // namespace detail
}  // namespace fsm