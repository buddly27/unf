#include <unf/reporter.h>
#include <unf/inspector.h>
#include <unf/broker.h>
#include <unf/notice.h>

#include <unfTest/listener.h>
#include <unfTest/notice.h>

#include <gtest/gtest.h>
#include <pxr/pxr.h>

using Listener = ::Test::Listener<
    ::Test::TestReporterNotice1, ::Test::TestReporterNotice2,
    ::Test::ChildReporterNotice>;

TEST(ReporterPluginTest, Notice)
{
    auto stage = PXR_NS::UsdStage::CreateInMemory();
    auto inspector = unf::Inspector::Create(stage);
    auto broker = unf::Broker::Create(stage);

    Listener listener(stage);

    stage->DefinePrim(PXR_NS::SdfPath{"/A"});

    ASSERT_EQ(listener.Received<::Test::TestReporterNotice1>(), 1);
    ASSERT_EQ(listener.Received<::Test::TestReporterNotice2>(), 1);
    ASSERT_EQ(listener.Received<::Test::ChildReporterNotice>(), 1);
}