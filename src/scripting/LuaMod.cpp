#include "LuaMod.h"

#include "Utils.h"
#include "sdk/Game.h"
void LuaMod::OnLoad() {
    if (info.luaScript == "") throw Error("LuaMod был заинициализирован, но скрипт не был найден");

    const auto fullPath = info.basePath / info.luaScript; 
    if (!exists(fullPath))  throw Error(std::format("Файл {} не найден", fullPath.generic_string()));

    lua = new LuaContext(true);
    
    lua->appendPath((info.basePath / "?.lua").generic_string());
    lua->writeVariable("MODULENAME", utils::get_module_name()); // useful for FFI
    lua->writeFunction("logDebug", [](std::string str) {
        Log::Debug << str << Log::Endl;
    });
    
    sdk::Game::AddToLua(*lua);
    
    auto stream = std::ifstream{fullPath};
    lua->executeCode(stream);
}

void LuaMod::OnUnload() {
    delete lua;
    lua = nullptr;
}

void LuaMod::RenderUI() {
    if (!lua) return;
    try {
        auto fn = lua->readVariable<tl::optional<std::function<void()>>>("render");
        if (fn) (*fn)();
    } catch(std::exception& e) {
        Log::Error << "Error while rendering: " << e.what() << Log::Endl;
    }
    // todo move error handling to outer scope
}