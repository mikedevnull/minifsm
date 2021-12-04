#pragma once
#include <fsm/detail/integer_sequence.hpp>
#include <fsm/detail/meta.hpp>
#include <fsm/substitutes.hpp>

namespace fsm {

template <typename... Types>
struct TypeList {
  static constexpr unsigned int size = sizeof...(Types);
};

template <typename TypeList, unsigned int N>
struct type_at_impl;

template <typename Head, typename... Tail>
struct type_at_impl<TypeList<Head, Tail...>, 0u> {
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
constexpr auto has_type = index_of<TL, T> != -1 ? true : false;

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

template <auto... Is, auto... Us>
constexpr auto concat_sequence(detail::IntegerSequence<Is...>,
                               detail::IntegerSequence<Us...>) {
  return detail::IntegerSequence<Is..., Us...>{};
}

template <typename TL, auto Start, decltype(Start) End>
using subrange = typename subrange_impl<TypeList<>, TL, Start, End>::type;

template <template <typename> typename Pred, typename TL, typename...>
struct push_back_if_impl;

template <template <typename> typename Pred, typename TL, typename... Ts>
using push_back_if = typename push_back_if_impl<Pred, TL, Ts...>::type;

template <template <typename> typename Pred, typename TL, typename... Ts>
struct push_back_if_impl<Pred, TL, TypeList<Ts...>> {
  using type = typename push_back_if_impl<Pred, TL, Ts...>::type;
};

template <template <typename> typename Pred, typename TL, typename T,
          typename... Ts>
struct push_back_if_impl<Pred, TL, T, Ts...> {
  using type = utils::conditional_t<
      Pred<T>::value,
      typename push_back_if_impl<Pred, push_back<TL, T>, Ts...>::type,
      typename push_back_if_impl<Pred, TL, Ts...>::type>;
};

template <template <typename> typename Pred, typename TL>
struct push_back_if_impl<Pred, TL> {
  using type = TL;
};

template <template <typename> typename Pred, typename TL>
using filter_if = push_back_if<Pred, TypeList<>, TL>;

template <template <typename> typename Pred, typename TL>
struct remove_if_impl {
  template <typename T>
  struct _not_predicate
      : utils::conditional_t<Pred<T>::value, utils::false_type,
                             utils::true_type> {};

  using type = push_back_if<_not_predicate, TypeList<>, TL>;
};

template <template <typename> typename Pred, typename TL>
using remove_if = typename remove_if_impl<Pred, TL>::type;

template <template <typename> typename Pred, typename Result, auto N,
          typename... Ts>
constexpr auto find_all_if_impl(Result result, TypeList<Ts...>,
                                utils::integral_constant<N>) {
  using TL = TypeList<Ts...>;
  if constexpr (N == TL::size) {
    return result;
  } else if constexpr (Pred<type_at<TL, N>>::value) {
    return find_all_if_impl<Pred>(
        concat_sequence(result, detail::IntegerSequence<N>{}), TL{},
        utils::integral_constant<N + 1>{});
  } else {
    return find_all_if_impl<Pred>(result, TL{},
                                  utils::integral_constant<N + 1>{});
  }
}

template <template <typename> typename Pred, typename TL>
using find_all_if = decltype(find_all_if_impl<Pred>(
    detail::IntegerSequence<>{}, TL{}, utils::integral_constant<0>{}));

template <typename Pred, typename Result, auto N, typename... Ts>
constexpr auto find_all_if_callable_impl(Result result, Pred pred,
                                         TypeList<Ts...>,
                                         utils::integral_constant<N>) {
  using TL = TypeList<Ts...>;
  if constexpr (N == TL::size) {
    return result;
  } else if constexpr (pred(type_at<TL, N>{})) {
    return find_all_if_callable_impl(
        concat_sequence(result, detail::IntegerSequence<N>{}), pred, TL{},
        utils::integral_constant<N + 1>{});
  } else {
    return find_all_if_callable_impl(result, pred, TL{},
                                     utils::integral_constant<N + 1>{});
  }
}

template <typename Pred, typename... Ts>
constexpr auto find_all_if_callable(TypeList<Ts...> tl, Pred pred) {
  return find_all_if_callable_impl(detail::IntegerSequence<>{}, pred, tl,
                                   utils::integral_constant<0>{});
}

}  // namespace fsm