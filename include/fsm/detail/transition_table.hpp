#pragma once
#include <fsm/detail/meta.hpp>
#include <fsm/detail/tuple.hpp>
#include <fsm/typelist.hpp>

namespace fsm {
namespace detail {

template <typename Config>
struct transition_table_from_config_impl {
  using type = decltype(Config::transition_table());
};

template <typename Config>
using transition_table_from_config =
    typename transition_table_from_config_impl<Config>::type;

template <typename TT, typename State>
constexpr auto filter_by_state_impl(TT t, State) {
  using Transitions = typename TT::Transitions;
  auto idx = find_all_if_callable(Transitions{}, [](auto transition) {
    using TState = decltype(transition.source());
    return utils::is_same<TState, State>::value;
  });
  return idx;
}

template <typename TT, typename State>
using filter_by_state = decltype(filter_by_state_impl(utils::declval<TT>(),
                                                      utils::declval<State>()));

template <typename State, typename... Ts>
constexpr auto filter_transitions_by_state(detail::Tuple<Ts...> transitions) {
  using TL = typename detail::Tuple<Ts...>::TL;
  auto idx = find_all_if_callable(TL{}, [](auto transition) {
    using TState = decltype(transition.source());
    return utils::is_same<TState, State>::value;
  });
  return select_by_index(transitions, idx);
}

template <typename Transitions>
struct extractStatesImpl {};

template <typename Transition, typename... Transitions>
struct extractStatesImpl<TypeList<Transition, Transitions...>> {
  using S = typename extractStatesImpl<TypeList<Transitions...>>::type;
  using TSource = typename Transition::Source;
  using TTarget = typename Transition::Target;

  using S1 = _if_t<fsm::push_back<S, TSource>, S, index_of<S, TSource> == -1>;
  using type =
      _if_t<fsm::push_back<S1, TTarget>, S, index_of<S1, TTarget> == -1>;
};

template <typename Transition>
struct extractStatesImpl<TypeList<Transition>> {
  using S = TypeList<typename Transition::Source>;
  using type = _if_t<fsm::push_back<S, typename Transition::Target>, S,
                     index_of<S, typename Transition::Target> == -1>;
};

template <>
struct extractStatesImpl<TypeList<>> {
  using type = TypeList<>;
};

template <typename T>
struct extractStates {
  using type = typename extractStatesImpl<typename T::Transitions>::type;
};

template <typename T>
using extractStates_t = typename extractStates<T>::type;

struct NoMatch {};

template <typename Typelist, typename Source, typename Event>
struct matchTransition {
  using type = NoMatch;
};

template <typename Source, typename Event, typename Head, typename... Tail>
struct matchTransition<TypeList<Head, Tail...>, Source, Event> {
  using type =
      _if_t<Head,
            typename matchTransition<TypeList<Tail...>, Source, Event>::type,
            Head::template match<Source, Event>()>;
};

template <typename TL, typename Source, typename Event>
using matchTransition_t = typename matchTransition<TL, Source, Event>::type;

template <typename TT>
struct extract_initial_state_impl {
 private:
  using Transtitions = typename TT::Transitions;
  static_assert(Transtitions::size > 0);
  using FirstTransition = type_at<Transtitions, 0>;

 public:
  using type = typename FirstTransition::Source;
};

template <typename TL>
using extract_initial_state = typename extract_initial_state_impl<TL>::type;

}  // namespace detail
}  // namespace fsm
