#include <fsm/detail/typelist.hpp>

#include "catch2/catch.hpp"

#include <type_traits>

class Foo {};

TEST_CASE("calculate length of typelist", "[meta]") {
  using TL1 = fsm::detail::TypeList<int, float, Foo>;
  using TL2 = fsm::detail::TypeList<int, int>;
  using TL3 = fsm::detail::TypeList<>;
  STATIC_REQUIRE(TL1::size == 3);
  STATIC_REQUIRE(TL2::size == 2);
  STATIC_REQUIRE(TL3::size == 0);
}

TEST_CASE("get type by index from typelist", "[meta]") {
  using TL = fsm::detail::TypeList<int, float, Foo>;
  STATIC_REQUIRE(std::is_same<fsm::detail::type_at<TL, 0>, int>::value);
  STATIC_REQUIRE(std::is_same<fsm::detail::type_at<TL, 1>, float>::value);
  STATIC_REQUIRE(std::is_same<fsm::detail::type_at<TL, 2>, Foo>::value);
}

TEST_CASE("get index of type from typelist", "[meta]") {
  using TL = fsm::detail::TypeList<int, float, Foo, int>;
  STATIC_REQUIRE(fsm::detail::index_of<TL, int> == 0);
  STATIC_REQUIRE(fsm::detail::index_of<TL, float> == 1);
  STATIC_REQUIRE(fsm::detail::index_of<TL, Foo> == 2);
  STATIC_REQUIRE(fsm::detail::index_of<TL, double> == -1);
}

TEST_CASE("check if type exists in typelist", "[meta]") {
  using TL = fsm::detail::TypeList<int, float, Foo, int>;
  STATIC_REQUIRE(fsm::detail::has_type<TL, int>);
  STATIC_REQUIRE(fsm::detail::has_type<TL, float>);
  STATIC_REQUIRE(fsm::detail::has_type<TL, Foo>);
  STATIC_REQUIRE_FALSE(fsm::detail::has_type<TL, double>);
}

TEST_CASE("append types to typelist", "[meta]") {
  using TL0 = fsm::detail::TypeList<>;
  using TL1 = fsm::detail::push_back<TL0, int>;

  STATIC_REQUIRE(fsm::detail::index_of<TL1, int> == 0);
  STATIC_REQUIRE(TL1::size == 1);

  using TL2 = fsm::detail::push_back<TL1, float>;
  STATIC_REQUIRE(fsm::detail::index_of<TL2, int> == 0);
  STATIC_REQUIRE(fsm::detail::index_of<TL2, float> == 1);
  STATIC_REQUIRE(TL2::size == 2);
}

TEST_CASE("transform types in typelist", "[meta]") {
  using TL = fsm::detail::TypeList<int, float, Foo>;
  using R = typename fsm::detail::transform<TL, std::add_const_t>;
  STATIC_REQUIRE(R::size == 3);
  STATIC_REQUIRE(fsm::detail::index_of<R, const int> == 0);
  STATIC_REQUIRE(fsm::detail::index_of<R, const float> == 1);
  STATIC_REQUIRE(fsm::detail::index_of<R, const Foo> == 2);
}

TEST_CASE("extract sublist of typelist", "[meta]") {
  using TL = fsm::detail::TypeList<int, float, Foo, std::string, double, char>;
  using TSub = fsm::detail::subrange<TL, 1, 4>;
  STATIC_REQUIRE(TSub::size == 3);
  STATIC_REQUIRE(fsm::detail::index_of<TSub, float> == 0);
  STATIC_REQUIRE(fsm::detail::index_of<TSub, Foo> == 1);
  STATIC_REQUIRE(fsm::detail::index_of<TSub, std::string> == 2);

  using TSubZero = fsm::detail::subrange<TL, 4, 4>;
  STATIC_REQUIRE(TSubZero::size == 0);
}

TEST_CASE("remove first occurence of type from typelist", "[meta]") {
  using TL = fsm::detail::TypeList<int, float, std::string, float, double>;
  using R = fsm::detail::no_duplicates<TL>;

  STATIC_REQUIRE(R::size == 4);
  STATIC_REQUIRE(fsm::detail::index_of<R, int> == 0);
  STATIC_REQUIRE(fsm::detail::index_of<R, float> == 1);
  STATIC_REQUIRE(fsm::detail::index_of<R, std::string> == 2);
  STATIC_REQUIRE(fsm::detail::index_of<R, double> == 3);
}

TEST_CASE("remove from typelist with predicate", "[meta]") {
  using TL = fsm::detail::TypeList<int, float, Foo, std::string, double, char>;
  using R = fsm::detail::remove_if<std::is_integral, TL>;

  STATIC_REQUIRE(R::size == 4);
  STATIC_REQUIRE(fsm::detail::index_of<R, float> == 0);
  STATIC_REQUIRE(fsm::detail::index_of<R, Foo> == 1);
  STATIC_REQUIRE(fsm::detail::index_of<R, std::string> == 2);
  STATIC_REQUIRE(fsm::detail::index_of<R, double> == 3);
}

TEST_CASE("filter typelist with predicate", "[meta]") {
  using TL = fsm::detail::TypeList<int, float, Foo, std::string, double, char>;
  using R = fsm::detail::filter_if<std::is_integral, TL>;

  STATIC_REQUIRE(R::size == 2);
  STATIC_REQUIRE(fsm::detail::index_of<R, int> == 0);
  STATIC_REQUIRE(fsm::detail::index_of<R, char> == 1);
}

TEST_CASE("typelist find indizes by template predicate", "[meta]") {
  using TL = fsm::detail::TypeList<int, float, Foo, std::string, double, char>;

  using Idx0 = fsm::detail::find_all_if<std::is_integral, TL>;
  STATIC_REQUIRE(Idx0::size == 2);
  STATIC_REQUIRE(std::is_same_v<Idx0, fsm::detail::IntegerSequence<0, 5>>);

  using Idx1 = fsm::detail::find_all_if<std::is_fundamental, TL>;
  STATIC_REQUIRE(Idx1::size == 4);
  STATIC_REQUIRE(
      std::is_same_v<Idx1, fsm::detail::IntegerSequence<0, 1, 4, 5>>);
}

TEST_CASE("typelist find indizes by constexpr predicate callable", "[meta]") {
  using TL = fsm::detail::TypeList<int, float, Foo, double, char>;

  constexpr auto pred0 = [](auto v) constexpr {
    return std::is_integral_v<decltype(v)>;
  };
  using Idx0 = decltype(fsm::detail::find_all_if_callable(TL{}, pred0));
  STATIC_REQUIRE(Idx0::size == 2);
  STATIC_REQUIRE(std::is_same_v<Idx0, fsm::detail::IntegerSequence<0, 4>>);

  constexpr auto pred1 = [](auto v) constexpr {
    return std::is_fundamental_v<decltype(v)>;
  };
  using Idx1 = decltype(fsm::detail::find_all_if_callable(TL{}, pred1));
  STATIC_REQUIRE(Idx1::size == 4);
  STATIC_REQUIRE(
      std::is_same_v<Idx1, fsm::detail::IntegerSequence<0, 1, 3, 4>>);
}

template <typename... Ts>
struct VariadicFooList {};

TEST_CASE("can rebind types to other variadic template", "[meta]") {
  using TL = fsm::detail::TypeList<int, float, Foo, double, int, char>;
  using R = fsm::detail::rebind<TL, VariadicFooList>;
  STATIC_REQUIRE(
      std::is_same_v<R, VariadicFooList<int, float, Foo, double, int, char>>);
}