#include "unf/reporter.h"

#include <pxr/pxr.h>

PXR_NAMESPACE_USING_DIRECTIVE

namespace unf {

TF_REGISTRY_FUNCTION(TfType) { TfType::Define<Reporter>(); }

Reporter::Reporter(const UsdStageWeakPtr& stage) : _stage(stage) {}

Reporter::~Reporter() { Revoke(); };

void Reporter::Revoke()
{
    for (auto& key : _keys) {
        TfNotice::Revoke(key);
    }

    _keys.clear();
}

}  // namespace unf
