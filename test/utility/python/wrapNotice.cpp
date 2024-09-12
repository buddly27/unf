#include <unfTest/notice.h>

#include <pxr/base/tf/notice.h>
#include <pxr/base/tf/pyNoticeWrapper.h>
#include <pxr/base/tf/pyPtrHelpers.h>
#include <pxr/pxr.h>

#include <boost/python.hpp>

using namespace boost::python;
using namespace Test;

PXR_NAMESPACE_USING_DIRECTIVE

TF_INSTANTIATE_NOTICE_WRAPPER(TestReporterNotice1, TfNotice);
TF_INSTANTIATE_NOTICE_WRAPPER(TestReporterNotice2, TfNotice);
TF_INSTANTIATE_NOTICE_WRAPPER(ChildReporterNotice, TfNotice);

void wrapNotice()
{
    TfPyNoticeWrapper<TestReporterNotice1, TfNotice>::Wrap();
    TfPyNoticeWrapper<TestReporterNotice2, TfNotice>::Wrap();
    TfPyNoticeWrapper<ChildReporterNotice, TfNotice>::Wrap();
}
