#pragma once

namespace fsm {
namespace detail {
template <auto... Idx>
struct IntegerSequence {};

template <auto v0, decltype(v0)... Idx>
struct IntegerSequence<v0, Idx...> {
  using value_type = decltype(v0);
  using type = IntegerSequence;
};

template <>
struct IntegerSequence<> {
  using type = IntegerSequence;
};

template <class L, auto... Is>
struct push_index_front_impl;

template <template <auto...> class L, auto... Us, auto... Is>
struct push_index_front_impl<L<Us...>, Is...> {
  using type = L<Is..., Us...>;
};

template <class L, auto... T>
using push_index_front = typename push_index_front_impl<L, T...>::type;

template <auto Current, decltype(Current) End>
struct make_range_impl {
  using L = typename make_range_impl<Current + 1, End>::type;
  using type = typename push_index_front_impl<L, Current>::type;
};

template <auto Current>
struct make_range_impl<Current, Current> {
  using type = IntegerSequence<>;
};

template <auto End>
using make_integer_range =
    typename make_range_impl<decltype(End)(0), End>::type;

}  // namespace detail
}  // namespace fsm