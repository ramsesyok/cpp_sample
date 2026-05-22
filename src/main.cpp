#include <iostream>
#include <string>

#include "greet.h"

int main(int argc, char* argv[]) {
    const std::string name = (argc > 1) ? argv[1] : "world";
    std::cout << myapp::greet(name) << std::endl;
    return 0;
}
