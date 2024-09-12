#include "reporter.h"

#include <unf/reporter.h>

#include <unfTest/notice.h>

#include <pxr/pxr.h>

PXR_NAMESPACE_USING_DIRECTIVE

namespace Test {

void ChildReporter::Register()
{
    _keys.push_back(TfNotice::Register(
        TfCreateWeakPtr(this), &ChildReporter::_OnReceived, _stage));
}

void ChildReporter::_OnReceived(const TestReporterNotice1& notice)
{
    ChildReporterNotice().Send(_stage);
}

}  // namespace Test

TF_REGISTRY_FUNCTION(TfType)
{
    unf::ReporterDefine<Test::ChildReporter, unf::Reporter>();
}
