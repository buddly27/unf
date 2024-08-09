#include <unf/reporterManager.h>

#include <gtest/gtest.h>
#include <pxr/pxr.h>

TEST(ReporterManagerTest, CreateMultiple)
{
    auto stage = PXR_NS::UsdStage::CreateInMemory();
    auto manager1 = unf::ReporterManager::Create(stage);
    ASSERT_EQ(manager1->GetCurrentCount(), 1);
    ASSERT_EQ(unf::ReporterManager::GetNumManagers(), 1);

    auto manager2 = unf::ReporterManager::Create(stage);
    ASSERT_EQ(manager1->GetCurrentCount(), 1);
    ASSERT_EQ(manager2->GetCurrentCount(), 1);
    ASSERT_EQ(unf::ReporterManager::GetNumManagers(), 1);

    // Managers referencing the same stage are identical
    ASSERT_EQ(manager1, manager2);

    auto otherStage = PXR_NS::UsdStage::CreateInMemory();
    auto manager3 = unf::ReporterManager::Create(otherStage);
    ASSERT_EQ(manager1->GetCurrentCount(), 1);
    ASSERT_EQ(manager2->GetCurrentCount(), 1);
    ASSERT_EQ(manager3->GetCurrentCount(), 1);
    ASSERT_EQ(unf::ReporterManager::GetNumManagers(), 2);

    // Managers referencing distinct stages are different
    ASSERT_NE(manager1, manager3);
}

TEST(ReporterManagerTest, StageExpired1)
{
    auto stage1 = PXR_NS::UsdStage::CreateInMemory();
    auto manager1 = unf::ReporterManager::Create(stage1);

    // Stage is destroyed, but manager reference is kept in registry
    stage1.Reset();
    ASSERT_FALSE(manager1.IsExpired());
    ASSERT_EQ(unf::ReporterManager::GetNumManagers(), 1);

    // Registry reference is removed when a manager is added
    auto stage2 = PXR_NS::UsdStage::CreateInMemory();
    auto manager2 = unf::ReporterManager::Create(stage2);
    ASSERT_TRUE(manager1.IsExpired());
    ASSERT_FALSE(manager2.IsExpired());
    ASSERT_EQ(unf::ReporterManager::GetNumManagers(), 1);
}

TEST(ReporterManagerTest, StageExpired2)
{
    unf::ReporterManagerWeakPtr manager1;

    {
        auto stage1 = PXR_NS::UsdStage::CreateInMemory();
        manager1 = unf::ReporterManager::Create(stage1);
    }

    // Stage is destroyed, but manager reference is kept in registry
    ASSERT_FALSE(manager1.IsExpired());
    ASSERT_EQ(unf::ReporterManager::GetNumManagers(), 1);

    // Registry reference is removed when a manager is added
    auto stage2 = PXR_NS::UsdStage::CreateInMemory();
    auto manager2 = unf::ReporterManager::Create(stage2);
    ASSERT_TRUE(manager1.IsExpired());
    ASSERT_FALSE(manager2.IsExpired());
    ASSERT_EQ(unf::ReporterManager::GetNumManagers(), 1);
}

TEST(ReporterManagerTest, Reset1)
{
    auto stage1 = PXR_NS::UsdStage::CreateInMemory();
    auto manager1 = unf::ReporterManager::Create(stage1);

    auto stage2 = PXR_NS::UsdStage::CreateInMemory();
    auto manager2 = unf::ReporterManager::Create(stage2);

    // Explicitly reset one manager
    manager1->Reset();
    ASSERT_TRUE(manager1.IsExpired());
    ASSERT_FALSE(manager2.IsExpired());
}

TEST(ReporterManagerTest, Reset2)
{
    auto stage1 = PXR_NS::UsdStage::CreateInMemory();
    auto manager1 = unf::ReporterManager::Create(stage1);

    auto stage2 = PXR_NS::UsdStage::CreateInMemory();
    auto manager2 = unf::ReporterManager::Create(stage2);

    // Explicitly reset one manager from associated stage
    unf::ReporterManager::Reset(stage1);
    ASSERT_TRUE(manager1.IsExpired());
    ASSERT_FALSE(manager2.IsExpired());
}

TEST(ReporterManagerTest, ResetAll)
{
    auto stage1 = PXR_NS::UsdStage::CreateInMemory();
    auto manager1 = unf::ReporterManager::Create(stage1);

    auto stage2 = PXR_NS::UsdStage::CreateInMemory();
    auto manager2 = unf::ReporterManager::Create(stage2);

    // Explicitly reset all managers
    unf::ReporterManager::ResetAll();
    ASSERT_TRUE(manager1.IsExpired());
    ASSERT_TRUE(manager2.IsExpired());
}
