#include <catch2/catch_amalgamated.hpp>

#include "greet.h"

TEST_CASE("greet returns a hello string", "[greet]") {
    REQUIRE(myapp::greet("world") == "Hello, world!");
    REQUIRE(myapp::greet("Claude") == "Hello, Claude!");
}
