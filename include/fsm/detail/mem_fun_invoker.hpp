#pragma once
#include "fsm/detail/type_utils.hpp"
#include "fsm/substitutes.hpp"

namespace fsm {
namespace detail {

template <typename MemFunPtr, typename C>
struct MemFunInvoker {
  MemFunInvoker(C& o, MemFunPtr f) : o_(o), f_(f) {}

  template <typename... Args>
  auto operator()(Args&&... args) {
    return (o_.*f_)(utils::forward<Args>(args)...);
  }

  C& o_;
  MemFunPtr f_;
};

template <typename T>
struct MemFunInvokerFactory;

template <typename R, typename C, typename... Args>
struct MemFunInvokerFactory<R (C::*)(Args...)> {
  template <R (C::*Func)(Args...)>
  static MemFunInvoker<R (C::*)(Args...), C> create(
      C& o, mem_fun_sig_t<R, C, Args...> f) {
    return MemFunInvoker<R (C::*)(Args...), C>(o, f);
  }
};

template <typename R, typename C, typename... Args>
struct MemFunInvokerFactory<R (C::*)(Args...) const> {
  using Func = R (C::*)(Args...) const;
  template <R (C::*Func)(Args...) const>
  static MemFunInvoker<R (C::*)(Args...) const, const C> create(
      const C& o, const_mem_fun_sig_t<R, C, Args...> f) {
    return MemFunInvoker<R (C::*)(Args...) const, const C>(o, f);
  }
};

template <typename R, typename C, typename... Args>
auto createInvoker(const C& o, const_mem_fun_sig_t<R, C, Args...> f) {
  using F = const_mem_fun_sig_t<R, C, Args...>;
  return MemFunInvoker<F, const C>{o, f};
}

template <typename R, typename C, typename... Args>
auto createInvoker(C& o, mem_fun_sig_t<R, C, Args...> f) {
  using F = mem_fun_sig_t<R, C, Args...>;
  return MemFunInvoker<F, C>{o, f};
}
}  // namespace detail
}  // namespace fsm