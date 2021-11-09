#pragma once
#include "fsm/detail/binder.hpp"
#include "fsm/detail/mem_fun_invoker.hpp"
#include "fsm/detail/type_utils.hpp"

namespace fsm {
namespace detail {

struct NoopVisitor {
  template <typename... Args>
  void operator()(Args&&...) const {}
};

template <typename T, typename... Args>
class has_on_with_signature {
  template <typename C, typename = decltype(fsm::utils::declval<C>().on(
                            fsm::utils::declval<Args>()...))>
  static fsm::utils::true_type test(int);
  template <typename C>
  static fsm::utils::false_type test(...);

 public:
  static constexpr bool value = decltype(test<T>(0))::value;
};

template <typename C, typename... Args>
auto wrap_and_bind_on(C& o, Args&&... args) {
  auto invoker = createInvoker<void, C, Args...>(o, &C::on);

  return fsm::detail::bind(invoker, fsm::utils::forward<Args>(args)...);
}

template <typename C, typename Event, typename SM, typename Dependency>
auto create_matching_on_visitor(C& o, const Event& event, SM& sm,
                                Dependency&& dep) {
  if constexpr (has_on_with_signature<C, const Event&, SM&,
                                      Dependency>::value) {
    return wrap_and_bind_on(o, event, sm, fsm::utils::forward<Dependency>(dep));
  } else if constexpr (has_on_with_signature<C, const Event&,
                                             Dependency>::value) {
    return wrap_and_bind_on(o, event, fsm::utils::forward<Dependency>(dep));
  } else if constexpr (has_on_with_signature<C, const Event&, SM&>::value) {
    return wrap_and_bind_on(o, event, sm);
  } else if constexpr (has_on_with_signature<C, const Event&>::value) {
    return wrap_and_bind_on(o, event);
  } else {
    return NoopVisitor{};
  }
}

template <typename Event, typename SM, typename Dependency>
struct OnEventVisitor {
  OnEventVisitor(const Event& event, SM& sm, Dependency dependency)
      : event(event), sm(sm), dependency(dependency) {}
  template <typename C>
  void operator()(C& o) {
    auto invoker = create_matching_on_visitor(
        o, event, sm, fsm::utils::forward<Dependency>(dependency));
    return invoker();
  }
  const Event& event;
  SM& sm;
  Dependency dependency;
};

template <typename Event, typename SM, typename Dependency>
auto createOnEventVisitor(const Event& event, SM& sm, Dependency d) {
  return OnEventVisitor<Event, SM, Dependency>{event, sm, d};
}

template <typename T, typename... Args>
class has_enter_with_signature {
  template <typename C, typename = decltype(fsm::utils::declval<C>().enter(
                            fsm::utils::declval<Args>()...))>
  static fsm::utils::true_type test(int);
  template <typename C>
  static fsm::utils::false_type test(...);

 public:
  static constexpr bool value = decltype(test<T>(0))::value;
};

template <typename C, typename... Args>
auto wrap_and_bind_enter(C& o, Args&&... args) {
  auto invoker =
      fsm::detail::MemFunInvoker<fsm::detail::mem_fun_sig_t<void, C, Args...>,
                                 C>{o, &C::enter};
  return fsm::detail::bind(invoker, fsm::utils::forward<Args>(args)...);
}

template <typename C, typename Event, typename SM, typename Dependency>
auto create_matching_enter_visitor(C& o, const Event& event, SM& sm,
                                   Dependency&& dep) {
  if constexpr (has_enter_with_signature<C, const Event&, SM&,
                                         Dependency>::value) {
    return wrap_and_bind_enter(o, event, sm,
                               fsm::utils::forward<Dependency>(dep));
  } else if constexpr (has_enter_with_signature<C, const Event&,
                                                Dependency>::value) {
    return wrap_and_bind_enter(o, event, fsm::utils::forward<Dependency>(dep));
  } else if constexpr (has_enter_with_signature<C, const Event&, SM&>::value) {
    return wrap_and_bind_enter(o, event, sm);
  } else if constexpr (has_enter_with_signature<C, const Event&>::value) {
    return wrap_and_bind_enter(o, event);
  } else {
    return NoopVisitor{};
  }
}

template <typename T, typename... Args>
class has_exit_with_signature {
  template <typename C, typename = decltype(fsm::utils::declval<C>().exit(
                            fsm::utils::declval<Args>()...))>
  static fsm::utils::true_type test(int);
  template <typename C>
  static fsm::utils::false_type test(...);

