#pragma once
#include "exceptions/Error.h"
#include "modloader/mods/Mod.h"
#include "thirdparty/LuaContext.h"

class LuaMod final : public Mod {
public:
    explicit LuaMod(ModInfo info) : Mod(std::move(info)), lua(nullptr) {}

    LuaContext* lua;

    void OnLoad() override;

    void OnUnload() override;

    void RenderUI() override;
};
