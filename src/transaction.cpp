#include "transaction.h"

void Transaction::begin() {
    active_ = true;
    commands_.clear();
}

void Transaction::addCommand(const Command& cmd) {
    commands_.push_back(cmd);
}

std::vector<Command> Transaction::commit() {
    active_ = false;
    return std::move(commands_);
}

void Transaction::rollback() {
    active_ = false;
    commands_.clear();
}
