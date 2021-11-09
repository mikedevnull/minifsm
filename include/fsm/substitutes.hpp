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

struct true_type {
  static constexpr bool value = true;
};
struct false_type {
  static constexpr bool value = false;
};

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

}  // namespace utils
}  // namespace fsm