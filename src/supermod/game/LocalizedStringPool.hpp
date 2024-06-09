﻿#pragma once
#include <supermod/pch.hpp>

namespace game
{
struct LocalizedString
{
    wchar_t key[20];
    wchar_t* localizedStr;
};

class LocalizedStringPool
{
public:
    LocalizedString strings[1024];
    int stringAmount;

    static LocalizedStringPool* GetInstance();
};
} // namespace game