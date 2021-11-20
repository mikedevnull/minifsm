#pragma once
#include <fsm/substitutes.hpp>

namespace fsm {
namespace detail {
template <typename T, typename F, bool>
struct _if;

template <typename T, typename F>
struct _if<T, F, true> {
  using type = T;
};

template <typename T, typename F>
struct _if<T, F, false> {
  using type = F;
};

template <typename T, typename F, bool C>
using _if_t = typename _if<T, F, C>::type;

template <bool C>
using _if_c = typename _if<utils::true_type, utils::false_type, C>::type;

template <typename T>
struct showTheType;

}  // namespace detail
}  // namespace fsm