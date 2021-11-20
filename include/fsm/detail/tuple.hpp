#pragma once
#include "fsm/detail/integer_sequence.hpp"
#include "fsm/detail/meta.hpp"
#include "fsm/substitutes.hpp"
#include "fsm/typelist.hpp"

namespace fsm {
namespace detail {

template <unsigned int N, typename T>
class TupleLeaf {
 public:
  TupleLeaf() = default;
  explicit TupleLeaf(T&& v) : value_(utils::forward<T>(v)) {}

  const T& get() const { return value_; }
  T& get() { return value_; }

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
  template <auto N = sizeof...(Ts)>
  constexpr TupleImpl(typename utils::enable_if<(N > 0)>::type* = 0)
      : TupleLeaf<Idx, Ts>()... {}

  constexpr explicit TupleImpl(Ts&&... args)
      : TupleLeaf<Idx, Ts>(utils::forward<Ts>(args))... {}
};

template <typename... Ts>
class Tuple {
 public:
  using TL = TypeList<Ts...>;
  using Idx = Indizes<Ts...>;
  template <auto N = sizeof...(Ts)>
  constexpr Tuple(typename utils::enable_if<(N > 0)>::type* = 0) : impl_() {}

  constexpr explicit Tuple(Ts&&... args) : impl_(utils::forward<Ts>(args)...) {}

  template <unsigned N>
  const auto& get() const {
    using T = type_at<TL, N>;
    return static_cast<const TupleLeaf<N, T>&>(impl_).get();
  }

  template <unsigned N>
  auto& get() {
    using T = type_at<TL, N>;
    return static_cast<TupleLeaf<N, T>&>(impl_).get();
  }

  static constexpr auto size = sizeof...(Ts);

 private:
  using Impl = TupleImpl<Idx, Ts...>;
  Impl impl_;
};

template <auto N, typename... Ts>
auto get(const Tuple<Ts...>& t) {
  return t.template get<N>();
}

template <auto N, typename... Ts>
auto& get(Tuple<Ts...>& t) {
  return t.template get<N>();
}

template <typename T, typename... Ts, auto... Is>
auto push_front_impl(const Tuple<Ts...>& tuple, T&& v, IntegerSequence<Is...>) {
  return Tuple<T, Ts...>{v, get<Is>(tuple)...};
}

template <typename T, typename... Ts>
auto push_front(const Tuple<Ts...>& tuple, T&& v) {
  return push_front_impl(tuple, v, typename Tuple<Ts...>::Idx{});
}

}  // namespace detail
}  // namespace fsm