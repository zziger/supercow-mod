#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>

class IMemoryCacheStorage
{
public:
    virtual ~IMemoryCacheStorage() = default;
    virtual uintptr_t Get(uint32_t hash) = 0;
    virtual void Put(uint32_t hash, uintptr_t address) = 0;
    virtual uint32_t Hash(const std::string& pattern) = 0;
};

class MemoryCacheStorage final : public IMemoryCacheStorage {
public:
    uintptr_t Get(const uint32_t hash) override {
        return _map[hash];
    }

    void Put(const uint32_t hash, const uintptr_t address) override {
        _map[hash] = address;
    }

    uint32_t Hash(const std::string& pattern) override {
        static std::hash<std::string> hash;
        return hash(pattern);
    }
	
private:
    std::unordered_map<uint32_t, uintptr_t> _map;
};
