#ifndef USD_NOTICE_FRAMEWORK_REPORTER_H
#define USD_NOTICE_FRAMEWORK_REPORTER_H

#include <pxr/pxr.h>
#include <pxr/usd/sdf/path.h>
#include <pxr/usd/sdf/types.h>
#include <pxr/usd/usd/api.h>
#include <pxr/usd/usd/common.h>
#include <pxr/usd/usd/notice.h>
#include <pxr/usd/usd/primRange.h>
#include <pxr/usd/usd/stage.h>

#include <vector>
#include <typeinfo>
#include <string>

namespace unf {

class Reporter;

/// Convenient alias for reporter reference pointer.
using ReporterPtr = TfRefPtr<Reporter>;

/// Convenient alias for reporter weak pointer.
using ReporterWeakPtr = TfWeakPtr<Reporter>;

/// \class Reporter
///
/// \brief
/// Interface for objects reacting to incoming PXR_NS::TfNotice derived
/// notices.
class Reporter : public PXR_NS::TfRefBase, public PXR_NS::TfWeakBase {
  public:
    /// Revoke all registered listeners on destruction.
    virtual ~Reporter();

    virtual std::string GetIdentifier() const { return typeid(*this).name(); }

    /// Register listeners to PXR_NS::TfNotice derived notices.
    virtual void Register() = 0;

    /// Revoke all registered listeners.
    virtual void Revoke();

  protected:
    /// Create a reporter targeting a stage.
    Reporter(const UsdStageWeakPtr& stage);

    /// List of handle-objects used for registering listeners.
    std::vector<PXR_NS::TfNotice::Key> _keys;

    /// Usd Stage associated with reporter.
    UsdStageWeakPtr _stage;

    friend class ReporterManager;
};

/// \class ReporterFactory
///
/// \brief
/// Interface for building Reporter type.
///
/// \sa
/// ReporterFactoryImpl
class ReporterFactory : public PXR_NS::TfType::FactoryBase {
  public:
    /// Base constructor to create a Reporter.
    virtual PXR_NS::TfRefPtr<Reporter> New(const UsdStageWeakPtr&) const = 0;
};

/// \class ReporterFactoryImpl
///
/// \brief
/// Templated factory class which creates a specific type of Reporter.
///
/// \sa
/// ReporterDefine
template <class T>
class ReporterFactoryImpl : public ReporterFactory {
  public:
    /// Create a Reporter and return reference pointer.
    virtual PXR_NS::TfRefPtr<Reporter> New(
        const UsdStageWeakPtr& stage) const override
    {
        return PXR_NS::TfCreateRefPtr(new T(stage));
    }
};

/// \fn ReporterDefine
///
/// \brief
/// Define a PXR_NS::TfType for a specific type of Reporter.
///
/// Typical usage to define a type for a reporter \p Foo would be:
///
/// \code{.cpp}
/// TF_REGISTRY_FUNCTION(PXR_NS::TfType)
/// {
///     ReporterDefine<Foo, Reporter>();
/// }
/// \endcode
template <class T, class... Bases>
void ReporterDefine()
{
    PXR_NS::TfType::Define<T, PXR_NS::TfType::Bases<Bases...> >()
        .template SetFactory<ReporterFactoryImpl<T> >();
}

}  // namespace unf

#endif  // USD_NOTICE_FRAMEWORK_REPORTER_H
