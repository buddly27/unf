#include "unf/reporterManager.h"
#include "unf/reporter.h"

#include <string>
#include <typeinfo>
#include <unordered_map>

namespace unf {

std::unordered_map<
    UsdStageWeakPtr, ReporterManagerPtr, ReporterManager::UsdStageWeakPtrHasher>
    ReporterManager::Registry;

ReporterManager::ReporterManager(const PXR_NS::UsdStageWeakPtr& stage)
    : _stage(stage)
{
    // Discover reporters added via plugin.
    _DiscoverReporters();

    // Register all reporters
    for (auto& element : _reporters) {
        element.second->Register();
    }
}

ReporterManagerWeakPtr ReporterManager::Create(
    const PXR_NS::UsdStageWeakPtr& stage)
{
    _CleanCache();

    // If a manager doesn't exist for the given stage, create one.
    if (Registry.find(stage) == Registry.end()) {
        Registry[stage] = TfCreateRefPtr(new ReporterManager(stage));
    }

    return TfWeakPtr<ReporterManager>(Registry[stage]);
}

size_t ReporterManager::GetNumReporters() { return _reporters.size(); }

size_t ReporterManager::GetNumManagers() { return Registry.size(); }

void ReporterManager::Reset() { Registry.erase(_stage); }

void ReporterManager::Reset(const PXR_NS::UsdStageWeakPtr& stage)
{
    Registry.erase(stage);
}

void ReporterManager::ResetAll() { Registry.clear(); }

void ReporterManager::_CleanCache()
{
    for (auto it = Registry.begin(); it != Registry.end();) {
        // If the stage doesn't exist anymore, delete the corresponding
        // manager from the registry.
        if (it->first.IsExpired()) {
            it = Registry.erase(it);
        }
        else {
            it++;
        }
    }
}

void ReporterManager::_DiscoverReporters()
{
    TfType root = TfType::Find<Reporter>();
    std::set<TfType> types;
    PlugRegistry::GetAllDerivedTypes(root, &types);

    for (const TfType& type : types) {
        _LoadFromPlugins<ReporterPtr, ReporterFactory>(type);
    }
}

void ReporterManager::_Add(const ReporterPtr& reporter)
{
    auto identifier = typeid(*reporter).name();
    _reporters[identifier] = reporter;
}

}  // namespace unf
