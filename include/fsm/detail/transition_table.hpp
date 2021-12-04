#pragma once
#include <fsm/detail/meta.hpp>
#include <fsm/detail/transitions.hpp>
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

template <typename Transitions>
struct extractStatesImpl {};

template <typename Transition, typename... Transitions>
struct extractStatesImpl<TypeList<Transition, Transitions...>> {
  using S = typename extractStatesImpl<TypeList<Transitions...>>::type;
  using TSource = typename Transition::Source;
  using TTarget = typename Transition::Target;

  using S1 = utils::conditional_t<index_of<S, TSource> == -1,
                                  fsm::push_back<S, TSource>, S>;
  using type = utils::conditional_t<index_of<S, TTarget> == -1,
                                    fsm::push_back<S1, TTarget>, S>;
};

template <typename Transition>
struct extractStatesImpl<TypeList<Transition>> {
  using S = TypeList<typename Transition::Source>;
  using type =
      utils::conditional_t<index_of<S, typename Transition::Target> == -1,
                           fsm::push_back<S, typename Transition::Target>, S>;
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
  using type = utils::conditional_t<
      Head::template match<Source, Event>(), Head,
      typename matchTransition<TypeList<Tail...>, Source, Event>::type>;
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

template <typename T>
struct get_context_from_transition_impl {
  using type = typename T::Context;
};

template <typename T>
using get_context_from_transition = typename T::Context;

template <typename TT>
struct extract_context_impl {
 private:
  template <typename C>
  struct is_nocontext : utils::is_same<NoContext, C> {};

  using _Contexts =
      transform<typename TT::Transitions, get_context_from_transition>;

 public:
  using type = no_duplicates<remove_if<is_nocontext, _Contexts>>;
};

template <typename TT>
using extract_contexts = typename extract_context_impl<TT>::type;
}  // namespace detail
}  // namespace fsm
