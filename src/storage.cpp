#include "storage.h"

bool Storage::isExpired(const Entry& entry) const {
    if (!entry.has_expiry) return false;
    return std::chrono::steady_clock::now() >= entry.expiry;
}

void Storage::set(const std::string& key, const std::string& value, int ttl_seconds) {
    std::lock_guard<std::mutex> lock(mutex_);
    Entry entry;
    entry.value = value;
    if (ttl_seconds > 0) {
        entry.has_expiry = true;
        entry.expiry = std::chrono::steady_clock::now() + std::chrono::seconds(ttl_seconds);
    }
    data_[key] = std::move(entry);
}

std::optional<std::string> Storage::get(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = data_.find(key);
    if (it == data_.end()) return std::nullopt;
    if (isExpired(it->second)) {
        data_.erase(it);
        return std::nullopt;
    }
    return it->second.value;
}

bool Storage::del(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = data_.find(key);
    if (it == data_.end()) return false;
    data_.erase(it);
    return true;
}

bool Storage::exists(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = data_.find(key);
    if (it == data_.end()) return false;
    if (isExpired(it->second)) {
        data_.erase(it);
        return false;
    }
    return true;
}

std::vector<std::string> Storage::keys() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> result;
    for (const auto& [k, v] : data_) {
        if (!isExpired(v)) {
            result.push_back(k);
        }
    }
    return result;
}

bool Storage::expire(const std::string& key, int seconds) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = data_.find(key);
    if (it == data_.end()) return false;
    if (isExpired(it->second)) {
        data_.erase(it);
        return false;
    }
    it->second.has_expiry = true;
    it->second.expiry = std::chrono::steady_clock::now() + std::chrono::seconds(seconds);
    return true;
}
