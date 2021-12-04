#pragma once

namespace fsm {
namespace utils {
template <typename T>
struct add_pointer {
  using type = T *;
};

template <typename T>
struct add_pointer<T *> {
  using type = T *;
};

template <typename T>
using add_pointer_t = typename add_pointer<T>::type;

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
struct remove_cv {
  typedef T type;
};
template <typename T>
struct remove_cv<const T> {
  typedef T type;
};
template <typename T>
struct remove_cv<volatile T> {
  typedef T type;
};
template <typename T>
struct remove_cv<const volatile T> {
  typedef T type;
};

template <typename T>
using remove_cv_t = typename remove_cv<T>::type;

template <typename T>
remove_reference_t<T> &&move(T &&arg) {
  return static_cast<remove_reference_t<T> &&>(arg);
}
template <class T>
struct remove_cvref {
  using type = remove_cv_t<remove_reference_t<T>>;
};

template <class T>
using remove_cvref_t = typename remove_cvref<T>::type;

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

template <bool B, class T>
using enable_if_t = typename enable_if<B, T>::type;

template <typename T, typename U>
struct is_same : public false_type {};

template <typename T>
struct is_same<T, T> : public true_type {};

template <typename T, typename U>
constexpr auto is_same_v = is_same<T, U>::value;

template <bool, typename T, typename U>
struct conditional {
  using type = U;
};

template <typename T, typename U>
struct conditional<true, T, U> {
  using type = T;
};

template <bool B, typename T, typename U>
using conditional_t = typename conditional<B, T, U>::type;

}  // namespace utils
}  // namespace fsm