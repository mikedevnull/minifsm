#pragma once

namespace fsm {

template <typename...>
struct TypeList;

template <typename Head, typename... Tail>
struct TypeList<Head, Tail...> {
  using head = Head;
  using tail = TypeList<Tail...>;
};

template <>
struct TypeList<> {};

template <typename TypeList, unsigned int N>
struct TypeAt;

template <typename Head, typename... Tail>
struct TypeAt<TypeList<Head, Tail...>, 0> {
  using type = Head;
};

template <typename Head, typename... Tail, unsigned int N>
struct TypeAt<TypeList<Head, Tail...>, N> {
  static_assert(N < sizeof...(Tail) + 1, "N out of range");
  using type = typename TypeAt<TypeList<Tail...>, N - 1>::type;
};

template <typename TypeList, typename TargetState>
struct IndexOf;

template <typename Head, typename... Tail>
struct IndexOf<TypeList<Head, Tail...>, Head> {
  static constexpr int value = 0;
};

template <typename Head, typename... Tail, typename T>
struct IndexOf<TypeList<Head, Tail...>, T> {
  static constexpr int value = IndexOf<TypeList<Tail...>, T>::value == -1
                                   ? -1
                                   : IndexOf<TypeList<Tail...>, T>::value + 1;
};

template <typename T>
struct IndexOf<TypeList<>, T> {
  static constexpr int value = -1;
};

}  // namespace fsm