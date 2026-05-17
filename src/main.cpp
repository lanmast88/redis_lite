#include "server.h"
#include <iostream>
#include <cstdlib>

int main(int argc, char* argv[]) {
    int port = 6379;
    if (argc > 1) {
        port = std::atoi(argv[1]);
        if (port <= 0 || port > 65535) {
            std::cerr << "Invalid port: " << argv[1] << std::endl;
            return 1;
        }
    }

    try {
        Server server(port);
        server.start();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
