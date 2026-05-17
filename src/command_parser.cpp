#include "command_parser.h"
#include <sstream>
#include <algorithm>
#include <cctype>

Command CommandParser::parse(const std::string& line) {
    Command cmd;
    std::istringstream ss(line);
    std::string token;

    if (!(ss >> token)) return cmd;

    std::transform(token.begin(), token.end(), token.begin(), ::toupper);
    cmd.name = token;

    while (ss >> token) {
        cmd.args.push_back(token);
    }

    cmd.valid = true;
    return cmd;
}
