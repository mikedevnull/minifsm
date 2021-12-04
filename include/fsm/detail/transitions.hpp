#pragma once
#include <fsm/types.hpp>
#include <fsm/utils/stl_substitutes.hpp>

namespace fsm {

namespace detail {

struct NoAction {
  template <typename S, typename E, typename T>
  void operator()(S &, const E &, T &) const {}
};

struct NoContext {};

template <typename C, typename F>
struct MemFunAction {
  constexpr MemFunAction(F f) : _fun(f) {}
  template <typename... Args>
  auto operator()(C &context, Args &&...args) const {
    return (context.*_fun)(utils::forward<Args>(args)...);
  }
  template <typename... Args>
  auto operator()(const C &context, Args &&...args) const {
    return (context.*_fun)(utils::forward<Args>(args)...);
  }

 private:
  F _fun;
};

template <typename S, typename E, typename T, typename C = NoContext,
          typename A = NoAction>
struct Transition {
  using Source = S;
  using Event = E;
  using Target = T;
  using Context = C;
  using Action = A;
  constexpr Transition(Action action) : action_(action) {}

  Transition() = default;

  constexpr Source source();
  constexpr Event event();
  constexpr Target target();

  template <typename Q = C>
  utils::enable_if_t<utils::is_same_v<Q, NoContext>, void> execute(
      Source &source, const Event &event, T &target) {
    action_(source, event, target);
  }

  template <typename Q = C>
  utils::enable_if_t<!utils::is_same_v<Q, NoContext>, void> execute(
      Context *context, Source &source, const Event &event, T &target) {
    action_(*context, source, event, target);
  }

  template <typename Q = C>
  utils::enable_if_t<!utils::is_same_v<Q, NoContext>, void> execute(
      const Context *context, Source &source, const Event &event, T &target) {
    action_(*context, source, event, target);
  }

  template <typename Q = C>
  utils::enable_if_t<!utils::is_same_v<Q, NoContext>, void> execute(
      Context &context, Source &source, const Event &event, T &target) {
    action_(context, source, event, target);
  }

  template <typename Q = C>
  utils::enable_if_t<!utils::is_same_v<Q, NoContext>, void> execute(
      const Context &context, Source &source, const Event &event, T &target) {
    action_(context, source, event, target);
  }

  template <typename CheckedState, typename CheckedEvent>
  static constexpr bool match() {
    return utils::is_same_v<CheckedState, Source> &&
           utils::is_same_v<CheckedEvent, Event>;
  }

 private:
  Action action_;
};

template <typename Source, typename E, typename Target>
constexpr auto make_transition(const fsm::State<Source>, const fsm::Event<E>,
                               const fsm::State<Target>) {
  return detail::Transition<Source, E, Target>{};
};

template <typename Source, typename E, typename Target, typename Action>
constexpr auto make_transition(const fsm::State<Source> &,
                               const fsm::Event<E> &,
                               const fsm::State<Target> &, Action action) {
  return detail::Transition<Source, E, Target, NoContext, Action>{action};
};

template <typename Source, typename E, typename Target, typename C,
          typename MemFun>
constexpr auto make_transition(const fsm::State<Source> &,
                               const fsm::Event<E> &,
                               const fsm::State<Target> &,
                               const fsm::Context<C> &, MemFun f) {
  return detail::Transition<Source, E, Target, C, MemFunAction<C, MemFun>>{
      MemFunAction<C, MemFun>{f}};
};

}  // namespace detail

}  // namespace fsm