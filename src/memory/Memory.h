// CMemory.h by Tuxick https://github.com/emcifuntik
#pragma once

#include <functional>
#include <iomanip>
#include <sstream>
#include <vector>

#include <MinHook.h>
#include <Psapi.h>
#include <Windows.h>
#include <cstdint>

#define MAX_SIG_SIZE 128

class Memory
{
    uintptr_t _address;

public:
    using pattern_not_found_callback = void (*)(const std::string& pattern);

    static pattern_not_found_callback OnPatternNotFound(const pattern_not_found_callback cb = nullptr)
    {
        static pattern_not_found_callback callback = nullptr;
        if (cb != nullptr)
            callback = cb;
        return callback;
    }

    class Pattern;
    static inline std::list<std::pair<Pattern, uintptr_t>> cache{};

    class Pattern
    {
        static bool Match(const BYTE* addr, const BYTE* pat, const BYTE* msk)
        {
            size_t n = 0;

            while (addr[n] == pat[n] || msk[n] == (BYTE)'?')
            {
                if (!msk[++n])
                    return true;
            }

            return false;
        }

        static BYTE* Find(BYTE* rangeStart, const int len, const char* sig, const char* mask)
        {
            const size_t maskLength = strlen(mask);
            for (int n = 0; n < len - maskLength; ++n)
            {
                if (Match(rangeStart + n, (BYTE*)sig, (BYTE*)mask))
                {
                    return rangeStart + n;
                }
            }
            return nullptr;
        }

        constexpr static unsigned char FromHex(const char c)
        {
            if (c >= 'a' && c <= 'f')
                return c - 'a' + 10;
            if (c >= 'A' && c <= 'F')
                return c - 'A' + 10;
            if (c >= '0' && c <= '9')
                return c - '0';

            return 0;
        }

    public:
        char sig[MAX_SIG_SIZE];
        char mask[MAX_SIG_SIZE];
        std::size_t size;

        bool Includes(const Pattern& pat) const
        {
            if (this->size > pat.size)
                return false;
            return Match((BYTE*)pat.sig, (BYTE*)this->sig, (BYTE*)this->mask);
        }

        template <std::size_t Size>
        explicit constexpr Pattern(const char (&idaPattern)[Size]) : sig{0},
                                                                     mask{0},
                                                                     size(0)
        {
            std::size_t j = 0;
            std::size_t q = 0;
            for (std::size_t i = 0; i < Size; ++i, ++j)
            {
                const char c = idaPattern[i];

                if (c >= 'a' && c <= 'f' || c >= 'A' && c <= 'F' || c >= '0' && c <= '9')
                {
                    sig[j] = (FromHex(c) << 4) + FromHex(idaPattern[i + 1]);
                    mask[j] = 'x';
                    i += 2;
                    q = 0;
                }
                else if (c == '?')
                {
                    sig[j] = '\x00';
                    mask[j] = '?';
                    ++i;
                    ++q;
                }
                else
                {
                    --j;
                }
            }

            size = j - q;
        }
        explicit Pattern(const char* idaPattern, int size) : sig{0}, mask{0}, size(0)
        {
            std::size_t j = 0;
            std::size_t q = 0;
            for (std::size_t i = 0; i < size; ++i, ++j)
            {
                const char c = idaPattern[i];

                if (c >= 'a' && c <= 'f' || c >= 'A' && c <= 'F' || c >= '0' && c <= '9')
                {
                    sig[j] = (FromHex(c) << 4) + FromHex(idaPattern[i + 1]);
                    mask[j] = 'x';
                    i += 2;
                    q = 0;
                }
                else if (c == '?')
                {
                    sig[j] = '\x00';
                    mask[j] = '?';
                    ++i;
                    ++q;
                }
                else
                {
                    --j;
                }
            }

            this->size = j - q;
        }

        Memory Search(const bool useCaching = true, const uintptr_t base = 0) const
        {
            if (useCaching)
            {
                for (auto& pair : cache)
                {
                    if (pair.first.Includes(*this))
                    {
                        // Log::Debug << "Found " << this->ToString() << " from cached " << pair.first.ToString() <<
                        // Log::Endl;
                        return Memory(pair.second);
                    }
                }
            }

            BYTE* res = Find(base == 0 ? (BYTE*)Base() : (BYTE*)base, (int)GetSize(), sig, mask);

            if (!res)
            {
                const std::string patString = ToString();
                if (OnPatternNotFound())
                    OnPatternNotFound()(patString);
                return Memory(UINTPTR_MAX, false);
            }

            if (useCaching)
            {
                auto addr = (uintptr_t)res - Memory::Base();
                bool found = false;

                for (auto& pair : cache)
                {
                    if (this->Includes(pair.first) && pair.second == addr)
                    {
                        // Log::Debug << "Found worse pattern! Replacing " << pair.first.ToString() << " with " <<
                        // this->ToString() << Log::Endl;
                        found = true;
                        pair.first = *this;
                    }
                }

                if (!found)
                {
                    // Log::Debug << "Did not found worse pattern! Adding " << this->ToString() << Log::Endl;
                    cache.push_back({*this, addr});
                }
            }
            return Memory(res);
        }

