#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <mutex>
#include <chrono>

struct Entry {
    std::string value;
    std::chrono::steady_clock::time_point expiry;
    bool has_expiry = false;
};

class Storage {
public:
    void set(const std::string& key, const std::string& value, int ttl_seconds = -1);
    std::optional<std::string> get(const std::string& key);
    bool del(const std::string& key);
    bool exists(const std::string& key);
    std::vector<std::string> keys();
    bool expire(const std::string& key, int seconds);

private:
    bool isExpired(const Entry& entry) const;

    std::unordered_map<std::string, Entry> data_;
    mutable std::mutex mutex_;
};
