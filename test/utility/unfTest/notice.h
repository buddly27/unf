#ifndef TEST_USD_NOTICE_FRAMEWORK_NOTICE_H
#define TEST_USD_NOTICE_FRAMEWORK_NOTICE_H

#include <unf/api.h>
#include <unf/notice.h>

#include <pxr/pxr.h>

#include <string>
#include <unordered_map>

namespace Test {

using DataMap = std::unordered_map<std::string, std::string>;

// Notice with can be consolidated within broker transactions.
class MergeableNotice
    : public unf::UnfNotice::StageNoticeImpl<MergeableNotice> {
  public:
    UNF_API MergeableNotice() = default;
    UNF_API MergeableNotice(const DataMap& data);

    UNF_API MergeableNotice(const MergeableNotice& other);

    UNF_API MergeableNotice& operator=(const MergeableNotice& other);

    UNF_API virtual ~MergeableNotice() = default;

    // Bring all Merge declarations from base class to prevent
    // overloaded-virtual warning.
    using unf::UnfNotice::StageNoticeImpl<MergeableNotice>::Merge;

    UNF_API virtual void Merge(MergeableNotice&& notice) override;

    UNF_API const DataMap& GetData() const;

  private:
    DataMap _data;
};

// Notice which cannot be consolidated within broker transactions.
class UnMergeableNotice
    : public unf::UnfNotice::StageNoticeImpl<UnMergeableNotice> {
  public:
    UNF_API UnMergeableNotice() = default;
    UNF_API virtual ~UnMergeableNotice() = default;

    UNF_API virtual bool IsMergeable() const;
};

// Declare notices used by the test dispatchers.
class InputNotice : public PXR_NS::TfNotice {
  public:
    UNF_API InputNotice();
};

class OutputNotice1 : public unf::UnfNotice::StageNoticeImpl<OutputNotice1> {
  public:
    UNF_API OutputNotice1(const InputNotice&);
};

class OutputNotice2 : public unf::UnfNotice::StageNoticeImpl<OutputNotice2> {
  public:
    UNF_API OutputNotice2(const InputNotice&);
};

class TestReporterNotice1 : public PXR_NS::TfNotice {
  public:
    UNF_API TestReporterNotice1(std::string msg) : _msg("TestReporterNotice1")
    {
    }
    UNF_API virtual ~TestReporterNotice1() = default;

    UNF_API const std::string& GetMsg() const { return _msg; }

  private:
    std::string _msg;
};

class TestReporterNotice2 : public PXR_NS::TfNotice {
  public:
    UNF_API TestReporterNotice2() : _msg("TestReporterNotice2") {}
    UNF_API virtual ~TestReporterNotice2() = default;

    UNF_API const std::string& GetMsg() const { return _msg; }

  private:
    std::string _msg;
};

class ChildReporterNotice : public PXR_NS::TfNotice {
  public:
    UNF_API ChildReporterNotice() : _msg("ChildReporterNotice") {}
    UNF_API virtual ~ChildReporterNotice() = default;

  private:
    std::string _msg;
};

}  // namespace Test

#endif  // TEST_USD_NOTICE_FRAMEWORK_NOTICE_H
