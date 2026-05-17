#include "server.h"
#include "client_handler.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <stdexcept>
#include <cstring>

Server::Server(int port) : port_(port) {}

Server::~Server() {
    if (server_fd_ >= 0) close(server_fd_);
}

void Server::start() {
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ < 0) throw std::runtime_error("Failed to create socket");

    int opt = 1;
    setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port_);

    if (bind(server_fd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
        throw std::runtime_error("Failed to bind to port " + std::to_string(port_));

    if (listen(server_fd_, 128) < 0)
        throw std::runtime_error("Failed to listen");

    std::cout << "Redis Lite server started on port " << port_ << std::endl;

    while (true) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd_, reinterpret_cast<sockaddr*>(&client_addr), &client_len);
        if (client_fd < 0) {
            std::cerr << "accept() failed: " << strerror(errno) << std::endl;
            continue;
        }

        std::thread([client_fd, this]() {
            ClientHandler handler(client_fd, storage_);
            handler.handle();
        }).detach();
    }
}
