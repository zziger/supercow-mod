#pragma once
#include <exception>
#include <string_view>

class Error final : public std::exception {
public:
    explicit Error(std::string message);
};
