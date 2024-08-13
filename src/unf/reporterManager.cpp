#include "unf/reporterManager.h"
#include "unf/reporter.h"

#include <string>
#include <typeinfo>
#include <unordered_map>

namespace unf {

Registry<ReporterManager> ReporterManager::_registry;

ReporterManager::ReporterManager(const PXR_NS::UsdStageWeakPtr& stage)
    : _stage(stage)
{
    // Discover reporters added via plugin.
    _collector.Discover<ReporterFactory>(_stage);

    // Register all reporters
    for (auto& item : _collector.GetItems()) {
        item.second->Register();
    }
}

ReporterManagerWeakPtr ReporterManager::Create(
    const PXR_NS::UsdStageWeakPtr& stage)
{
    return TfWeakPtr<ReporterManager>(_registry.Add(stage));
}

size_t ReporterManager::GetNumReporters() { return _collector.Size(); }

size_t ReporterManager::GetNumManagers() { return _registry.Size(); }

void ReporterManager::Reset() { _registry.Remove(_stage); }

void ReporterManager::Reset(const PXR_NS::UsdStageWeakPtr& stage)
{
    _registry.Remove(stage);
}

void ReporterManager::ResetAll() { _registry.Clear(); }

}  // namespace unf
