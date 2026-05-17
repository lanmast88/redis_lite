#pragma once
#include "command_parser.h"
#include <vector>

class Transaction {
public:
    bool isActive() const { return active_; }
    void begin();
    void addCommand(const Command& cmd);
    std::vector<Command> commit();
    void rollback();

private:
    bool active_ = false;
    std::vector<Command> commands_;
};