 public:
  static constexpr bool value = decltype(test<T>(0))::value;
};

template <typename C, typename... Args>
auto wrap_and_bind_exit(C& o, Args&&... args) {
  auto invoker =
      fsm::detail::MemFunInvoker<fsm::detail::mem_fun_sig_t<void, C, Args...>,
                                 C>{o, &C::exit};
  return fsm::detail::bind(invoker, fsm::utils::forward<Args>(args)...);
}

template <typename C, typename Event, typename TargetState, typename Dependency>
auto create_matching_exit_visitor(C& o, const Event& event,
                                  const TargetState& target, Dependency&& dep) {
  if constexpr (has_exit_with_signature<C, const Event&, const TargetState&,
                                        Dependency>::value) {
    return wrap_and_bind_exit(o, event, target,
                              fsm::utils::forward<Dependency>(dep));
  } else if constexpr (has_exit_with_signature<C, const Event&,
                                               Dependency>::value) {
    return wrap_and_bind_exit(o, event, fsm::utils::forward<Dependency>(dep));
  } else if constexpr (has_exit_with_signature<C, const Event&,
                                               const TargetState&>::value) {
    return wrap_and_bind_exit(o, event, target);
  } else if constexpr (has_exit_with_signature<C, const Event&>::value) {
    return wrap_and_bind_exit(o, event);
  } else if constexpr (has_exit_with_signature<C, const TargetState&>::value) {
    return wrap_and_bind_exit(o, target);
  } else if constexpr (has_exit_with_signature<C, Dependency>::value) {
    return wrap_and_bind_exit(o, fsm::utils::forward<Dependency>(dep));
  } else if constexpr (has_exit_with_signature<C>::value) {
    return wrap_and_bind_exit(o);
  } else {
    return NoopVisitor{};
  }
}

}  // namespace detail

// namespace detail {
// template <typename SM, typename Event, typename DependencyHolder>
// struct DispatchEventVisitor {
//   explicit DispatchEventVisitor(const Event &event, SM &sm,
//                                 const DependencyHolder &dep)
//       : event_(event), sm_(sm), dep_(dep) {}

//   template <typename CurrentState>
//   void operator()(CurrentState &state) {
//     using DepT = typename DependencyHolder::type;
//     static_assert(has_on_signature<CurrentState, void, Event, SM &, DepT>()
//     ||
//                       has_on_signature<CurrentState, void, Event, DepT>() ||
//                       has_on_signature<CurrentState, void, Event, SM &>() ||
//                       has_on_signature<CurrentState, void, Event>(),
//                   "Does not have any 'on' method with a valid signature");
//     if constexpr (has_on_signature<CurrentState, void, Event, SM &, DepT>())
//     {
//       state.on(event_, sm_, dep_.value);
//     } else if constexpr (has_on_signature<CurrentState, void, Event, DepT>())
//     {
//       state.on(event_, dep_.value);
//     } else if constexpr (has_on_signature<CurrentState, void, Event, SM &>())
//     {
//       state.on(event_, sm_);
//     } else if constexpr (has_on_signature<CurrentState, void, Event>()) {
//       state.on(event_);
//     }
//   }

//   const Event &event_;
//   SM &sm_;
//   const DependencyHolder &dep_;
// };

// template <typename Event, typename TargetState, typename DependencyHolder>
// struct EnterStateVisitor {
//   explicit EnterStateVisitor(const Event &event, TargetState &ts,
//                              const DependencyHolder &dep)
//       : event_(event), targetState_(ts), dep_(dep) {}

//   template <typename SourceState>
//   void operator()(const SourceState &source) {
//     using DepT = typename DependencyHolder::type;
//     static_assert(
//         has_enter_signature<TargetState, void, Event, SourceState, DepT>() ||
//             has_enter_signature<TargetState, void, Event, DepT>() ||
//             has_enter_signature<TargetState, void, Event, SourceState>() ||
//             has_enter_signature<TargetState, void, Event>() ||
//             has_enter_signature<TargetState, void, DepT>(),
//         "Does not have any 'enter' method with a valid signature");
//     if constexpr (has_enter_signature<TargetState, void, Event, SourceState,
//                                       DepT>()) {
//       targetState_.enter(event_, source, dep_.value);
//     } else if constexpr (has_enter_signature<TargetState, void, Event,
//                                              DepT>()) {
//       targetState_.enter(event_, dep_.value);
//     } else if constexpr (has_enter_signature<TargetState, void, Event,
//                                              SourceState>()) {
//       targetState_.enter(event_, source);
//     } else if constexpr (has_enter_signature<TargetState, void, Event>()) {
//       targetState_.enter(event_);
//     } else if constexpr (has_enter_signature<TargetState, void, DepT>()) {
//       targetState_.enter(dep_.value);
//     }
//   }

//   const Event &event_;
//   TargetState &targetState_;
//   const DependencyHolder &dep_;
// };

// template <typename Event, typename TargetState>
// struct ExitStateVisitor {
//   explicit ExitStateVisitor(const Event &event, const TargetState &s)
//       : event_(event), targetState_(s) {}

//   template <typename SourceState>
//   void operator()(SourceState &source) {
//     source.exit(targetState_, event_);
//   }

//   const Event &event_;
//   const TargetState &targetState_;
// };

}  // namespace fsm