        std::string ToString() const
        {
            std::stringstream ss;

            for (size_t i = 0; i < size; ++i)
            {
                if (mask[i] == 'x')
                    ss << std::setfill('0') << std::setw(2) << std::hex << (int)(unsigned char)sig[i] << " ";
                else
                    ss << "? ";
            }

            return ss.str();
        }
    };

    static size_t GetSize()
    {
        static MODULEINFO info{nullptr};
        GetModuleInformation(GetCurrentProcess(), nullptr, &info, sizeof(MODULEINFO));
        return info.SizeOfImage;
    }

    static uintptr_t& Base()
    {
        static uintptr_t _Base;
        return _Base;
    }

    struct Hook
    {
        static std::vector<std::function<void()>>& Hooks()
        {
            static std::vector<std::function<void()>> _Hooks;
            return _Hooks;
        }

        Hook(const std::function<void()>& fn) { Hooks().push_back(fn); }
    };

    static void RunHooks()
    {
        for (auto& fn : Hook::Hooks())
            fn();
    }

    explicit Memory(const uintptr_t address, const bool addBase = true) : _address(addBase ? Base() + address : address)
    {
    }
    explicit Memory(void* address) : _address((uintptr_t)address) {}
    explicit Memory() : _address(0) {}

    bool IsValid() const { return _address != UINTPTR_MAX; }

    uintptr_t GetAddress() const { return _address; }

    Memory& Add(int offset)
    {
        _address += offset;
        return *this;
    }

    void FarJump(void* func)
    {
        MemUnlock lock(_address, 12);

        *((WORD*)_address) = 0xB848;
        *((intptr_t*)(_address + 2)) = (uintptr_t)func;
        *((WORD*)(_address + 10)) = 0xE0FF;
    }

    void FarJump(const Memory& func) { FarJump((void*)func._address); }

    void NearCall(void* func)
    {
        Put<uint8_t>(0xE8);
        (*this + 1).Put(int32_t(reinterpret_cast<uintptr_t>(func) - _address - 5));
    }

    void NearCall(const Memory& func) { NearCall((void*)func._address); }

    void SetOffset(uint64_t value, int offset = 1)
    {
        (*this + offset).Put(static_cast<int32_t>(value - _address - (4 + offset)));
    }

    void SetOffset(const Memory& func) { SetOffset(func._address); }

    void Nop(size_t len)
    {
        MemUnlock lock(_address, len);

        memset((void*)_address, 0x90, len);
    }

    void PutRetn() { Put<uint8_t>(0xC3); }

    Memory GetOffset(int offset = 3) const
    {
        const auto ptr = (int32_t*)(_address + offset);
        return Memory(*ptr + (_address + offset + 4), false);
    }

    Memory& operator+=(const int rhs)
    {
        _address += rhs;
        return *this;
    }

    Memory& operator-=(const int rhs)
    {
        _address -= rhs;
        return *this;
    }

    Memory operator+(const int right) const { return Memory(*this) += right; }

    Memory operator-(const int right) const { return Memory(*this) -= right; }

    bool operator==(const Memory& other) const { return _address == other._address; }

    template <typename R = void*, typename... Args>
    R Call(Args... args)
    {
        return ((R(*)(Args...))_address)(args...);
    }

    template <class T = void*>
    T Get(const int offset = 0) const
    {
        return (T)(_address + offset);
    }

    template <typename T>
    void Put(const T& value)
    {
        MemUnlock lock(_address, sizeof(T));

        memcpy((void*)_address, &value, sizeof(T));
    }

    template <std::size_t N>
    void Put(uint8_t (&BYTEs)[N])
    {
        Put<uint8_t[N]>(BYTEs);
    }

    template <class T>
    T GetCall() const
    {
        return (T)(*(int32_t*)(_address + 1) + _address + 5);
    }

    Memory& GoToNearCall()
    {
        Add(*(int32_t*)(_address + 1) + 5);
        return *this;
    }

#ifdef MH_ALL_HOOKS
    template <class T>
    bool Detour(T* fn, T** orig = nullptr, int32_t id = 0) const
    {
        MH_CreateHookEx(id, (void*)_address, (void*)fn, (void**)orig);
        return MH_EnableHookEx(id, (void*)_address) == MH_OK;
    }

    void DeactivateDetour(int32_t id = 0) const { MH_DisableHookEx(id, (void*)_address); }

    template <class T>
    bool ExclusiveDetour(T* fn, T** orig = nullptr) const
    {
        MH_CreateHook((void*)_address, (void*)fn, (void**)orig);
        return MH_EnableHook((void*)_address) == MH_OK;
    }

    void DeactivateExclusiveDetour() const { MH_DisableHook((void*)_address); }
#endif

private:
    struct MemUnlock
    {
        unsigned long rights;
        size_t size;
        void* address;

        MemUnlock(const uint64_t addr, const size_t len) : size(len), address((void*)addr)
        {
            VirtualProtect(address, len, PAGE_EXECUTE_READWRITE, &rights);
        }

        ~MemUnlock() { VirtualProtect(address, size, rights, nullptr); }
    };
};

template <>
struct std::hash<Memory>
{
    auto operator()(const Memory& mem) const noexcept -> size_t { return mem.GetAddress(); }
};
