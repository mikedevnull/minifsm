#pragma once
#include "fsm/substitutes.hpp"

namespace fsm {
namespace detail {

template <typename F, typename... Args>
struct Binder {};

template <typename F, typename Arg, typename... Args>
struct Binder<F, Arg, Args...> : public Binder<F, Args...> {
  using Base = Binder<F, Args...>;

  Binder(F f, Arg&& arg, Args&&... args)
      : Binder<F, Args...>(f, utils::forward<Args>(args)...),
        _value(utils::forward<Arg>(arg)) {}

  void operator()() { Base::invoke(utils::forward<Arg>(_value)); }

 protected:
  template <typename... Params>
  void invoke(Params&&... params) {
    return Base::invoke(utils::forward<Params>(params)...,
                        utils::forward<Arg>(_value));
  }

  Arg&& _value;
};

template <typename F>
struct Binder<F> {
  Binder(F f) : _f(f) {}

 protected:
  template <typename... Params>
  void invoke(Params&&... params) {
    _f(utils::forward<Params>(params)...);
  }

  F _f;
};

template <typename F, typename... Args>
Binder<F, Args...> bind(F f, Args&&... args) {
  return Binder<F, Args...>{f, utils::forward<Args>(args)...};
}

}  // namespace detail
}  // namespace fsm