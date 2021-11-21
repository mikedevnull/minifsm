#pragma once

namespace fsm {
namespace detail {
template <auto... Idx>
struct IntegerSequence;

template <auto v0, decltype(v0)... Idx>
struct IntegerSequence<v0, Idx...> {
  using value_type = decltype(v0);
  using type = IntegerSequence;
  static constexpr auto size = sizeof...(Idx) + 1;
};

template <>
struct IntegerSequence<> {
  using type = IntegerSequence;
  static constexpr auto size = 0;
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
  static_assert(Current < End);
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

template <auto Start, decltype(Start) End>
using make_integer_range_ = typename make_range_impl<Start, End>::type;

}  // namespace detail
}  // namespace fsm