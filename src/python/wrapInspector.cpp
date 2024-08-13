#include "unf/inspector.h"

#include <pxr/base/tf/makePyConstructor.h>
#include <pxr/base/tf/pyPtrHelpers.h>
#include <pxr/base/tf/weakPtr.h>
#include <pxr/pxr.h>

#include <boost/python.hpp>

using namespace boost::python;
using namespace unf;

PXR_NAMESPACE_USING_DIRECTIVE

void wrapInspector()
{
    class_<Inspector, InspectorWeakPtr, boost::noncopyable>(
        "Inspector",
        "Intermediate object handling the creation and management of Reporter "
        "instances per stage",
        no_init)

        .def(TfPyRefAndWeakPtr())

        .def(
            "Create",
            &Inspector::Create,
            arg("stage"),
            "Create an inspector from a Usd Stage.",
            return_value_policy<TfPyRefPtrFactory<> >())
        .staticmethod("Create")

        .def(
            "GetNumReporters",
            &Inspector::GetNumReporters,
            "Return number of reporters created")

        .def(
            "GetNumInspectors",
            &Inspector::GetNumInspectors,
            "Return number of inspector created")
        .staticmethod("GetNumInspectors")

        .def(
            "Reset",
            (void (Inspector::*)()) & Inspector::Reset,
            "Un-register inspector")

        .def("ResetAll", &Inspector::ResetAll, "Un-register all inspectors")
        .staticmethod("ResetAll");
}
