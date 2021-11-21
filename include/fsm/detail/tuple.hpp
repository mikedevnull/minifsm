#pragma once
#include "fsm/detail/integer_sequence.hpp"
#include "fsm/typelist.hpp"

namespace fsm {
namespace detail {

template <unsigned int N, typename T>
class TupleLeaf {
 public:
  constexpr TupleLeaf() = default;
  template <typename Arg>
  constexpr explicit TupleLeaf(Arg&& v) : value_(utils::forward<Arg>(v)) {}

  constexpr const T& get() const { return value_; }
  constexpr T& get() { return value_; }

 private:
  T value_;
};

template <typename... Ts>
using Indizes = make_integer_range<sizeof...(Ts)>;

template <typename Idx, typename... Ts>
class TupleImpl;

template <auto... Idx, typename... Ts>
class TupleImpl<IntegerSequence<Idx...>, Ts...> : public TupleLeaf<Idx, Ts>... {
 public:
  template <auto... ProvidedIs, typename... ProvidedTypes, auto... DefaultedIs,
            typename... DefaultedTypes, typename... Args>
  constexpr explicit TupleImpl(IntegerSequence<ProvidedIs...>,
                               TypeList<ProvidedTypes...>,
                               IntegerSequence<DefaultedIs...>,
                               TypeList<DefaultedTypes...>, Args&&... args)
      : TupleLeaf<ProvidedIs, ProvidedTypes>(utils::forward<Args>(args))...,
        TupleLeaf<DefaultedIs, DefaultedTypes>()... {}

  template <typename... Args>
  constexpr explicit TupleImpl(Args&&... args)
      : TupleLeaf<Idx, Ts>(utils::forward<Args>(args))... {}
};

template <typename... Ts>
class Tuple {
 public:
  using TL = transform<TypeList<Ts...>, utils::remove_reference_t>;
  using Idx = Indizes<Ts...>;

  template <typename... Args>
  constexpr explicit Tuple(Args&&... args)
      : impl_(make_integer_range<sizeof...(Args)>{},
              transform<TypeList<Args...>, utils::remove_reference_t>{},
              make_integer_range_<sizeof...(Args), sizeof...(Ts)>{},
              subrange<TL, sizeof...(Args), sizeof...(Ts)>{},
              utils::forward<Args>(args)...) {}

  template <auto N>
  constexpr const auto& get() const {
    using T = type_at<TL, N>;
    return static_cast<const TupleLeaf<N, T>&>(impl_).get();
  }

  template <auto N>
  constexpr auto& get() {
    using T = type_at<TL, N>;
    return static_cast<TupleLeaf<N, T>&>(impl_).get();
  }

  static constexpr auto size = sizeof...(Ts);

 private:
  using Impl = TupleImpl<Idx, Ts...>;
  Impl impl_;
};

template <auto N, typename... Ts>
constexpr auto get(const Tuple<Ts...>& t) {
  return t.template get<N>();
}

template <auto N, typename... Ts>
constexpr auto& get(Tuple<Ts...>& t) {
  return t.template get<N>();
}

template <typename T, typename... Ts, auto... Is>
constexpr auto push_front_impl(const Tuple<Ts...>& tuple, T v,
                               IntegerSequence<Is...>) {
  return Tuple<T, Ts...>{v, get<Is>(tuple)...};
}

template <typename T, typename... Ts>
constexpr auto push_front(const Tuple<Ts...>& tuple, T v) {
  return push_front_impl(tuple, v, typename Tuple<Ts...>::Idx{});
}

template <typename T, typename... Ts, auto... Is>
constexpr auto push_back_impl(const Tuple<Ts...>& tuple, T v,
                              IntegerSequence<Is...>) {
  return Tuple<Ts..., T>{get<Is>(tuple)..., v};
}

template <typename T, typename... Ts>
constexpr auto push_back_tuple(const Tuple<Ts...>& tuple, T v) {
  return push_back_impl(tuple, v, typename Tuple<Ts...>::Idx{});
}

template <typename... Args>
constexpr auto make_tuple(Args&&... args) {
  return Tuple<Args...>{utils::forward<Args>(args)...};
}

template <typename F, typename... Ts, auto... Is>
constexpr auto transform_tuple_impl(const Tuple<Ts...>& tuple, F&& f,
                                    IntegerSequence<Is...>) {
  return fsm::detail::make_tuple(f(get<Is>(tuple))...);
}

template <typename F, typename... Ts>
constexpr auto transform_tuple(const Tuple<Ts...>& tuple, F&& f) {
  using Idx = make_integer_range<sizeof...(Ts)>;
  return transform_tuple_impl(tuple, f, Idx{});
}

template <auto... Is, typename... Ts>
constexpr auto select_by_index(const Tuple<Ts...>& tuple,
                               IntegerSequence<Is...>) {
  return fsm::detail::make_tuple(get<Is>(tuple)...);
}

}  // namespace detail
}  // namespace fsm