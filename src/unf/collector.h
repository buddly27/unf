#ifndef USD_NOTICE_FRAMEWORK_COLLECTOR_H
#define USD_NOTICE_FRAMEWORK_COLLECTOR_H

#include <pxr/base/plug/plugin.h>
#include <pxr/base/plug/registry.h>
#include <pxr/base/tf/refPtr.h>
#include <pxr/base/tf/weakPtr.h>
#include <pxr/pxr.h>

#include <string>
#include <typeinfo>
#include <unordered_map>

namespace unf {

class Broker;
class Inspector;

template <typename T>
using CollectorItemMap = std::unordered_map<std::string, PXR_NS::TfRefPtr<T>>;

/// \class Collector
///
/// Generic class for collecting and managing items.
template <typename T>
class Collector {
  public:
    virtual ~Collector() = default;

    /// Return internal map of collected items.
    CollectorItemMap<T>& GetItems() { return _items; }

    /// Returns the current number of items collected.
    size_t Size() { return _items.size(); }

    /// \brief
    /// Retrieves an item of a specific subtype from the collector.
    ///
    /// This method searches for an item of type `U` within the
    /// collector. If found, it returns a weak pointer to the item.
    /// Otherwise, it returns `nullptr`.
    template <class U>
    PXR_NS::TfWeakPtr<U> Get()
    {
        static_assert(std::is_base_of<T, U>::value);
        auto it = _items.find(typeid(U).name());
        return it != _items.end()
                   ? PXR_NS::TfStatic_cast<PXR_NS::TfWeakPtr<U>>(it->second)
                   : nullptr;
    }

    /// \brief
    /// Retrieves an item by its \p identifier from the collector.
    ///
    /// This method searches for an item with the specified identifier
    /// within the collector. If found, it returns a weak pointer to the
    /// item. Otherwise, it returns `nullptr`.
    PXR_NS::TfWeakPtr<T> Get(const std::string& identifier)
    {
        auto it = _items.find(identifier);
        return it != _items.end()
                   ? PXR_NS::TfStatic_cast<PXR_NS::TfWeakPtr<T>>(it->second)
                   : nullptr;
    }

    /// \brief
    /// Adds a new item of a specific subtype to the collector.
    ///
    /// This method creates a new item of type `U`, adds it to the
    /// collector, and returns a reference pointer to the newly added item.
    template <class U, class... Args>
    PXR_NS::TfRefPtr<T> Add(Args&&... args)
    {
        static_assert(std::is_base_of<T, U>::value);
        auto item = PXR_NS::TfCreateRefPtr(new U(std::forward<Args>(args)...));
        _Add(item);
        return item;
    }

    /// \brief
    /// Discovers and loads items from plugins based on their type.
    ///
    /// This method discovers all types derived from `T` and attempts to
    /// load them from plugins using the specified `ItemFactory`.
    /// If successful, the items are added to the collector.
    template <class ItemFactory, class... Args>
    void Discover(Args&&... args)
    {
        auto root = PXR_NS::TfType::Find<T>();
        std::set<PXR_NS::TfType> types;
        PXR_NS::PlugRegistry::GetAllDerivedTypes(root, &types);

        for (const auto& type : types) {
            _LoadFromPlugins<ItemFactory>(type, std::forward<Args>(args)...);
        }
    }

  private:
    Collector() = default;

    /// \brief
    /// Adds an item to the internal map of the collector.
    ///
    /// This method adds the given item to the internal map, using the item's
    /// identifier as the key.
    void _Add(const PXR_NS::TfRefPtr<T>& item)
    {
        _items[item->GetIdentifier()] = item;
    }

    /// \brief
    /// Loads items plugins based on a targeted \p type.
    ///
    /// This method attempts to load items of the specified type from plugins
    /// using the provided factory. If the plugin is successfully loaded and
    /// the item is created, it is added to the collector.
    template <class ItemFactory, class... Args>
    void _LoadFromPlugins(const PXR_NS::TfType& type, Args&&... args)
    {
        PXR_NAMESPACE_USING_DIRECTIVE

        const auto plugin =
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

        PXR_NS::TfRefPtr<T> item;
        ItemFactory* factory = type.GetFactory<ItemFactory>();

        if (factory) {
            item = factory->New(std::forward<Args>(args)...);
        }

        if (!item) {
            TF_CODING_ERROR(
                "Failed to manufacture %s from plugin %s",
                type.GetTypeName().c_str(),
                plugin->GetName().c_str());
            return;
        }

        _Add(item);
    }

    /// Internal data.
    CollectorItemMap<T> _items;

    friend Broker;
    friend Inspector;
};

}  // namespace unf

#endif  // USD_NOTICE_FRAMEWORK_COLLECTOR_H
