#include "client_handler.h"
#include <algorithm>
#include <unistd.h>
#include <sys/socket.h>
#include <sstream>

ClientHandler::ClientHandler(int fd, Storage& storage)
    : fd_(fd), storage_(storage) {}

void ClientHandler::handle() {
    char buf[4096];
    std::string leftover;

    while (true) {
        ssize_t n = recv(fd_, buf, sizeof(buf) - 1, 0);
        if (n <= 0) break;

        buf[n] = '\0';
        leftover += buf;

        size_t pos;
        while ((pos = leftover.find('\n')) != std::string::npos) {
            std::string line = leftover.substr(0, pos);
            leftover.erase(0, pos + 1);

            // Strip \r if present
            if (!line.empty() && line.back() == '\r') line.pop_back();
            if (line.empty()) continue;

            Command cmd = CommandParser::parse(line);
            if (!cmd.valid) {
                sendResponse("-ERR empty command\r\n");
                continue;
            }

            std::string response = processCommand(cmd);
            sendResponse(response);
            if (quit_) break;
        }
    }
    close(fd_);
}

std::string ClientHandler::processCommand(const Command& cmd) {
    if (cmd.name == "BEGIN") {
        if (transaction_.isActive()) return "-ERR transaction already started\r\n";
        transaction_.begin();
        return "+OK\r\n";
    }
    if (cmd.name == "ROLLBACK") {
        if (!transaction_.isActive()) return "-ERR no active transaction\r\n";
        transaction_.rollback();
        return "+OK\r\n";
    }
    if (cmd.name == "COMMIT") {
        if (!transaction_.isActive()) return "-ERR no active transaction\r\n";
        auto cmds = transaction_.commit();
        std::ostringstream out;
        out << "*" << cmds.size() << "\r\n";
        for (const auto& c : cmds) {
            out << executeCommand(c);
        }
        return out.str();
    }

    if (transaction_.isActive()) {
        transaction_.addCommand(cmd);
        return "+QUEUED\r\n";
    }

    return executeCommand(cmd);
}

std::string ClientHandler::executeCommand(const Command& cmd) {
    if (cmd.name == "SET") {
        if (cmd.args.size() < 2) return "-ERR wrong number of arguments for SET\r\n";
        int ttl = -1;
        if (cmd.args.size() >= 4) {
            std::string flag = cmd.args[2];
            std::transform(flag.begin(), flag.end(), flag.begin(), ::toupper);
            if (flag == "EX") {
                try { ttl = std::stoi(cmd.args[3]); } catch (...) {
                    return "-ERR value is not an integer\r\n";
                }
            }
        }
        storage_.set(cmd.args[0], cmd.args[1], ttl);
        return "+OK\r\n";
    }

    if (cmd.name == "GET") {
        if (cmd.args.size() < 1) return "-ERR wrong number of arguments for GET\r\n";
        auto val = storage_.get(cmd.args[0]);
        if (!val) return "$-1\r\n";
        return "$" + std::to_string(val->size()) + "\r\n" + *val + "\r\n";
    }

    if (cmd.name == "DEL") {
        if (cmd.args.size() < 1) return "-ERR wrong number of arguments for DEL\r\n";
        int count = 0;
        for (const auto& key : cmd.args) {
            if (storage_.del(key)) ++count;
        }
        return ":" + std::to_string(count) + "\r\n";
    }

    if (cmd.name == "EXISTS") {
        if (cmd.args.size() < 1) return "-ERR wrong number of arguments for EXISTS\r\n";
        return storage_.exists(cmd.args[0]) ? ":1\r\n" : ":0\r\n";
    }

    if (cmd.name == "KEYS") {
        auto ks = storage_.keys();
        std::ostringstream out;
        out << "*" << ks.size() << "\r\n";
        for (const auto& k : ks) {
            out << "$" << k.size() << "\r\n" << k << "\r\n";
        }
        return out.str();
    }

    if (cmd.name == "EXPIRE") {
        if (cmd.args.size() < 2) return "-ERR wrong number of arguments for EXPIRE\r\n";
        int seconds = 0;
        try { seconds = std::stoi(cmd.args[1]); } catch (...) {
            return "-ERR value is not an integer\r\n";
        }
        return storage_.expire(cmd.args[0], seconds) ? ":1\r\n" : ":0\r\n";
    }

    if (cmd.name == "PING") {
        return "+PONG\r\n";
    }

    if (cmd.name == "QUIT") {
        quit_ = true;
        return "+OK\r\n";
    }

    return "-ERR unknown command '" + cmd.name + "'\r\n";
}

void ClientHandler::sendResponse(const std::string& msg) {
    if (msg.empty()) return;
    send(fd_, msg.c_str(), msg.size(), 0);
}
