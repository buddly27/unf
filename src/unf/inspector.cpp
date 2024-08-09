#include "unf/inspector.h"
#include "unf/reporter.h"

#include <pxr/pxr.h>

#include <string>
#include <typeinfo>
#include <unordered_map>

PXR_NAMESPACE_USING_DIRECTIVE

namespace unf {

Registry<Inspector> Inspector::_registry;

Inspector::Inspector(const UsdStageWeakPtr& stage) : _stage(stage)
{
    // Discover reporters added via plugin.
    _collector.Discover<ReporterFactory>(_stage);

    // Register all reporters
    for (auto& item : _collector.GetItems()) {
        item.second->Register();
    }
}

InspectorWeakPtr Inspector::Create(const UsdStageWeakPtr& stage)
{
    return TfWeakPtr<Inspector>(_registry.Add(stage));
}

size_t Inspector::GetNumReporters() { return _collector.Size(); }

size_t Inspector::GetNumInspectors() { return _registry.Size(); }

void Inspector::Reset() { _registry.Remove(_stage); }

void Inspector::Reset(const UsdStageWeakPtr& stage) { _registry.Remove(stage); }

void Inspector::ResetAll() { _registry.Clear(); }

}  // namespace unf
