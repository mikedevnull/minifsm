#include <fsm/typelist.hpp>

#include "catch2/catch.hpp"

#include <type_traits>

class Foo;

TEST_CASE("calculate length of typelist", "[meta]") {
  using TL1 = fsm::TypeList<int, float, Foo>;
  using TL2 = fsm::TypeList<int, int>;
  using TL3 = fsm::TypeList<>;
  STATIC_REQUIRE(TL1::size == 3);
  STATIC_REQUIRE(TL2::size == 2);
  STATIC_REQUIRE(TL3::size == 0);
}

TEST_CASE("get type by index from typelist", "[meta]") {
  using TL = fsm::TypeList<int, float, Foo>;
  STATIC_REQUIRE(std::is_same<fsm::type_at<TL, 0>, int>::value);
  STATIC_REQUIRE(std::is_same<fsm::type_at<TL, 1>, float>::value);
  STATIC_REQUIRE(std::is_same<fsm::type_at<TL, 2>, Foo>::value);
}

TEST_CASE("get index of type from typelist", "[meta]") {
  using TL = fsm::TypeList<int, float, Foo, int>;
  STATIC_REQUIRE(fsm::index_of<TL, int> == 0);
  STATIC_REQUIRE(fsm::index_of<TL, float> == 1);
  STATIC_REQUIRE(fsm::index_of<TL, Foo> == 2);
  STATIC_REQUIRE(fsm::index_of<TL, double> == -1);
}

TEST_CASE("check if type exists in typelist", "[meta]") {
  using TL = fsm::TypeList<int, float, Foo, int>;
  STATIC_REQUIRE(fsm::has_type<TL, int>);
  STATIC_REQUIRE(fsm::has_type<TL, float>);
  STATIC_REQUIRE(fsm::has_type<TL, Foo>);
  STATIC_REQUIRE_FALSE(fsm::has_type<TL, double>);
}

TEST_CASE("append types to typelist", "[meta]") {
  using TL0 = fsm::TypeList<>;
  using TL1 = fsm::push_back<TL0, int>;

  STATIC_REQUIRE(fsm::index_of<TL1, int> == 0);
  STATIC_REQUIRE(TL1::size == 1);

  using TL2 = fsm::push_back<TL1, float>;
  STATIC_REQUIRE(fsm::index_of<TL2, int> == 0);
  STATIC_REQUIRE(fsm::index_of<TL2, float> == 1);
  STATIC_REQUIRE(TL2::size == 2);
}

TEST_CASE("transform types in typelist", "[meta]") {
  using TL = fsm::TypeList<int, float, Foo>;
  using R = typename fsm::transform<TL, std::add_const_t>;
  STATIC_REQUIRE(R::size == 3);
  STATIC_REQUIRE(fsm::index_of<R, const int> == 0);
  STATIC_REQUIRE(fsm::index_of<R, const float> == 1);
  STATIC_REQUIRE(fsm::index_of<R, const Foo> == 2);
}

TEST_CASE("extract sublist of typelist", "[meta]") {
  using TL = fsm::TypeList<int, float, Foo, std::string, double, char>;
  using TSub = fsm::subrange<TL, 1, 4>;
  STATIC_REQUIRE(TSub::size == 3);
  STATIC_REQUIRE(fsm::index_of<TSub, float> == 0);
  STATIC_REQUIRE(fsm::index_of<TSub, Foo> == 1);
  STATIC_REQUIRE(fsm::index_of<TSub, std::string> == 2);

  using TSubZero = fsm::subrange<TL, 4, 4>;
  STATIC_REQUIRE(TSubZero::size == 0);
}