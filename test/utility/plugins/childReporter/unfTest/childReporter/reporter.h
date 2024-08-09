#ifndef TEST_USD_NOTICE_FRAMEWORK_PLUGIN_CHILD_REPORTER_H
#define TEST_USD_NOTICE_FRAMEWORK_PLUGIN_CHILD_REPORTER_H

#include <unf/reporter.h>

#include <unfTest/notice.h>

#include <pxr/pxr.h>

namespace Test {

class ChildReporter : public unf::Reporter {
  public:
    ChildReporter(const PXR_NS::UsdStageWeakPtr& stage) : unf::Reporter(stage)
    {
    }

    void Register();

    void _OnReceived(const TestReporterNotice1& notice);
};

}  // namespace Test

#endif  // TEST_USD_NOTICE_FRAMEWORK_PLUGIN_CHILD_REPORTER_H
