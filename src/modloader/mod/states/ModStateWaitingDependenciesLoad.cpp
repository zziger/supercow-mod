#include "ModStateWaitingDependenciesLoad.h"

#include <modloader/ModManager.h>
#include <modloader/mod/Mod.h>

#include "ModStateDisabled.h"
#include "ModStateEnabled.h"
#include "ModStateWaitingDependentsUnload.h"

using namespace std::chrono;

void modloader::ModStateWaitingDependenciesLoad::Init(Mod& mod)
{
    if (Check(mod)) mod.SetState<ModStateEnabled>();
}

void modloader::ModStateWaitingDependenciesLoad::Update(Mod& mod)
{
    if (!mod.IsEnabled())
    {
        mod.SetState<ModStateWaitingDependentsUnload>();
        return;
    }

    if (Check(mod)) mod.SetState<ModStateEnabled>();
}

std::string modloader::ModStateWaitingDependenciesLoad::GetIcon()
{
    auto animatedIcon = ICON_MD_HOURGLASS_TOP;
    if (duration_cast<seconds>(system_clock::now().time_since_epoch()).count() % 2 == 0)
        animatedIcon = ICON_MD_HOURGLASS_BOTTOM;

    return std::string(ICON_MD_FORMAT_LIST_BULLETED) + animatedIcon;
}

bool modloader::ModStateWaitingDependenciesLoad::Check(const Mod& mod)
{
    auto deps = mod.GetInfo()->dependencies;
    if (deps.empty()) return true;
    return std::ranges::all_of(deps, [](const ModInfo::Dependency& dependency) -> bool {
        const auto mod = ModManager::FindModByID(dependency.id);
        return mod && mod->IsActive() && dependency.version.Match(mod->GetInfo()->version);
    });
}
