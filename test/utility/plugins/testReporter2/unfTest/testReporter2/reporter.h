#ifndef TEST_USD_NOTICE_FRAMEWORK_PLUGIN_TEST_REPORTER_2_H
#define TEST_USD_NOTICE_FRAMEWORK_PLUGIN_TEST_REPORTER_2_H

#include <unf/reporter.h>
#include <unf/notice.h>

#include <pxr/pxr.h>

namespace Test {

class TestReporter2 : public unf::Reporter {
  public:
    TestReporter2(const PXR_NS::UsdStageWeakPtr& stage) : unf::Reporter(stage)
    {
    }

    void Register();

    void _OnReceived(const unf::UnfNotice::ObjectsChanged& notice);
};

}  // namespace Test

#endif  // TEST_USD_NOTICE_FRAMEWORK_PLUGIN_TEST_REPORTER_2_H
