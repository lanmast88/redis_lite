#pragma once
#include <string>
#include <vector>

struct Command {
    std::string name;
    std::vector<std::string> args;
    bool valid = false;
};

class CommandParser {
public:
    static Command parse(const std::string& line);
};
