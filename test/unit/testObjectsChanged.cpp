#include <unf/broker.h>
#include <unf/notice.h>

#include <unfTest/observer.h>

#include <gtest/gtest.h>
#include <pxr/usd/sdf/layer.h>
#include <pxr/usd/sdf/path.h>
#include <pxr/usd/sdf/valueTypeName.h>
#include <pxr/usd/usd/attribute.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usd/primRange.h>
#include <pxr/usd/usd/stage.h>


class ObjectsChangedTest : public ::testing::Test {
  protected:
    void SetUp() override
    {
        _stage = PXR_NS::UsdStage::CreateInMemory();
        _broker = unf::Broker::Create(_stage);
    }

    PXR_NS::UsdStageRefPtr _stage;
    unf::BrokerPtr _broker;
};

TEST_F(ObjectsChangedTest, ResyncedObject)
{
    auto prim1 = _stage->DefinePrim(PXR_NS::SdfPath{"/Foo"});

    ::Test::Observer<unf::UnfNotice::ObjectsChanged> observer(_stage);

    auto prim2 = _stage->DefinePrim(PXR_NS::SdfPath{"/Bar"});

    ASSERT_EQ(observer.Received(), 1);

    const auto& n = observer.GetLatestNotice();
    ASSERT_FALSE(n.ResyncedObject(prim1));
    ASSERT_TRUE(n.ResyncedObject(prim2));
    ASSERT_FALSE(n.AffectedObject(prim1));
    ASSERT_TRUE(n.AffectedObject(prim2));
}

TEST_F(ObjectsChangedTest, ChangedInfoOnly)
{
    auto prim1 = _stage->DefinePrim(PXR_NS::SdfPath{"/Foo"});
    auto prim2 = _stage->DefinePrim(PXR_NS::SdfPath{"/Bar"});

    ::Test::Observer<unf::UnfNotice::ObjectsChanged> observer(_stage);

    prim2.SetMetadata(PXR_NS::TfToken{"comment"}, "This is a test");

    ASSERT_EQ(observer.Received(), 1);

    const auto& n = observer.GetLatestNotice();
    ASSERT_FALSE(n.ChangedInfoOnly(prim1));
    ASSERT_TRUE(n.ChangedInfoOnly(prim2));
    ASSERT_FALSE(n.AffectedObject(prim1));
    ASSERT_TRUE(n.AffectedObject(prim2));
}

TEST_F(ObjectsChangedTest, GetResyncedPaths)
{
    ::Test::Observer<unf::UnfNotice::ObjectsChanged> observer(_stage);

    _stage->DefinePrim(PXR_NS::SdfPath{"/Foo"});

    ASSERT_EQ(observer.Received(), 1);

    const auto& n = observer.GetLatestNotice();
    ASSERT_EQ(
        n.GetResyncedPaths(), PXR_NS::SdfPathVector{PXR_NS::SdfPath{"/Foo"}});
}

TEST_F(ObjectsChangedTest, GetChangedInfoOnlyPaths)
{
    auto prim = _stage->DefinePrim(PXR_NS::SdfPath{"/Foo"});

    ::Test::Observer<unf::UnfNotice::ObjectsChanged> observer(_stage);

    prim.SetMetadata(PXR_NS::TfToken{"comment"}, "This is a test");

    ASSERT_EQ(observer.Received(), 1);

    const auto& n = observer.GetLatestNotice();
    ASSERT_EQ(
        n.GetChangedInfoOnlyPaths(),
        PXR_NS::SdfPathVector{PXR_NS::SdfPath{"/Foo"}});
}

TEST_F(ObjectsChangedTest, GetChangedFields)
{
    auto prim = _stage->DefinePrim(PXR_NS::SdfPath{"/Foo"});

    ::Test::Observer<unf::UnfNotice::ObjectsChanged> observer(_stage);

    prim.SetMetadata(PXR_NS::TfToken{"comment"}, "This is a test");

    ASSERT_EQ(observer.Received(), 1);

    const auto& n = observer.GetLatestNotice();
    ASSERT_EQ(
        n.GetChangedFields(prim), unf::TfTokenSet{PXR_NS::TfToken{"comment"}});
    ASSERT_EQ(
        n.GetChangedFields(PXR_NS::SdfPath{"/Foo"}),
        unf::TfTokenSet{PXR_NS::TfToken{"comment"}});
    ASSERT_EQ(
        n.GetChangedFields(PXR_NS::SdfPath{"/Incorrect"}), unf::TfTokenSet{});
}

TEST_F(ObjectsChangedTest, HasChangedFields)
{
    auto prim = _stage->DefinePrim(PXR_NS::SdfPath{"/Foo"});

    ::Test::Observer<unf::UnfNotice::ObjectsChanged> observer(_stage);

    prim.SetMetadata(PXR_NS::TfToken{"comment"}, "This is a test");

    ASSERT_EQ(observer.Received(), 1);

    const auto& n = observer.GetLatestNotice();
    ASSERT_TRUE(n.HasChangedFields(prim));
    ASSERT_TRUE(n.HasChangedFields(PXR_NS::SdfPath{"/Foo"}));
    ASSERT_FALSE(n.HasChangedFields(PXR_NS::SdfPath{"/Incorrect"}));
}

TEST_F(ObjectsChangedTest, Descendants)
{
    ::Test::Observer<unf::UnfNotice::ObjectsChanged> observer(_stage);

    _stage->DefinePrim(PXR_NS::SdfPath{"/Foo/Bar"});

    ASSERT_EQ(observer.Received(), 2);

    const auto& n = observer.GetLatestNotice();
    ASSERT_EQ(n.GetResyncedPaths().at(0), PXR_NS::SdfPath{"/Foo/Bar"});
}

TEST_F(ObjectsChangedTest, Transaction)
{
    ::Test::Observer<unf::UnfNotice::ObjectsChanged> observer(_stage);

    _broker->BeginTransaction();
    _stage->DefinePrim(PXR_NS::SdfPath{"/Foo/Bar"});
    _broker->EndTransaction();

    ASSERT_EQ(observer.Received(), 1);

    const auto& n = observer.GetLatestNotice();
    ASSERT_EQ(n.GetResyncedPaths().size(), 1);
    ASSERT_EQ(n.GetResyncedPaths().at(0), PXR_NS::SdfPath{"/Foo"});
}
