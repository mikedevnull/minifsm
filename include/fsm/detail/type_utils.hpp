#pragma once

namespace fsm {
namespace detail {
template <typename R, typename C, typename... Args>
struct mem_fun_sig {
  using type = R (C::*)(Args...);
};

template <typename R, typename C, typename... Args>
struct const_mem_fun_sig {
  using type = R (C::*)(Args...) const;
};

template <typename R, typename C, typename... Args>
using mem_fun_sig_t = typename mem_fun_sig<R, C, Args...>::type;

template <typename R, typename C, typename... Args>
using const_mem_fun_sig_t = typename const_mem_fun_sig<R, C, Args...>::type;
}  // namespace detail
}  // namespace fsm
