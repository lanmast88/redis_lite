#pragma once
#include "storage.h"

class Server {
public:
    explicit Server(int port);
    ~Server();
    void start();

private:
    int port_;
    int server_fd_ = -1;
    Storage storage_;
};
