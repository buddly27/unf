#include <unf/reporter.h>
#include <unf/inspector.h>
#include <unf/broker.h>
#include <unf/notice.h>

#include <unfTest/childReporter/reporter.h>
#include <unfTest/testReporter1/reporter.h>
#include <unfTest/testReporter2/reporter.h>
#include <unfTest/listener.h>
#include <unfTest/notice.h>

#include <gtest/gtest.h>
#include <pxr/pxr.h>

using Listener = ::Test::Listener<
    ::Test::TestReporterNotice1, ::Test::TestReporterNotice2,
    ::Test::ChildReporterNotice>;

TEST(ReporterTest, AddReporters)
{
    auto stage1 = PXR_NS::UsdStage::CreateInMemory();
    auto inspector1 = unf::Inspector::Create(stage1);
    size_t reporterNb1 = inspector1->GetNumReporters();

    inspector1->AddReporter<::Test::TestReporter1>();
    inspector1->AddReporter<::Test::ChildReporter>();

    ASSERT_EQ(inspector1->GetNumReporters(), reporterNb1 + 2);
    ASSERT_EQ(unf::Inspector::GetNumInspectors(), 1);

    inspector1->AddReporter<::Test::TestReporter2>();
    ASSERT_EQ(inspector1->GetNumReporters(), reporterNb1 + 3);

    auto stage2 = PXR_NS::UsdStage::CreateInMemory();
    auto inspector2 = unf::Inspector::Create(stage2);
    size_t reporterNb2 = inspector2->GetNumReporters();
    ASSERT_NE(reporterNb2, inspector1->GetNumReporters());
    ASSERT_EQ(unf::Inspector::GetNumInspectors(), 2);

    inspector2->AddReporter<::Test::ChildReporter>();
    ASSERT_EQ(inspector1->GetNumReporters(), reporterNb1 + 3);
    ASSERT_EQ(inspector2->GetNumReporters(), reporterNb2 + 1);
}

TEST(ReporterTest, GetReporter)
{
    auto stage = PXR_NS::UsdStage::CreateInMemory();
    auto inspector = unf::Inspector::Create(stage);
    inspector->AddReporter<::Test::TestReporter1>();

    ASSERT_NE(inspector->GetReporter<::Test::TestReporter1>(), nullptr);
    ASSERT_EQ(inspector->GetReporter<::Test::TestReporter2>(), nullptr);
}

TEST(ReporterTest, Notice)
{
    auto stage = PXR_NS::UsdStage::CreateInMemory();
    auto inspector = unf::Inspector::Create(stage);
    auto broker = unf::Broker::Create(stage);

    Listener listener(stage);

    stage->DefinePrim(SdfPath{"/A"});

    ASSERT_EQ(listener.Received<::Test::TestReporterNotice1>(), 0);
    ASSERT_EQ(listener.Received<::Test::TestReporterNotice2>(), 0);
    ASSERT_EQ(listener.Received<::Test::ChildReporterNotice>(), 0);

    inspector->AddReporter<::Test::TestReporter1>();

    stage->DefinePrim(SdfPath{"/B"});

    ASSERT_EQ(listener.Received<::Test::TestReporterNotice1>(), 1);
    ASSERT_EQ(listener.Received<::Test::TestReporterNotice2>(), 0);
    ASSERT_EQ(listener.Received<::Test::ChildReporterNotice>(), 0);

    inspector->AddReporter<::Test::TestReporter2>();

    stage->DefinePrim(SdfPath{"/C"});

    ASSERT_EQ(listener.Received<::Test::TestReporterNotice1>(), 2);
    ASSERT_EQ(listener.Received<::Test::TestReporterNotice2>(), 1);
    ASSERT_EQ(listener.Received<::Test::ChildReporterNotice>(), 0);

    inspector->AddReporter<::Test::ChildReporter>();

    stage->DefinePrim(SdfPath{"/D"});

    ASSERT_EQ(listener.Received<::Test::TestReporterNotice1>(), 3);
    ASSERT_EQ(listener.Received<::Test::TestReporterNotice2>(), 2);
    ASSERT_EQ(listener.Received<::Test::ChildReporterNotice>(), 1);
}
