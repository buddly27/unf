#ifndef USD_NOTICE_FRAMEWORK_BROKER_H
#define USD_NOTICE_FRAMEWORK_BROKER_H

/// \file unf/broker.h

#include "unf/api.h"
#include "unf/collector.h"
#include "unf/capturePredicate.h"
#include "unf/notice.h"
#include "unf/registry.h"

#include <pxr/base/plug/plugin.h>
#include <pxr/base/plug/registry.h>
#include <pxr/base/tf/refBase.h>
#include <pxr/base/tf/refPtr.h>
#include <pxr/base/tf/weakBase.h>
#include <pxr/base/tf/weakPtr.h>
#include <pxr/pxr.h>
#include <pxr/usd/usd/common.h>
#include <pxr/usd/usd/stage.h>

#include <functional>
#include <memory>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <vector>

namespace unf {

class Broker;
class Dispatcher;

/// Convenient alias for Broker reference pointer.
using BrokerPtr = PXR_NS::TfRefPtr<Broker>;

/// Convenient alias for Broker weak pointer.
using BrokerWeakPtr = PXR_NS::TfWeakPtr<Broker>;

/// Convenient alias for Dispatcher reference pointer.
using DispatcherPtr = PXR_NS::TfRefPtr<Dispatcher>;

/// Convenient alias for Dispatcher weak pointer.
using DispatcherWeakPtr = PXR_NS::TfWeakPtr<Dispatcher>;

/// \class Broker
///
/// \brief
/// Intermediate object between the Usd Stage and any clients that needs
/// asynchronous handling and upstream filtering of notices.
class Broker : public PXR_NS::TfRefBase, public PXR_NS::TfWeakBase {
  public:
    /// \brief
    /// Create a broker from a Usd Stage.
    ///
    /// If a broker has already been created from this \p stage, it will be
    /// returned. Otherwise, a new one will be created and returned.
    UNF_API static BrokerPtr Create(const PXR_NS::UsdStageWeakPtr& stage);

    UNF_API virtual ~Broker() = default;

    /// Remove default copy constructor.
    UNF_API Broker(const Broker&) = delete;

    /// Remove default assignment operator.
    UNF_API Broker& operator=(const Broker&) = delete;

    /// Return Usd Stage associated with the broker.
    UNF_API const PXR_NS::UsdStageWeakPtr GetStage() const { return _stage; }

    /// \brief
    /// Indicate whether a notice transaction has been started.
    /// \sa BeginTransaction
    UNF_API bool IsInTransaction();

    /// \brief
    /// Start a notice transaction.
    ///
    /// Notices derived from UnfNotice::StageNotice will be held during
    /// the transaction and emitted at the end.
    ///
    /// By default, all UnfNotice::StageNotice notices will be captured during
    /// the entire scope of the transaction. A CapturePredicate can be passed to
    /// influence which notices are captured. Notices that are not captured
    /// will not be emitted.
    ///
    /// \warning
    /// Each transaction started must be closed with EndTransaction.
    /// It is preferrable to use NoticeTransaction over this API to safely
    /// manage transactions.
    ///
    /// \sa EndTransaction
    /// \sa NoticeTransaction
    UNF_API void BeginTransaction(
        CapturePredicate predicate = CapturePredicate::Default());

    /// \brief
    /// Start a notice transaction with a capture predicate function.
    ///
    /// The following example will filter out all 'Foo' notices emitted during
    /// the transaction.
    ///
    /// \code{.cpp}
    /// broker->BeginTransaction([&](const unf::UnfNotice::StageNotice& n) {
    ///     return (n.GetTypeId() != typeid(Foo).name());
    /// });
    /// \endcode
    ///
    /// \warning
    /// Each transaction started must be closed with EndTransaction.
    /// It is preferrable to use NoticeTransaction over this API to safely
    /// manage transactions.
    ///
    /// \sa EndTransaction
    /// \sa NoticeTransaction
    UNF_API void BeginTransaction(const CapturePredicateFunc&);

    /// \brief
    /// Stop a notice transaction.
    ///
    /// This will trigger the emission of all captured
    /// UnfNotice::StageNotice notices. Each notice type will be
    /// consolidated before emission if applicable.
    ///
    /// \warning
    /// It is preferrable to use NoticeTransaction over this API to safely
    /// manage transactions.
    ///
    /// \sa BeginTransaction
    /// \sa NoticeTransaction
    UNF_API void EndTransaction();

    /// \brief
    /// Create and send a UnfNotice::StageNotice notice via the broker.
    ///
    /// \note
    /// The associated stage will be used as sender.
    template <class UnfNotice, class... Args>
    void Send(Args&&... args)
    {
        Send(UnfNotice::Create(std::forward<Args>(args)...));
    }

    /// \brief
    /// Send a UnfNotice::StageNotice notice via the broker.
    ///
    /// \note
    /// The associated stage will be used as sender.
    UNF_API void Send(const UnfNotice::StageNoticeRefPtr&);

    /// Return dispatcher reference associated with \p identifier.
    UNF_API DispatcherWeakPtr GetDispatcher(std::string identifier);

    /// \brief
    /// Create and register a new dispatcher.
    ///
    /// This will call the Dispatcher::Register method.
    template <class T>
    void AddDispatcher()
    {
        auto self = PXR_NS::TfCreateWeakPtr(this);
        const auto& dispatcher = _collector.Add<T>(self);
        dispatcher->Register();
    }

    /// \brief
    /// Un-register broker.
    ///
    /// \warning
    /// The broker is not safe to use after this call.
    UNF_API void Reset();

    /// Un-register all brokers.
    UNF_API static void ResetAll();

  private:
    Broker(const PXR_NS::UsdStageWeakPtr&);

    /// Record each hashed stage pointer to its corresponding broker pointer.
    static Registry<Broker> _registry;

    class _NoticeMerger {
      public:
        _NoticeMerger(CapturePredicate predicate = CapturePredicate::Default());

        void Add(const UnfNotice::StageNoticeRefPtr&);
        void Join(_NoticeMerger&);
        void Merge();
        void PostProcess();
        void Send(const PXR_NS::UsdStageWeakPtr&);

      private:
        using _NoticePtrList = std::vector<UnfNotice::StageNoticeRefPtr>;
        using _NoticePtrMap = std::unordered_map<std::string, _NoticePtrList>;

        _NoticePtrMap _noticeMap;
        CapturePredicate _predicate;
    };

    /// Usd Stage associated with broker.
    PXR_NS::UsdStageWeakPtr _stage;

    /// List of NoticeMerger objects which handle transactions.
    std::vector<_NoticeMerger> _mergers;

    /// Collection of Dispatcher instances.
    Collector<Dispatcher> _collector;

    friend class Registry<Broker>;
};

}  // namespace unf

#endif  // USD_NOTICE_FRAMEWORK_BROKER_H
