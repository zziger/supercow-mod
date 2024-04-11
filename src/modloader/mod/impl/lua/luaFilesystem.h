#pragma once
#include "lua.h"
#include <Data.h>

namespace sol::stack
{
    inline std::filesystem::path pop_path(lua_State* L)
    {
        return sol::stack::check_usertype<std::filesystem::path>(L)
            ? sol::stack::pop<std::filesystem::path>(L)
            : sol::stack::pop<std::string>(L);
    }
}

namespace lua::helpers
{
    inline std::filesystem::copy_options makeCopyFlagsFromLua(const sol::table& table)
    {
        using namespace std::filesystem;
        copy_options flags = copy_options::none;

        const std::optional<bool> recursive = table["recursive"];
        if (!recursive)
            throw std::exception("recursive was not bool");
        if (*recursive)
            flags |= copy_options::recursive;

        if (const std::optional<std::string> existing = table["existing"])
        {
            if (*existing == "skip")
                flags |= copy_options::skip_existing;
            if (*existing == "update")
                flags |= copy_options::update_existing;
            if (*existing == "overwrite")
                flags |= copy_options::overwrite_existing;
            else
                throw std::exception("invalid recursive value: should be 'skip', 'update' or 'overwrite'");
        }
        else if (!table["existing"].is<sol::nil_t>())
        {
            throw std::exception("recursive was not string");
        }

        if (const std::optional<std::string> symlinks = table["symlinks"])
        {
            if (symlinks == "copy")
                flags |= copy_options::copy_symlinks;
            if (symlinks == "skip")
                flags |= copy_options::skip_symlinks;
            else
                throw std::exception("invalid symlinks value: should be 'skip' or 'copy'");
        }
        else if (!table["symlinks"].is<sol::nil_t>())
        {
            throw std::exception("symlinks was not string");
        }

        return flags;
    };
}

inline void registerLuaFilesystem(sol::table table)
{
    using namespace std::filesystem;
    using string = std::string;
    using namespace lua::helpers;

    table.new_usertype<path>(
        "path", sol::call_constructor,
        sol::constructors<path(), path(std::string)>(),

        sol::meta_function::to_string,
        static_cast<std::string (path::*)() const>(&path::generic_string),

        sol::meta_function::division, [](lua_State* L) -> int
        {
            const path other = sol::stack::pop_path(L);
            const path self = sol::stack::pop_path(L);

            sol::stack::push(L, self / other);
            return 1;
        },

        "clear", &path::clear,
        "removeFilename", &path::remove_filename,
        "replaceFilename", &path::replace_filename,
        "replaceExtension", &path::replace_filename,

        "parentPath", &path::parent_path,
        "filename", &path::filename,
        "stem", &path::stem,
        "extension", &path::extension,

        "isEmpty", &path::empty,
        "hasParentPath", &path::has_parent_path,
        "hasFilename", &path::has_filename,
        "hasStem", &path::has_stem,
        "hasExtension", &path::extension,

        "isAbsolute", &path::is_absolute,
        "isRelative", &path::is_relative
    );

    table["currentPath"] = sol::overload_conv<
        path (),
        void (const path&),
        void (const std::string&)
    >([](auto... args) { return current_path(args...); });

    table["tempDirectoryPath"] = sol::resolve<path()>(&temp_directory_path); // tempDirectoryPath() -> path

    table["absolute"] = sol::overload_conv<
        path (const path&),
        path (const string&)
    >([](auto... args) { return absolute(args...); });
    table["relative"] = sol::overload_conv<
        path (const path&),
        path (const string&),
        path (const path&, const path&),
        path (const string&, const string&),
        path (const path&, const string&),
        path (const string&, const path&)
    >([](auto... args) { return relative(args...); });

    table["isDirectory"] = sol::overload_conv<
        bool (const path&),
        bool (const string&)
    >([](auto... args) { return is_directory(args...); });
    table["isEmpty"] = sol::overload_conv<
        bool (const path&),
        bool (const string&)
    >([](auto... args) { return is_empty(args...); });
    table["exists"] = sol::overload_conv<
        bool (const path&),
        bool (const string&)
    >([](auto... args) { return exists(args...); });

    table["remove"] = sol::overload_conv<
        bool (const path&),
        bool (const string&)
    >([](auto... args) { return remove(args...); });
    table["removeAll"] = sol::overload_conv<
        uint64_t (const path&),
        uint64_t (const string&)
    >([](auto... args) { return remove_all(args...); });

    table["createDirectory"] = sol::overload_conv<
        bool (const path&, const path&),
        bool (const string&, const string&),
        bool (const path&, const string&),
        bool (const string&, const path&)
    >([](auto... args) { return create_directory(args...); });
    table["createDirectories"] = sol::overload_conv<
        bool (const path&),
        bool (const string&)
    >([](auto... args) { return create_directories(args...); });

    table["copy"] = sol::overload(
        sol::resolve<void(const path&, const path&)>(&copy), // copy(path, path) -> void
        [](const std::string& from, const std::string& to) -> void { return copy(from, to); }, // copy(string, string) -> void
        [](const path& from, const path& to, const sol::table& options) -> void { copy(from, to, makeCopyFlagsFromLua(options)); }, // copy(path, path, options) -> void
        [](const std::string& from, const std::string& to, const sol::table& options) -> void { copy(path(from), path(from), makeCopyFlagsFromLua(options)); } // copy(string, string, options) -> void
    );
    table["move"] = table["rename"] = sol::overload_conv<
        void (const path&, const path&),
        void (const string&, const string&)
    >([](auto... args) { return rename(args...); });
}
