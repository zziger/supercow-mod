#pragma once

#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <windows.h>

class Log
{
    enum Type
    {
        INFO,
        DEBUG,
        WARN,
        ERR,
        GAME
    } type = INFO;
    
    typedef Log& (*LogFn)(Log&);

    std::ofstream logfile;
    Log() : logfile("modlog.txt", std::ios::app) {}
    
public:
    
    enum Color
    {
        BLUE = FOREGROUND_BLUE,
        GREEN = FOREGROUND_GREEN,
        RED = FOREGROUND_RED,
        CYAN = BLUE | GREEN,
        PURPLE = BLUE | RED,
        YELLOW = GREEN | RED,
        WHITE = BLUE | RED | GREEN,
        LIGHT_BLUE = BLUE | FOREGROUND_INTENSITY,
        LIGHT_GREEN = GREEN | FOREGROUND_INTENSITY,
        LIGHT_RED = RED | FOREGROUND_INTENSITY,
        LIGHT_CYAN = CYAN | FOREGROUND_INTENSITY,
        LIGHT_PURPLE = PURPLE | FOREGROUND_INTENSITY,
        LIGHT_YELLOW = YELLOW | FOREGROUND_INTENSITY,
        LIGHT_WHITE = WHITE | FOREGROUND_INTENSITY,
        BLACK = 0,
        GRAY = FOREGROUND_INTENSITY,
    };
    
    Log(const Log&) = delete;
    Log(Log&&) = delete;
    Log& operator=(const Log&) = delete;

    template<class T>
    Log& Put(const T& val)
    {
        std::cout << val;
        logfile << val;
        logfile.flush();
        return *this;
    }

    Log& Put(const Color& val)
    {
        const auto hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        FlushConsoleInputBuffer(hConsole);
        SetConsoleTextAttribute(hConsole, val);
        return *this;
    }
    
    Log& Put(LogFn val)
    {
        return val(*this);
    }
    
    template<class T>
    Log& operator<<(const T& val)
    {
        return Put(val);
    }
    
    Log& SetType(Type _type)
    {
        type = _type;
        return *this;
    }
    static constexpr struct Log_Info
    {
        template<class T>
        Log& operator<<(const T& val) const
        {
            return Instance().SetType(INFO).Put(Prefix).Put(val);
        }
    } Info{};

    static constexpr struct Log_Debug
    {
        template<class T>
        Log& operator<<(const T& val) const
        {
            return Instance().SetType(DEBUG).Put(Prefix).Put(val);
        }
    } Debug{};

    static constexpr struct Log_Warn
    {
        template<class T>
        Log& operator<<(const T& val) const
        {
            return Instance().SetType(WARN).Put(Prefix).Put(val);
        }
    } Warn{};

    static constexpr struct Log_Error
    {
        template<class T>
        Log& operator<<(const T& val) const
        {
            return Instance().SetType(ERR).Put(Prefix).Put(val);
        }
    } Error{};

    static constexpr struct Log_Game
    {
        template<class T>
        Log& operator<<(const T& val) const
        {
            return Instance().SetType(GAME).Put(Prefix).Put(val);
        }
    } Game{};
    
    static Log& Prefix(Log& log)
    {
        tm newTimeHandle{};
        const time_t currentTimeHandle = time(0);
        localtime_s(&newTimeHandle, &currentTimeHandle);
        std::ostringstream TimeDateString;
        TimeDateString << std::put_time(&newTimeHandle, "[%H:%M:%S] ");
        log << GRAY << TimeDateString.str();
        switch (log.type) {
            case INFO: log << LIGHT_CYAN << "[INFO]  " << WHITE; break;
            case DEBUG: log << LIGHT_PURPLE << "[DEBUG] " << WHITE; break;
            case WARN: log << LIGHT_YELLOW << "[WARN]  "; break;
            case ERR: log << LIGHT_RED << "[ERROR] "; break;
            case GAME: log << CYAN << "[GAME]  " << GRAY; break;
        }
        return log;
    }
    
    static Log& Endl(Log& log)
    {
        log << WHITE << "\n";
        return log;
    }
    
    static Log& Instance()
    {
        static Log instance;
        return instance;
    }
};