#pragma once

namespace fsm {
namespace utils {
template <typename T>
struct remove_reference {
  using type = T;
};

template <typename T>
struct remove_reference<T &> {
  using type = T;
};

template <typename T>
struct remove_reference<T &&> {
  using type = T;
};

template <typename T>
using remove_reference_t = typename remove_reference<T>::type;

template <typename T>
remove_reference_t<T> &&move(T &&arg) {
  return static_cast<remove_reference_t<T> &&>(arg);
}

template <auto v>
struct integral_constant {
  using type = decltype(v);
  static constexpr auto value = v;
};

struct true_type : integral_constant<true> {};
struct false_type : integral_constant<false> {};

template <typename T>
constexpr T &&forward(remove_reference_t<T> &&param) noexcept {
  return static_cast<T &&>(param);
}
template <typename T>
constexpr T &&forward(remove_reference_t<T> &param) noexcept {
  return static_cast<T &&>(param);
}

template <typename T>
T &&declval();

template <bool B, class T = void>
struct enable_if {};

template <class T>
struct enable_if<true, T> {
  typedef T type;
};

template <typename T, typename U>
struct is_same : public false_type {};

template <typename T>
struct is_same<T, T> : public true_type {};

}  // namespace utils
}  // namespace fsm