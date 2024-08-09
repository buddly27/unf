#include "unf/broker.h"
#include "unf/capturePredicate.h"
#include "unf/dispatcher.h"
#include "unf/notice.h"

#include <pxr/base/tf/weakPtr.h>
#include <pxr/pxr.h>
#include <pxr/usd/usd/common.h>
#include <pxr/usd/usd/notice.h>

PXR_NAMESPACE_USING_DIRECTIVE

namespace unf {

// Initiate static registry.
Registry<Broker> Broker::_registry;

Broker::Broker(const UsdStageWeakPtr& stage) : _stage(stage)
{
    auto self = TfCreateWeakPtr(this);

    // Add default dispatcher.
    _collector.Add<StageDispatcher>(self);

    // Discover dispatchers added via plugin to complete or override
    // default dispatcher.
    _collector.Discover<DispatcherFactory>(self);

    // Register all dispatchers
    for (auto& element : _collector.GetItems()) {
        element.second->Register();
    }
}

BrokerPtr Broker::Create(const UsdStageWeakPtr& stage)
{
    return _registry.Add(stage);
}

bool Broker::IsInTransaction() { return _mergers.size() > 0; }

void Broker::BeginTransaction(CapturePredicate predicate)
{
    _mergers.push_back(_NoticeMerger(predicate));
}

void Broker::BeginTransaction(const CapturePredicateFunc& function)
{
    _mergers.push_back(_NoticeMerger(CapturePredicate(function)));
}

void Broker::EndTransaction()
{
    if (!IsInTransaction()) {
        return;
    }

    _NoticeMerger& merger = _mergers.back();

    // If there are only one merger left, process all notices.
    if (_mergers.size() == 1) {
        merger.Merge();
        merger.PostProcess();
        merger.Send(_stage);
    }
    // Otherwise, it means that we are in a nested transaction that should
    // not be processed yet. Join data with next merger.
    else {
        (_mergers.end() - 2)->Join(merger);
    }

    _mergers.pop_back();
}

void Broker::Send(const UnfNotice::StageNoticeRefPtr& notice)
{
    if (_mergers.size() > 0) {
        _mergers.back().Add(notice);
    }
    // Otherwise, send the notice.
    else {
        notice->Send(_stage);
    }
}

TfWeakPtr<Dispatcher> Broker::GetDispatcher(std::string identifier)
{
    return _collector.Get(identifier);
}

void Broker::Reset() { _registry.Remove(_stage); }

void Broker::ResetAll() { _registry.Clear(); }

Broker::_NoticeMerger::_NoticeMerger(CapturePredicate predicate)
    : _predicate(std::move(predicate))
{
}

void Broker::_NoticeMerger::Add(const UnfNotice::StageNoticeRefPtr& notice)
{
    // Indicate whether the notice needs to be captured.
    if (!_predicate(*notice)) return;

    // Store notices per type name, so that each type can be merged if
    // required.
    std::string name = notice->GetTypeId();
    _noticeMap[name].push_back(notice);
}

void Broker::_NoticeMerger::Join(_NoticeMerger& merger)
{
    for (auto& element : merger._noticeMap) {
        auto& source = element.second;
        auto& target = _noticeMap[element.first];

        target.reserve(target.size() + source.size());
        std::move(
            std::begin(source), std::end(source), std::back_inserter(target));

        source.clear();
    }

    merger._noticeMap.clear();
}

void Broker::_NoticeMerger::Merge()
{
    for (auto& element : _noticeMap) {
        auto& notices = element.second;

        // If there are more than one notice for this type and
        // if the notices are mergeable, we only need to keep the
        // first notice, and all other can be pruned.
        if (notices.size() > 1 && notices[0]->IsMergeable()) {
            auto& notice = notices.at(0);
            auto it = std::next(notices.begin());

            while (it != notices.end()) {
                // Attempt to merge content of notice with first notice
                // if this is possible.
                notice->Merge(std::move(**it));
                it = notices.erase(it);
            }
        }
    }
}

void Broker::_NoticeMerger::PostProcess()
{
    for (auto& element : _noticeMap) {
        auto& notice = element.second[0];
        notice->PostProcess();
    }
}

void Broker::_NoticeMerger::Send(const UsdStageWeakPtr& stage)
{
    for (auto& element : _noticeMap) {
        auto& notices = element.second;

        // Send all remaining notices.
        for (const auto& notice : element.second) {
            notice->Send(stage);
        }
    }
}

}  // namespace unf
