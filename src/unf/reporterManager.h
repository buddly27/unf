#ifndef USD_NOTICE_FRAMEWORK_REPORTER_MANAGER_H
#define USD_NOTICE_FRAMEWORK_REPORTER_MANAGER_H

#include "unf/reporter.h"

#include <pxr/base/plug/plugin.h>
#include <pxr/base/plug/registry.h>
#include <pxr/base/tf/refBase.h>
#include <pxr/base/tf/refPtr.h>
#include <pxr/base/tf/weakBase.h>
#include <pxr/base/tf/weakPtr.h>
#include <pxr/pxr.h>
#include <pxr/usd/usd/common.h>
#include <pxr/usd/usd/stage.h>

#include <string>
#include <typeinfo>
#include <unordered_map>

namespace unf {

class ReporterManager;

/// Convenient alias for ReporterManager reference pointer.
using ReporterManagerPtr = TfRefPtr<ReporterManager>;

/// Convenient alias for ReporterManager weak pointer.
using ReporterManagerWeakPtr = TfWeakPtr<ReporterManager>;

/// Convenient alias for Reporter reference pointer.
using ReporterPtr = PXR_NS::TfRefPtr<Reporter>;

/// \class ReporterManager
///
/// \brief
/// Intermediate object handling the creation and management of Reporter
/// instances per stage.
class ReporterManager : public PXR_NS::TfRefBase, public PXR_NS::TfWeakBase {
  public:
    /// \brief
    /// Create a manager from a Usd Stage.
    ///
    /// If a manager has already been created from this \p stage, it will be
    /// returned. Otherwise, a new one will be created and returned.
    static ReporterManagerWeakPtr Create(const PXR_NS::UsdStageWeakPtr& stage);

    virtual ~ReporterManager() = default;

    /// \brief
    /// Return number of reporters created.
    size_t GetNumReporters();

    /// \brief
    /// Return number of factory generators added.
    static size_t GetNumManagers();

    /// \brief
    /// Return reporter reference associated with \p identifier.
    template <class T>
    PXR_NS::TfRefPtr<T> GetReporter();

    /// \brief
    /// Create and register a new reporter.
    ///
    /// This will call the Reporter::Register method.
    template <class T>
    void AddReporter();

    /// \brief
    /// Un-register manager.
    ///
    /// \warning
    /// The manager is not safe to use after this call.
    void Reset();

    /// \brief
    /// Un-register manager associated with Usd Stage.
    static void Reset(const PXR_NS::UsdStageWeakPtr& stage);

    /// \brief
    /// Un-register all managers.
    static void ResetAll();

  private:
    ReporterManager(const PXR_NS::UsdStageWeakPtr& stage);

    /// Un-register managers targeting expired stages.
    static void _CleanCache();

    /// Discover all reporters registered as plugins.
    void _DiscoverReporters();

    /// Register reporter within manager by its identifier.
    void _Add(const ReporterPtr&);

    /// Create and register reporter within manager without running the
    /// Reporter::Register method.
    template <class T>
    ReporterPtr _AddReporter();

    /// Load all reporters from discovered factory types.
    template <class OutputPtr, class OutputFactory>
    void _LoadFromPlugins(const PXR_NS::TfType& type);

    struct UsdStageWeakPtrHasher {
        std::size_t operator()(const PXR_NS::UsdStageWeakPtr& ptr) const
        {
            return hash_value(ptr);
        }
    };

    /// Record each hashed stage pointer to its corresponding manager pointer.
    static std::unordered_map<
        PXR_NS::UsdStageWeakPtr, ReporterManagerPtr, UsdStageWeakPtrHasher>
        Registry;

    /// Record each Reporter created and registered per Reporter type name.
    std::unordered_map<std::string, ReporterPtr> _reporters;

    /// Usd Stage associated with manager.
    PXR_NS::UsdStageWeakPtr _stage;
};

template <class T>
PXR_NS::TfRefPtr<T> ReporterManager::GetReporter()
{
    auto identifier = typeid(T).name();

    if (_reporters.find(identifier) == _reporters.end()) {
        return nullptr;
    }
    return TfStatic_cast<TfWeakPtr<T>>(_reporters.at(identifier));
}

template <class T>
void ReporterManager::AddReporter()
{
    const auto& reporter = _AddReporter<T>();
    reporter->Register();
}

template <class T>
ReporterPtr ReporterManager::_AddReporter()
{
    static_assert(std::is_base_of<Reporter, T>::value);
    ReporterPtr reporter = PXR_NS::TfCreateRefPtr(new T(_stage));
    _Add(reporter);
    return reporter;
}

template <class OutputPtr, class OutputFactory>
void ReporterManager::_LoadFromPlugins(const PXR_NS::TfType& type)
{
    PXR_NAMESPACE_USING_DIRECTIVE

    const PXR_NS::PlugPluginPtr plugin =
        PXR_NS::PlugRegistry::GetInstance().GetPluginForType(type);

    if (!plugin) {
        return;
    }

    if (!plugin->Load()) {
        TF_CODING_ERROR(
            "Failed to load plugin %s for %s",
            plugin->GetName().c_str(),
            type.GetTypeName().c_str());
        return;
    }

    OutputPtr output;
    OutputFactory* factory = type.GetFactory<OutputFactory>();

    if (factory) {
        output = factory->New(_stage);
    }

    if (!output) {
        TF_CODING_ERROR(
            "Failed to manufacture %s from plugin %s",
            type.GetTypeName().c_str(),
            plugin->GetName().c_str());
        return;
    }

    _Add(output);
}

}  // namespace unf

#endif  // USD_NOTICE_FRAMEWORK_REPORTER_MANAGER_H
