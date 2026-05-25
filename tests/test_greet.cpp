#include <catch2/catch_amalgamated.hpp>

#include "greet.h"

TEST_CASE("Greet returns a hello string", "[greet]") {
  REQUIRE(myapp::Greet("world") == "Hello, world!");
  REQUIRE(myapp::Greet("Claude") == "Hello, Claude!");
}
