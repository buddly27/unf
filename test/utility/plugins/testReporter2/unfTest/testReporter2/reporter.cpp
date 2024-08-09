#include "reporter.h"

#include <unf/reporter.h>
#include <unf/notice.h>

#include <unfTest/notice.h>

#include <pxr/pxr.h>

PXR_NAMESPACE_USING_DIRECTIVE

namespace Test {

void TestReporter2::Register()
{
    _keys.push_back(TfNotice::Register(
        TfCreateWeakPtr(this), &TestReporter2::_OnReceived, _stage));
}

void TestReporter2::_OnReceived(const unf::UnfNotice::ObjectsChanged& notice)
{
    TestReporterNotice2().Send(_stage);
}

}  // namespace Test

TF_REGISTRY_FUNCTION(TfType)
{
    unf::ReporterDefine<Test::TestReporter2, unf::Reporter>();
}
