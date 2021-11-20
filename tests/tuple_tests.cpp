#include "fsm/detail/meta.hpp"
#include "fsm/detail/tuple.hpp"

#include <catch2/catch.hpp>
#include <type_traits>

TEST_CASE("tuple index type generation", "[meta]") {
  using Idx = fsm::detail::make_integer_range<3>;
  STATIC_REQUIRE(std::is_same_v<fsm::detail::IntegerSequence<0, 1, 2>, Idx>);

  using EmptyIdx = fsm::detail::make_integer_range<0>;
  STATIC_REQUIRE(std::is_same_v<fsm::detail::IntegerSequence<>, EmptyIdx>);

  using IndexFromArgs = fsm::detail::Indizes<int, float, double, char>;
  using expected = fsm::detail::IntegerSequence<std::size_t{0}, std::size_t{1},
                                                std::size_t{2}, std::size_t{3}>;
  //   fsm::detail::showTheType<expect8ed::value_type> x;
  STATIC_REQUIRE(std::is_same_v<expected, IndexFromArgs>);
}

TEST_CASE("tuple construction with values", "[meta]") {
  const fsm::detail::Tuple<int, bool, char> t{42, true, 'x'};
  REQUIRE(t.get<0>() == 42);
  REQUIRE(t.get<1>() == true);
  REQUIRE(t.get<2>() == 'x');
}

TEST_CASE("tuple element access by member get", "[meta]") {
  fsm::detail::Tuple<int, float> t;
  t.get<0>() = 2;
  REQUIRE(t.get<0>() == 2);
  t.get<1>() = 4.2;
  REQUIRE(t.get<1>() > 4.1);
  REQUIRE(t.get<1>() < 4.3);
  REQUIRE(t.get<0>() == 2);
}

TEST_CASE("tuple element access by free get", "[meta]") {
  fsm::detail::Tuple<int, float> t;
  fsm::detail::get<0>(t) = 2;
  REQUIRE(fsm::detail::get<0>(t) == 2);
  fsm::detail::get<1>(t) = 4.2;
  REQUIRE(fsm::detail::get<1>(t) > 4.1);
  REQUIRE(fsm::detail::get<1>(t) < 4.3);
  REQUIRE(fsm::detail::get<0>(t) == 2);
}

template <typename F, typename... Ts, auto... Is>
auto applyImpl(const fsm::detail::Tuple<Ts...>& t,
               fsm::detail::IntegerSequence<Is...>, F f) {
  return f(fsm::detail::get<Is>(t)...);
}

template <typename F, typename... Ts>
auto apply(const fsm::detail::Tuple<Ts...>& t, F f) {
  using Idx = typename fsm::detail::Tuple<Ts...>::Idx;
  return applyImpl(t, Idx{}, f);
}

TEST_CASE("unpack tuple elements with spread free get", "[meta]") {
  fsm::detail::Tuple<int, bool> t{42, true};

  auto f = [](int i, bool v) {
    REQUIRE(i == 42);
    REQUIRE(v == true);
  };
  using Idx = typename fsm::detail::Tuple<int, bool>::Idx;
  apply(t, f);
}

TEST_CASE("append values to front of tuple", "[meta]") {
  const fsm::detail::Tuple<int, bool> t{42, true};
  auto t2 = fsm::detail::push_front(t, 'c');

  STATIC_REQUIRE(t2.size == 3);
  REQUIRE(t2.get<0>() == 'c');
  REQUIRE(t2.get<1>() == 42);
  REQUIRE(t2.get<2>() == true);
}