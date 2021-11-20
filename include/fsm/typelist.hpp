#pragma once
#include <fsm/detail/meta.hpp>
#include <fsm/substitutes.hpp>

namespace fsm {

template <typename... Types>
struct TypeList {
  static constexpr auto size = sizeof...(Types);
};

template <typename TypeList, unsigned int N>
struct type_at_impl;

template <typename Head, typename... Tail>
struct type_at_impl<TypeList<Head, Tail...>, 0> {
  using type = Head;
};

template <typename Head, typename... Tail, unsigned int N>
struct type_at_impl<TypeList<Head, Tail...>, N> {
  static_assert(N < sizeof...(Tail) + 1, "N out of range");
  using type = typename type_at_impl<TypeList<Tail...>, N - 1>::type;
};

template <typename TL, unsigned int N>
using type_at = typename type_at_impl<TL, N>::type;

template <typename TypeList, typename TargetState>
struct index_of_impl;

template <typename T, typename... Ts>
struct index_of_impl<TypeList<T, Ts...>, T> {
  static constexpr int value = 0;
};

template <typename Head, typename T, typename... Tail>
struct index_of_impl<TypeList<Head, Tail...>, T> {
  static constexpr int value =
      index_of_impl<TypeList<Tail...>, T>::value == -1
          ? -1
          : index_of_impl<TypeList<Tail...>, T>::value + 1;
};

template <typename T>
struct index_of_impl<TypeList<>, T> {
  static constexpr int value = -1;
};

template <typename TP, typename T>
constexpr auto index_of = index_of_impl<TP, T>::value;

template <typename TL, typename T>
struct push_back_impl;

template <typename T, typename... Ts>
struct push_back_impl<TypeList<Ts...>, T> {
  using type = TypeList<Ts..., T>;
};

template <typename TL, typename T>
using push_back = typename push_back_impl<TL, T>::type;

template <typename TL, typename T>
constexpr auto has_type = detail::_if_c<index_of<TL, T> != -1>::value;

template <typename TL, template <typename...> typename F>
struct transform_impl;

template <template <typename...> typename TL, template <typename...> typename F,
          typename... T>
struct transform_impl<TL<T...>, F> {
  using type = TL<F<T>...>;
};

template <typename TL, template <typename...> typename F>
using transform = typename transform_impl<TL, F>::type;

template <typename A, typename TL, auto Start, decltype(Start) End>
struct subrange_impl;

template <typename A, typename... Ts, auto Start, decltype(Start) End>
struct subrange_impl<A, TypeList<Ts...>, Start, End> {
  using T = push_back<A, type_at<TypeList<Ts...>, Start>>;
  using type = typename subrange_impl<T, TypeList<Ts...>, Start + 1, End>::type;
};

template <typename A, typename... Ts, auto End>
struct subrange_impl<A, TypeList<Ts...>, End, End> {
  using type = A;
};

template <typename TL, auto Start, decltype(Start) End>
using subrange = typename subrange_impl<TypeList<>, TL, Start, End>::type;
}  // namespace fsm