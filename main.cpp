
#include <iostream>
#include <string>
#include <asio.hpp>

#include "server.hpp"

int main(int argc, char* argv[]) {
    try {
        server::server s("0.0.0.0", "1025", "./");
        s.run();
    } catch (std::exception& e) {
        std::cerr << "exception: " << e.what() << "\n";
    }

    return 0;
}
