#pragma once

namespace fsm {
namespace detail {

template <typename T>
struct DependencyHolder {
  DependencyHolder(T v) : value(v) {}
  using type = T;
  T value;
};

struct NoDependency;
template <>
struct DependencyHolder<NoDependency> {
  using type = NoDependency;
};

}  // namespace detail
}  // namespace fsm