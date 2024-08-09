#include <unf/inspector.h>

#include <gtest/gtest.h>
#include <pxr/pxr.h>

TEST(InspectorTest, CreateMultiple)
{
    auto stage = PXR_NS::UsdStage::CreateInMemory();
    auto inspector1 = unf::Inspector::Create(stage);
    ASSERT_EQ(inspector1->GetCurrentCount(), 1);
    ASSERT_EQ(unf::Inspector::GetNumInspectors(), 1);

    auto inspector2 = unf::Inspector::Create(stage);
    ASSERT_EQ(inspector1->GetCurrentCount(), 1);
    ASSERT_EQ(inspector2->GetCurrentCount(), 1);
    ASSERT_EQ(unf::Inspector::GetNumInspectors(), 1);

    // Managers referencing the same stage are identical
    ASSERT_EQ(inspector1, inspector2);

    auto otherStage = PXR_NS::UsdStage::CreateInMemory();
    auto inspector3 = unf::Inspector::Create(otherStage);
    ASSERT_EQ(inspector1->GetCurrentCount(), 1);
    ASSERT_EQ(inspector2->GetCurrentCount(), 1);
    ASSERT_EQ(inspector3->GetCurrentCount(), 1);
    ASSERT_EQ(unf::Inspector::GetNumInspectors(), 2);

    // Managers referencing distinct stages are different
    ASSERT_NE(inspector1, inspector3);
}

TEST(InspectorTest, StageExpired1)
{
    auto stage1 = PXR_NS::UsdStage::CreateInMemory();
    auto inspector1 = unf::Inspector::Create(stage1);

    // Stage is destroyed, but inspector reference is kept in registry
    stage1.Reset();
    ASSERT_FALSE(inspector1.IsExpired());
    ASSERT_EQ(unf::Inspector::GetNumInspectors(), 1);

    // Registry reference is removed when a inspector is added
    auto stage2 = PXR_NS::UsdStage::CreateInMemory();
    auto inspector2 = unf::Inspector::Create(stage2);
    ASSERT_TRUE(inspector1.IsExpired());
    ASSERT_FALSE(inspector2.IsExpired());
    ASSERT_EQ(unf::Inspector::GetNumInspectors(), 1);
}

TEST(InspectorTest, StageExpired2)
{
    unf::InspectorWeakPtr inspector1;

    {
        auto stage1 = PXR_NS::UsdStage::CreateInMemory();
        inspector1 = unf::Inspector::Create(stage1);
    }

    // Stage is destroyed, but inspector reference is kept in registry
    ASSERT_FALSE(inspector1.IsExpired());
    ASSERT_EQ(unf::Inspector::GetNumInspectors(), 1);

    // Registry reference is removed when a inspector is added
    auto stage2 = PXR_NS::UsdStage::CreateInMemory();
    auto inspector2 = unf::Inspector::Create(stage2);
    ASSERT_TRUE(inspector1.IsExpired());
    ASSERT_FALSE(inspector2.IsExpired());
    ASSERT_EQ(unf::Inspector::GetNumInspectors(), 1);
}

TEST(InspectorTest, Reset1)
{
    auto stage1 = PXR_NS::UsdStage::CreateInMemory();
    auto inspector1 = unf::Inspector::Create(stage1);

    auto stage2 = PXR_NS::UsdStage::CreateInMemory();
    auto inspector2 = unf::Inspector::Create(stage2);

    // Explicitly reset one inspector
    inspector1->Reset();
    ASSERT_TRUE(inspector1.IsExpired());
    ASSERT_FALSE(inspector2.IsExpired());
}

TEST(InspectorTest, Reset2)
{
    auto stage1 = PXR_NS::UsdStage::CreateInMemory();
    auto inspector1 = unf::Inspector::Create(stage1);

    auto stage2 = PXR_NS::UsdStage::CreateInMemory();
    auto inspector2 = unf::Inspector::Create(stage2);

    // Explicitly reset one inspector from associated stage
    unf::Inspector::Reset(stage1);
    ASSERT_TRUE(inspector1.IsExpired());
    ASSERT_FALSE(inspector2.IsExpired());
}

TEST(InspectorTest, ResetAll)
{
    auto stage1 = PXR_NS::UsdStage::CreateInMemory();
    auto inspector1 = unf::Inspector::Create(stage1);

    auto stage2 = PXR_NS::UsdStage::CreateInMemory();
    auto inspector2 = unf::Inspector::Create(stage2);

    // Explicitly reset all inspectors
    unf::Inspector::ResetAll();
    ASSERT_TRUE(inspector1.IsExpired());
    ASSERT_TRUE(inspector2.IsExpired());
}
