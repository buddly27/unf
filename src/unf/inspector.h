#ifndef USD_NOTICE_FRAMEWORK_REPORTER_MANAGER_H
#define USD_NOTICE_FRAMEWORK_REPORTER_MANAGER_H

#include "unf/api.h"
#include "unf/collector.h"
#include "unf/registry.h"
#include "unf/reporter.h"

#include <pxr/base/tf/refBase.h>
#include <pxr/base/tf/refPtr.h>
#include <pxr/base/tf/weakBase.h>
#include <pxr/base/tf/weakPtr.h>
#include <pxr/pxr.h>
#include <pxr/usd/usd/common.h>
#include <pxr/usd/usd/stage.h>

#include <string>

namespace unf {

class Inspector;

/// Convenient alias for Inspector reference pointer.
using InspectorPtr = TfRefPtr<Inspector>;

/// Convenient alias for Inspector weak pointer.
using InspectorWeakPtr = TfWeakPtr<Inspector>;

/// Convenient alias for Reporter reference pointer.
using ReporterPtr = PXR_NS::TfRefPtr<Reporter>;

/// \class Inspector
///
/// \brief
/// Intermediate object handling the creation and management of Reporter
/// instances per stage.
class Inspector : public PXR_NS::TfRefBase, public PXR_NS::TfWeakBase {
  public:
    /// \brief
    /// Create an inpector from a Usd Stage.
    ///
    /// If an inspector has already been created from this \p stage, it
    /// will be returned. Otherwise, a new one will be created and returned.
    UNF_API static InspectorWeakPtr Create(
        const PXR_NS::UsdStageWeakPtr& stage);

    UNF_API virtual ~Inspector() = default;

    /// \brief
    /// Return number of reporters created.
    UNF_API size_t GetNumReporters();

    /// \brief
    /// Return number of inspectors created.
    UNF_API static size_t GetNumInspectors();

    /// \brief
    /// Return reporter reference associated with reporter of class `T`.
    template <class T>
    PXR_NS::TfWeakPtr<T> GetReporter()
    {
        return _collector.Get<T>();
    }

    /// \brief
    /// Create and register a new reporter.
    ///
    /// This will call the Reporter::Register method.
    template <class T>
    void AddReporter()
    {
        const auto& reporter = _collector.Add<T>(_stage);
        reporter->Register();
    }

    /// \brief
    /// Un-register inspector.
    ///
    /// \warning
    /// The inspector is not safe to use after this call.
    UNF_API void Reset();

    /// \brief
    /// Un-register inspector associated with Usd Stage.
    UNF_API static void Reset(const PXR_NS::UsdStageWeakPtr& stage);

    /// \brief
    /// Un-register all inspectors.
    UNF_API static void ResetAll();

  private:
    Inspector(const PXR_NS::UsdStageWeakPtr& stage);

    /// Record each hashed stage pointer to its corresponding inspector pointer.
    static Registry<Inspector> _registry;

    /// Usd Stage associated with inspector.
    PXR_NS::UsdStageWeakPtr _stage;

    /// Collection of Reporter instances.
    Collector<Reporter> _collector;

    friend class Registry<Inspector>;
};

}  // namespace unf

#endif  // USD_NOTICE_FRAMEWORK_REPORTER_MANAGER_H
