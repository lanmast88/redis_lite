#pragma once
#include "storage.h"
#include "transaction.h"
#include "command_parser.h"
#include <string>

class ClientHandler {
public:
    ClientHandler(int fd, Storage& storage);
    void handle();

private:
    std::string processCommand(const Command& cmd);
    std::string executeCommand(const Command& cmd);
    void sendResponse(const std::string& msg);

    int fd_;
    Storage& storage_;
    Transaction transaction_;
    bool quit_ = false;
};
