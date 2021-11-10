#include <fsm/typelist.hpp>

#include "catch2/catch.hpp"

#include <type_traits>

class Foo;

TEST_CASE("calculate length of typelist", "[meta]") {
  using TL1 = fsm::TypeList<int, float, Foo>;
  using TL2 = fsm::TypeList<int, int>;
  using TL3 = fsm::TypeList<>;
  STATIC_REQUIRE(fsm::Length<TL1>::value == 3);
  STATIC_REQUIRE(fsm::Length<TL2>::value == 2);
  STATIC_REQUIRE(fsm::Length<TL3>::value == 0);
}

TEST_CASE("get type by index from typelist", "[meta]") {
  using TL = fsm::TypeList<int, float, Foo>;
  STATIC_REQUIRE(std::is_same<fsm::TypeAt<TL, 0>::type, int>::value);
  STATIC_REQUIRE(std::is_same<fsm::TypeAt<TL, 1>::type, float>::value);
  STATIC_REQUIRE(std::is_same<fsm::TypeAt<TL, 2>::type, Foo>::value);
}

TEST_CASE("get index of type from typelist", "[meta]") {
  using TL = fsm::TypeList<int, float, Foo, int>;
  STATIC_REQUIRE(fsm::IndexOf<TL, int>::value == 0);
  STATIC_REQUIRE(fsm::IndexOf<TL, float>::value == 1);
  STATIC_REQUIRE(fsm::IndexOf<TL, Foo>::value == 2);
  STATIC_REQUIRE(fsm::IndexOf<TL, double>::value == -1);
}