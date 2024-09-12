#ifndef USD_NOTICE_FRAMEWORK_REGISTRY_H
#define USD_NOTICE_FRAMEWORK_REGISTRY_H

#include <pxr/base/tf/refPtr.h>
#include <pxr/pxr.h>
#include <pxr/usd/usd/common.h>
#include <pxr/usd/usd/stage.h>

#include <boost/functional/hash.hpp>

#include <unordered_map>

namespace unf {

class Broker;
class Inspector;

/// \class Registry
///
/// Generic class that manages and stored reference pointers associated
/// with a USD Stage.
template <typename T>
class Registry {
  public:
    /// Returns the current number of entries in the registry.
    size_t Size() { return _map.size(); }

    /// \brief
    /// Add a new entry to the registry and return it.
    ///
    /// If the stage is not already in the registry, a new instance will
    /// be created and added. Otherwise, the existing entry is returned.
    ///
    /// Entries with expired stages will be cleared as part of this process.
    PXR_NS::TfRefPtr<T> Add(const PXR_NS::UsdStageWeakPtr& stage)
    {
        ClearExpired();

        if (_map.find(stage) == _map.end()) {
            _map[stage] = PXR_NS::TfCreateRefPtr(new T(stage));
        }

        return _map.at(stage);
    }

    /// Removes the entry associated with the given \p stage.
    void Remove(const PXR_NS::UsdStageWeakPtr& stage) { _map.erase(stage); }

    /// Clears all entries in the registry.
    void Clear() { _map.clear(); }

    /// Clears all entries associated with expired stages.
    void ClearExpired()
    {
        for (auto it = _map.begin(); it != _map.end();) {
            // If the stage doesn't exist anymore, clear the registry
            // for associated entry
            if (it->first.IsExpired()) {
                it = _map.erase(it);
            }
            else {
                it++;
            }
        }
    }

  private:
    Registry() = default;

    // Custom hasher for UsdStageWeakPtr
    struct UsdStageWeakPtrHasher {
        std::size_t operator()(const PXR_NS::UsdStageWeakPtr& ptr) const
        {
            return hash_value(ptr);
        }
    };

    using MapType = std::unordered_map<
        PXR_NS::UsdStageWeakPtr, PXR_NS::TfRefPtr<T>, UsdStageWeakPtrHasher>;

    /// Internal data.
    MapType _map;

    friend Broker;
    friend Inspector;
};

}  // namespace unf

#endif  // USD_NOTICE_FRAMEWORK_REGISTRY_H
