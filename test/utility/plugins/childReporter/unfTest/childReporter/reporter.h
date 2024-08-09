#ifndef TEST_USD_NOTICE_FRAMEWORK_PLUGIN_CHILD_REPORTER_H
#define TEST_USD_NOTICE_FRAMEWORK_PLUGIN_CHILD_REPORTER_H

#include <unf/api.h>
#include <unf/reporter.h>

#include <unfTest/notice.h>

#include <pxr/pxr.h>

namespace Test {

class ChildReporter : public unf::Reporter {
  public:
    UNF_API ChildReporter(const PXR_NS::UsdStageWeakPtr& stage)
        : unf::Reporter(stage)
    {
    }

    UNF_API void Register() override;

  private:
    void _OnReceived(const TestReporterNotice1& notice);
};

}  // namespace Test

#endif  // TEST_USD_NOTICE_FRAMEWORK_PLUGIN_CHILD_REPORTER_H
