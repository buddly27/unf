#ifndef TEST_USD_NOTICE_FRAMEWORK_PLUGIN_TEST_REPORTER_1_H
#define TEST_USD_NOTICE_FRAMEWORK_PLUGIN_TEST_REPORTER_1_H

#include <unf/api.h>
#include <unf/reporter.h>
#include <unf/notice.h>

#include <pxr/pxr.h>

namespace Test {

class TestReporter1 : public unf::Reporter {
  public:
    UNF_API TestReporter1(const PXR_NS::UsdStageWeakPtr& stage)
        : unf::Reporter(stage)
    {
    }

    UNF_API void Register() override;

  private:
    void _OnReceived(const unf::UnfNotice::ObjectsChanged& notice);
};

}  // namespace Test

#endif  // TEST_USD_NOTICE_FRAMEWORK_PLUGIN_TEST_REPORTER_1_H
